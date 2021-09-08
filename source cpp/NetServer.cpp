#include "NetServer.h"
#include "sha256.h"
#include "NetfileManager.h"


struct DL {

	unsigned char id[32]; // UNIQUE IDENTIFIER OF THE DL. a ts hash with random value
	uint8_t  flag; // FLAG BYTE OF THE DL
	uint32_t ini_timestamp;// TIMESTAMP AT LAST RCVED TIME
	uint32_t data_size; // TOTAL DATA SIZE
	uint32_t rcv_size; // TOTAL RCV SIZE
	unsigned char* dataptr; // PTR USE IF DATASIZE IS LOWER THAN 
	//uint32_t current_offset; // CURRENT OFFSET 
	//uint32_t current_chunk_size;
};


DL * DLPTRS [MAX_DL];
TCPServer* server;


void PrintDLinformation(int index) 
{
	if (DLPTRS[index] == NULL) {
		std::cout << "DLL not existing...";
		return;
	}
	std::cout << "-_-_-_-_-_-_-_-_ -DLL info_-_-_-_-_-_-_-" << std::endl;
	std::cout << "Index          : " << index << std::endl;
	std::cout << "ID             : ";
	
	printHash(DLPTRS[index]->id);
	std::cout << "Flag           : " << (int)DLPTRS[index]->flag << std::endl;
	std::cout << "Last Timestamp : " << DLPTRS[index]->ini_timestamp << std::endl;
	std::cout << "Data Size      : " << DLPTRS[index]->data_size << std::endl;
	std::cout << "Rcv  Size      : " << DLPTRS[index]->rcv_size << std::endl;
	std::cout << "-_-_-_-_-_-_-_-_ -_-_-_-_-_-_-_-_-_-_-" << std::endl;
}
void OnServerReceive(TCPServer* listen_server, SOCKET client, unsigned char* data, int datalength)
{
	//std::cout << "received first byte " << (int)data[0] << "length was " << datalength << std::endl;
	if (data[0] == 1) {
		//ASK NETWORK_TIME [FLAG 01]
		unsigned char buffer[5];
		buffer[0] = 1; // ANS NETWORK_TIME FLAG
		UintToBytes(GetTimeStamp(), buffer + 1);
		listen_server->Send(client, buffer, 4);
	}
	if (data[0] == 2) {

		RefreshDLs();
		int nptr = GetFreeDLPointer();
		if (nptr < 0 || currentQPtr == MAX_QFILE) {
			std::cout << "Cannot accept DL anymore. " << std::endl;
			return;
		}
		if (datalength < 38) {
			std::cout << "Insuffisant bytes for new DL " << std::endl;
			return;
		}
		DLPTRS[nptr] = new DL;
		DLPTRS[nptr]->rcv_size = 0;
		DLPTRS[nptr]->flag = data[1];
		DLPTRS[nptr]->data_size = BytesToUint(data + 34);
		printHash(data + 2); // receiving bad sha...
		memcpy(DLPTRS[nptr]->id, data + 2, 32); // copying not working ?
		DLPTRS[nptr]->ini_timestamp = GetTimeStamp();
		if (DLPTRS[nptr]->data_size <= MAX_DL_ALLOCATION) {
			DLPTRS[nptr]->dataptr = (unsigned char*)malloc(BytesToUint(data + 34));
		}
		else {
			DLPTRS[nptr]->dataptr = NULL;
			//  create recipient file
		}
		std::cout << "New download started" << std::endl;


	}
	if (data[0] == 3) {
		// RECEIVE DL
		if (datalength < 41) {
			std::cout << "Insuffisant bytes for  DL reception " << std::endl;
			return;
		}
		/*
			
			UL  DATA[FLAG 03]
			id				32 +1
			curroffset		4  +33
			currchunksize	4  +37
			data		currchunksize +41
			
		*/
		printHash(data + 1);
		int dpptr = GetDlPointerByID(data + 1);
		uint32_t curroffset = BytesToUint(data + 33);
		uint32_t currchunksize = BytesToUint(data + 37);
		if (dpptr == -1) {
			std::cout << "Unknown ID for DL proccess" << std::endl;
			return;
		}
		if (currchunksize != datalength - 41) {
			std::cout << "Missing bytes during DL proccess" << std::endl;
			return;
		}
		
		if (DLPTRS[dpptr]->data_size <= MAX_DL_ALLOCATION) {
			if (DLPTRS[dpptr]->data_size < curroffset + currchunksize) {
				std::cout << "Overflow. Cannot proccess DL" << std::endl;
				return;
			}
			unsigned char* dataptr = DLPTRS[dpptr]->dataptr + curroffset;
			memcpy(dataptr, data + 41, currchunksize);
			
			
		}
		else 
		{
			// craft new part of recipient file 
		}
		// update ts and rcv size 
		DLPTRS[dpptr]->ini_timestamp = GetTimeStamp();
		DLPTRS[dpptr]->rcv_size += currchunksize;
		
		// check if DL is finished 
		if (DLPTRS[dpptr]->rcv_size == DLPTRS[dpptr]->data_size) 
		{
			std::cout << "DLL terminated!" << std::endl;

			// [0] Build file if needed ...
			if (DLPTRS[dpptr]->data_size <= MAX_DL_ALLOCATION) {
				char hashpath[255];
				GetHashString(data + 1, hashpath);
				std::ostringstream s;
				s << "tmp\\" << hashpath;
				std::string ss = s.str();
				const char* bname = ss.c_str();

				FILE* f = fopen(bname, "ab");
				if (f == NULL) { return; }
				fwrite(DLPTRS[dpptr]->dataptr, 1, DLPTRS[dpptr]->data_size, f);
				fclose(f);
				std::cout << "DL file created :" << bname << std::endl;
			}
			else {
				// [0] rebuild crafted file.
			}
			// [1] Add to pending TX process or pending Block process lists. 
			AddQFile(DLPTRS[dpptr]->id, DLPTRS[dpptr]->flag);
			// [2] Delete DL
			StopDL(dpptr);
			while ( 1){}
		}
		
		
	}
}

