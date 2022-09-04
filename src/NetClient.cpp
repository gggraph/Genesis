#include "NetClient.h"
#include "Utils.h"
#include<string.h> 
#include "sha256.h"
#include "consensus.h"

#define MAXPEERS 16
#define ULCHUNKSIZE 4096

TCPClient* peers[MAXPEERS]; // Array of TCPClient Pointers


struct UL {
	// Waiting 30 seconds to get all permissions, then broadcast to all of them?
	bool   _hasPermission[MAXPEERS]; // permission flag get from ASK_PERMISSION. 
	unsigned char id[32];			 // UNIQUE IDENTIFIER OF THE UL.
	uint32_t ini_timestamp;          // TimeStamp At Creation.
	char   _filePath[255];			 // filePath to the file to broadcast.
	char   _flag;					 // type of data to send (blocks, txs, etc... ) 
	
};


void DataReceived(unsigned char* data, int datasize)
{
	if (data[0] == 1) {
		//ANS NETWORK_TIME [FLAG 01]
		uint32_t ans_ts = BytesToUint(data + 1);
		std::cout << "GET NETWORK TIME FROM SERVER : " << ans_ts << std::endl;
	}
	
}

void ConnectToPeerList(const char *  filePath)
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
			char * ipstr,* portstr;
			ipstr = strtok(buffer, ":");
			portstr = strtok(NULL, ":");
			int port = atoi(portstr);
			ConnectToPeer(ipstr, port);
		}
		if (strstr(buffer, "peer:") != NULL) {
			
			lfound = true;
		}
	}
		
	fclose(f);
}
void ConnectToPeer(std::string ServerIP, int port)
{
	int nindex = GetFreePeerPointer();
	if (nindex == -1)
	{
		std::cout << "Max Peer connection achieved. Cannot Connect more. " << std::endl;
		return;
	}
	peers[nindex] = new TCPClient;
	if (peers[nindex]->Connect(ServerIP, port))
	{
		peers[nindex]->ListenRecvInThread(DataReceived);
		std::cout << "Connected client." << std::endl;

	}
	else {
		std::cout << "Connect() fail." << std::endl;
	}

}

int GetFreePeerPointer()
{
	for (int i = 0; i < MAXPEERS; i++) {
		if (peers[i] == NULL) {
			return i;
		}
	}
	return -1;
}

void SendToAllPeers(unsigned char* data, int datalength) 
{
	for (int i = 0; i < MAXPEERS; i++) {
		if (peers[i] != NULL) {
			PeerSend(i, data, datalength);
		}
	}
}
void PeerSend(int peerIndex, unsigned char* data, int datalength)
{
	if (peers[peerIndex] == NULL) {
		std::cout << "WRONG PEER INDEX";
		return;
	}
	peers[peerIndex]->Send(data, datalength);
}

void DisconnectAll() {
	for (int i = 0; i < MAXPEERS; i++) {
		if (peers[i] != NULL) {
			DisconnectPeer(i);
		}
	}
}
void DisconnectPeer(int peerIndex)
{
	delete peers[peerIndex];
}

