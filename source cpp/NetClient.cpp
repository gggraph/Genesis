#include "NetClient.h"
#include "Utils.h"
#include<string.h> 
#include "sha256.h"
#include "consensus.h"

#define MAXPEERS 16
#define ULCHUNKSIZE 4096

TCPClient* peers[MAXPEERS]; // Array of TCPClient Pointers

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
	return; 
	// TODO
	// need to prepare custom header block files 
	for (int i = firstindex; i < firstindex + count; i++) 
	{
		GetOfficialBlock(firstindex);
		
	}
	


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
		uint32_t packetsize = ULCHUNKSIZE;
		if (bOff + ULCHUNKSIZE + 41 > lSize) {
			packetsize = lSize - bOff - 41;
		}
		fseek(f, bOff, SEEK_SET);
		fread(packet + 41, 1, packetsize, f);

		// craft data packet 
		data[0] = 3; // flag byte of packet
		memcpy(data + 1, hash, 32); // copy id
		UintToBytes(bOff, data + 33); // copy currentoffset
		UintToBytes(packetsize, data + 37); // copy current packetsize
		PeerSend(0, data, packetsize + 41);
		// update boff
		bOff += packetsize;
	}

	fclose(f);
}