void StopServer() {
    delete server;  //also close all sockets
}

void StartServer(const char* filePath) 
{
	// path is net.ini at root searching line peer:
	FILE* f = fopen(filePath, "rb");
	if (f == NULL) {
		return;
	}
	// reading line by line, max 256 bytes
	char buffer[256];
	bool lfound = false;
	while (fgets(buffer, 256, f)) {
		if (lfound) {
			char* ipstr, * portstr;
			ipstr = strtok(buffer, ":");
			portstr = strtok(NULL, ":");
			int port = atoi(portstr);
			server = new TCPServer(ipstr, port, OnServerReceive);
			if (server->Init())
			{
				server->RunInThread();
			}
			break;
		}
		if (strstr(buffer, "peer:") != NULL) {

			lfound = true;
		}
	}

	fclose(f);
}
void StartServer(std::string IP, int port)
{
    server = new TCPServer(IP, 54000, OnServerReceive);
    if (server->Init())
    {
        server->RunInThread();
    }
}




/*
  ---------------------------------------------------------------------------------
								server -> client
ANS NETWORK_TIME [FLAG 01]



								client -> server

ASK NETWORK_TIME [FLAG 01]
	-		0
ASK NEW_UL       [FLAG 02]
	flag			1 +1
	id				32 (currently its is ts hash with a random number)
	total size		4

UL  DATA	 [FLAG 03]
	id				32
	curroffset		4
	currchunksize	4
	data		currchunksize

---------------------------------------------------------------------------------

LISTENING DATA. WICH CAN BE EITHER BLOCKSDATA OR PENDING TRANSACTION, OR SOMETHING ELSE. ACCEPT OR NOT DOWNLOAD. MUTE CLIENT ETC.

	DOWNLOADS
			DOWNLOAD ID ( UNIQUE )
			DOWNLOAD TOTAL SIZE    : data total size  send by the peer
			DOWNLOAD TOTALRECEIVED : data incrementing every time a chunk is received.
			DOWNLOAD CURRENT OFFSET : data at current offset sent by the peer
			DOWNLOAD CURRENT CHUNK SIZE : the current data sent by the peer

		DOWNLOAD AUTO CLOSE AFTER 1MINUTE NO RESPONSE
		ACCEPT DOWNLOAD
		PROCCESS DOWNLOAD
		DELETE DOWNLOAD
		MAXDOWNLOAD
		MAXDOWNLOADSIZE

	PEERS.
		CONNECT TO PEERS SERVER SO CREATE A NEW PEER ...
		BROADCASTFILE ( DATAFLAG, FILE ) AS DATACHUNK WHICH CONTAINS
														ID
														TOTAL SIZE
														CURRENT OFFSET
														CURRENT CHUNK SIZE

*/

int ConcatenateDLChunk() {
	return 0;
}

int GetDlPointerByID(unsigned char * id) 
{
	for (int i = 0; i < MAX_DL; i++) {
		if (DLPTRS[i] != NULL) {
			if (memcmp(DLPTRS[i]->id, id, 32) == 0) {
				return i;
			}
		}
	}
	return -1;
}

void StopDL(int index) 
{
	
	// delete allocate memory if needed 
	if (DLPTRS[index]->data_size <= MAX_DL_ALLOCATION) {
		free(DLPTRS[index]->dataptr);
	}
	delete DLPTRS[index];
	// nullify pointer
	DLPTRS[index] = NULL;
}
void RefreshDLs() 
{
	int cts = GetTimeStamp();
	for (int i = 0; i < MAX_DL; i++) {
		if (DLPTRS[i] != NULL) {
			if (cts - DLPTRS[i]->ini_timestamp > MAX_DL_SUSPENDTIME) {
				StopDL(i);
			}
		}
	}
}
int GetFreeDLPointer() 
{
	for (int i = 0; i < MAX_DL; i++) {
		if (DLPTRS[i] == NULL) {
			return i;
		}
	}
	return -1;
}