void BroadcastOfficialBlocks(uint32_t firstindex, uint32_t count) // count is lowered if last index is above firstindex + count
{
	// Get Total Size of whole blocks 
	if (firstindex == 0) {
		std::cout << "FORBIDDEN. cannot send genesis block";
		return;
	}
	uint32_t lbi = GetLatestBlockIndex(true);
	if (firstindex + count > lbi+1) {
		count = (lbi+1) - firstindex;
	}
	
	 /*
		HEADER COOKING : 
		first index (4b)
		last index  (4b)
		blocks ptr  (4b*number of blocks)
	 */
	// will i need to create a file ? seems gross . ULCHUNK IS 4096
	uint32_t totalSize = 8 + 4 * count;
	uint32_t trackCounter = 0;
	uint32_t headerSize = 8 + 4 * count;
	unsigned char* header = (unsigned char*)malloc(headerSize);
	UintToBytes(firstindex, header);
	UintToBytes(firstindex + count - 1  , header + 4);
	// update first 8 bytes


	// need to prepare custom header block files 
	for (int i = firstindex; i < firstindex + count; i++) 
	{
		UintToBytes(totalSize, header + 8 + (trackCounter * 4));

		unsigned char * b = GetOfficialBlock(i);
		// [0] GET THE BLOCK SIZE
		uint32_t bsize = 115; // 113 min bytes + txn size
		if (GetMinerTokenFlag(b)) {
			bsize += 60;
		}
		for (int i = 0; i < GetTransactionNumber(b); i++) {
			unsigned char* TX = GetBlockTransaction(b, i);
			bsize += 85; 
			bsize += GetTXDataSize(TX);
		}
		totalSize += bsize;
		trackCounter++;
		free(b);
		
	}
	// Ask Permissions
	unsigned char data[41];
	uint32_t ts = GetTimeStamp();
	int rd = 0;
	memcpy(data, &rd, 4);
	memcpy(data + 4, &ts, 4);
	Sha256.init();
	Sha256.write((char*)data, 8);
	unsigned char hash[32];
	memcpy(hash, Sha256.result(), 32);
	memcpy(data + 2, hash, 32);
	data[0] = 2; // flag byte of packet
	data[1] = 1; // flag byte of data 
	UintToBytes(totalSize, data + 34); // tell datasize 
	// we should do some UL Management like DL in server cpp ( with permission flag etc. ) 
	SendToAllPeers(data, 38);

	// [?] Wait for Permissions
	
	// Broadcast
	unsigned char packet[ULCHUNKSIZE];

	uint32_t bOff = 0;
	uint32_t cBlockIndex = firstindex;
	uint32_t cBlockOff = 0;

	// first send the header in one packet ... 
	packet[0] = 3; // flag byte of packet
	memcpy(packet + 1, hash, 32); // copy id
	UintToBytes(0, packet + 33); // copy currentoffset
	UintToBytes(41 + headerSize, packet + 37); // can get error if headerptr reach 4096 bytes
	memcpy(packet + 41, header, 8 + (4 * count));
	SendToAllPeers( packet, 49 + (4 * count) );
	
	while (bOff < totalSize)
	{
		unsigned char* b = GetOfficialBlock(cBlockIndex);
		// [0] GET THE BLOCK SIZE
		uint32_t bsize = 115; // 113 min bytes + txn size
		if (GetMinerTokenFlag(b)) {
			bsize += 60;
		}
		for (int i = 0; i < GetTransactionNumber(b); i++) {
			unsigned char* TX = GetBlockTransaction(b, i);
			bsize += 85;
			bsize += GetTXDataSize(TX);
		}
		uint32_t packetsize = ULCHUNKSIZE - 41;
		if (packetsize > bsize - cBlockOff) {
			packetsize = bsize - cBlockOff;
			memcpy(packet + 41, b + cBlockOff, packetsize);
			cBlockIndex++; 
			cBlockOff = 0;
		}
		else {
			memcpy(packet + 41, b + cBlockOff, packetsize);
		}
		// craft data packet 
		packet[0] = 3; // flag byte of packet
		memcpy(packet + 1, hash, 32); // copy id
		UintToBytes(bOff, packet + 33); // copy currentoffset
		UintToBytes(packetsize, packet + 37); // copy current packetsize
		PeerSend(0, packet, packetsize + 41);
		// update boff
		bOff += packetsize;
		// free heap alloc
		free(b);

	}

	// free the header in last operations.
	free(header);
	
}

void BroadcastRawFile(const char* filePath, unsigned char _flag) // flag is 1 for block file. 2 for tx file etc.
{
	// chunk cannot exceed 4096
	FILE* f = fopen(filePath, "rb");
	if (f == NULL) { return; } // throw error if cannot read
	fseek(f, 0, SEEK_END);
	uint32_t lSize = ftell(f);
	rewind(f);


	// Ask Permissions
	unsigned char data[41];
	uint32_t ts = GetTimeStamp();
	int rd = 0;
	memcpy(data, &rd, 4);
	memcpy(data + 4, &ts, 4);
	Sha256.init();
	Sha256.write((char*)data, 8);
	unsigned char hash[32];
	memcpy(hash, Sha256.result(), 32);
	memcpy(data + 2, hash, 32);
	data[0] = 2; // flag byte of packet
	data[1] = _flag; // flag byte of data 
	UintToBytes(lSize, data + 34); // tell datasize 
	// we should do some UL Management like DL in server cpp ( with permission flag etc. ) 
	SendToAllPeers( data, 38); 

	// [?] Wait for Permissions


	// Broadcast
	unsigned char packet[ULCHUNKSIZE];
	uint32_t bOff = 0;
	while (bOff < lSize)
	{
		
		// read file to buffer in chunk format
		uint32_t packetsize = ULCHUNKSIZE - 41;
		if (bOff + (ULCHUNKSIZE) > lSize) {
			packetsize = lSize - bOff;
		}
		std::cout << "Sending " << packetsize << "bytes" << "[" << bOff << "/" << lSize << "]" << std::endl;
		fseek(f, bOff, SEEK_SET);
		fread(packet + 41, 1, packetsize, f);

		// craft data packet 
		packet[0] = 3; // flag byte of packet
		memcpy(packet + 1, hash, 32); // copy id
		UintToBytes(bOff, packet + 33); // copy currentoffset
		UintToBytes(packetsize, packet + 37); // copy current packetsize
		SendToAllPeers( packet, packetsize + 41);
		// update boff
		bOff += packetsize;

	}

	fclose(f);
}






