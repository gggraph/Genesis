#include "NetServer.h"
#include "NetClient.h"
#include "Genesis.h"
#include "sha256.h"
#include "Mine.h"
#include <windows.h>
#include "vm.h"
#include "arith256.h"
#include "tx.h"
#include "NetfileManager.h"
#include "UI.h";

/*
TODO 
	* 
	* APPLY COMMAND LINE
	* DONE. REVIEW COMMENT AND CODE
*/

unsigned char GENESIS_TARGET[32] =
{
	0x2F,0x95,0x5C,0x98,0x3D,0x33,0x7E,0xE6,
	0x97,0xFD,0xD,0x65,0xE7,0x37,0xC,0x62,
	0xC0,0xB,0x4,0x45,0x98,0x90,0xC2,0x7D,
	0xAC,0x75,0x65,0xBD,0x93,0x5,0xA,0x0
};
int main(int argv, char** args)
{
	SayHello();
	PrintCommandList();
	//GetCommand();
	std::thread cmdthd(GetCommand);
	//cmdthd.join(); is wait to finish cmd thread
	while (1) {}

}

bool GenesisLoop() 
{
	// verify files sanity
	InitChain();
	VerifyFiles();
	LoadBlockPointers();
	// get pukey for mining process. 

	return false;
}
	/*
	StartServer("net.ini");
	ConnectToPeerList("net.ini");

	std::cout << "Mine one block" << std::endl;
	getchar();
	
	InitChain();
	VerifyFiles();
	LoadBlockPointers();

	// [0b] creating pairkeys if not existing
	MakeSECP256K1PairKeys();

	// [1] print genesis
	unsigned char* gb = GetOfficialBlock(0);
	PrintBlockInfo(gb);
	free(gb);


	// [2] start mining from public Key 
	unsigned char pukey[64]; //  pukey
	ReadFile("puk", 0, 64, pukey); // fill pukey buffer 

	char wblockpath[255];
	Mine(pukey, 5000, 0, wblockpath);

	std::cout << "send file ...";
	BroadcastRawFile(wblockpath, 1);
	std::cout << "process q file ...";
	getchar();
	ProccessNextQFile();
	// send the file to server 
	*/
	/*
	unsigned char ge_tar[] =
	{
		0x2F,0x95,0x5C,0x98,0x3D,0x33,0x7E,0xE6,
		0x97,0xFD,0xD,0x65,0xE7,0x37,0xC,0x62,
		0xC0,0xB,0x4,0x45,0x98,0x90,0xC2,0x7D,
		0xAC,0x75,0x65,0xBD,0x93,0x5,0x0,0x0

	};
	std::cout << "before div " << std::endl;
	for (int i = 0; i < 32; i++) {
		std::cout << (int)ge_tar[i] << std::endl;
	}
	//shiftdiv_256(ge_tar, POW_TARGET_TIME); // working
	mul_256(ge_tar, 2048);
	std::cout << "after div " << std::endl;
	for (int i = 0; i < 32; i++) {
		std::cout << (int)ge_tar[i] << std::endl;
	}
	while ( 1){}
	*/
	/*
	TestVM();
	
	unsigned char A256[32];
	unsigned char B256[32];
	memset(A256,0, 255); memset(B256, 0, 31);
	A256[31] = 1;
	B256[31] = 2;
	int cr = cmp_256(A256, B256);
	mul_256(A256, 200); // it works.
	div_256(A256, 2); // will test. should result 3. 
	char hex[64];
	hx_256(A256, hex);
	std::cout << hex << std::endl;

	while ( true ){}
	*/

	// [0] init software
	//Demo();
	


	/*
	std::cout << "Building NewUploadDemand" << std::endl;
	unsigned char data[241];
	uint32_t ts = GetTimeStamp();
	int rd = 0;
	memcpy(data, &rd, 4);
	memcpy(data+4, &ts, 4);
	Sha256.init();
	Sha256.write((char*)data, 8);
	unsigned char hash[32];
	memcpy(hash, Sha256.result(), 32);
	memcpy(data + 2, hash, 32);
	data[0] = 2; // flag byte of packet
	data[1] = 1; // flag byte of data 
	
	
	printHash(Sha256.result());
	UintToBytes(200, data + 34);

	getchar();
	std::cout << "Sending ... " << std::endl;
	getchar();
	PeerSend(0, data, 38);
	getchar();
	PrintDLinformation(0);
	
	std::cout << "Testing send file  ... " << std::endl;
	getchar();


	data[0] = 3; // flag byte of packet
	memcpy(data + 1, hash, 32);
	UintToBytes(0, data + 33);
	UintToBytes(200, data + 37);
	PeerSend(0, data, 241);
	*/


void Demo() 
{
	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [STEP 01] Initialize blockchain files" << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();
	InitChain();
	VerifyFiles();
	LoadBlockPointers();
	
	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [STEP 02] Create new pair keys for ECDSA" << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();
	// [0b] creating pairkeys if not existing
	MakeSECP256K1PairKeys();

	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [INFO	  ] Here is the genesis block" << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();
	// [1] print genesis
	unsigned char* gb = GetOfficialBlock(0);
	PrintBlockInfo(gb);
	free(gb);

	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [STEP 03] Mine seven blocks." << std::endl;
	std::cout << "         Wait for blocks validations and upgrade of the official chain." << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();
	// [2] start mining from public Key 
	unsigned char pukey[64]; //  pukey
	ReadFile("puk", 0, 64, pukey); // fill pukey buffer 

	char wblockpath[255];
	Mine(pukey, 5000, 0, wblockpath);

	// proccess blocks file
	ProccessBlocksFile(wblockpath);
	// Start Mining 7 times to get a Offical Blockchain update.
	for (int i = 0; i < 7; i++) {
		Mine(pukey, 5000, 0, wblockpath);
		ProccessBlocksFile(wblockpath);
	}

	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [STEP 04] Get Wallet identifier and print his data. " << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();

	// Get My Utxo Pointer
	uint32_t utxop = GetUtxoPointer(pukey);
	std::cout << " Your wallet identifier : " << utxop << std::endl;
	unsigned char utxobuff[72];
	GetUtxo(utxop, utxobuff);
	PrintUTXO(utxobuff);

	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [STEP 05] Mine 10 more blocks. See your new sold " << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();

	// Mine 10 times with my pointer to get my reward
	for (int i = 0; i < 10; i++) {
		Mine(pukey, 5000, utxop, wblockpath);
		ProccessBlocksFile(wblockpath);
	}
	getchar();
	GetUtxo(utxop, utxobuff);
	PrintUTXO(utxobuff);
	getchar();

	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [STEP 06] Donate 8 coins to random key. " << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();

	// create a transaction
	unsigned char prkey[32]; //  
	ReadFile("prk", 0, 32, prkey); // fill pukey buffer gmme erroe here
	unsigned char rdpukey[64];
	GetRandomValidPublicKey(rdpukey);
	CreateDefaultTransaction(prkey, utxop, GetUtxoTOU(utxobuff) + 1, 5000, 50, 8, 0, rdpukey);
	//CreateDefaultTransaction(prkey, utxop, GetUtxoTOU(utxobuff) + 1, 5000, 0, 1, utxop, NULL);

	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [STEP 07] Mine the block containing the transaction. " << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();
	Mine(pukey, 5000, utxop, wblockpath);
	ProccessBlocksFile(wblockpath);

	// recreating ptx
	remove("ptx");
	FILE* f = fopen("ptx", "wb");
	if (f == NULL) return ;
	fclose(f);
	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [STEP 08] Mine more blocks to win the rule distance " << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();

	// continue mining block
	for (int i = 0; i < 10; i++) {
		Mine(pukey, 5000, utxop, wblockpath);
		ProccessBlocksFile(wblockpath);
	}
	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [STEP 09] Here is your new sold " << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();
	GetUtxo(utxop, utxobuff);
	PrintUTXO(utxobuff);

	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [STEP 10] Travel in the past" << std::endl;
	std::cout << "        See your sold 10 blocks earlier. " << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();
	uint32_t lastbi = GetLatestBlockIndex(true);
	DowngradeUtxoAtSpecificBlockTime(utxobuff, lastbi - 10);
	PrintUTXO(utxobuff);
	std::cout << "___________________________________________________________" << std::endl;
	std::cout << std::endl;
	std::cout << "        [DEMO IS FINISHED] " << std::endl;
	std::cout << "___________________________________________________________" << std::endl;
	getchar();
}



bool VerifyFiles()
{
	if ( !FileExists("blockchain"))
	{
		_wmkdir(L"blockchain");
		//TestBlock(); // should be genesis but lets go
		CreateGenesisBlock();
	}
	if (!FileExists("utxos"))
	{
		// rebuild UTXOS From Blockchain
	}
	if (!FileExists("sc"))
	{
		_wmkdir(L"sc");
		_wmkdir(L"sc\\tmp");
		// rebuild SC From Blockchain
	}
	if (!FileExists("tmp"))
	{
		_wmkdir(L"tmp");
	}
	if (!FileExists("fork"))
	{
		_wmkdir(L"fork");
	}
	if (!FileExists("ptx"))
	{
		FILE* f = fopen("ptx", "wb");
		if (f == NULL) return false;
		fclose(f);
	}

	return true;
}

void InitChain () // delete all files. 
{
	DeleteDirectory("blockchain");
	DeleteDirectory("utxos");
	DeleteDirectory("sc");
	DeleteDirectory("tmp");
	DeleteDirectory("fork");
	remove("ptx");
	std::cout << "All files have been deleted." << std::endl;
}
void CreateGenesisBlock() 
{
	/*
		Reminder : index (4o) . hash (32o) . phash (32o) . timestamp (4o) . hashtarget (32o) .  nonce (4 o) .  miner token [can be either 4+1 o or 64+1 o] .
		. txn ( 2o ) . txs (variable) 
	*/
	// ULTRA FAST IMP
	unsigned char genese[115];
	memset(genese, 0, 115);
	uint32_t ts = (uint32_t)FIRST_UNIX_TIMESTAMP;
	memcpy(genese + 68, &ts, 4);
	memcpy(genese + 72, GENESIS_TARGET, 32);

	// create the first blockchain file
	AppendFile("blockchain\\0", genese, 115);
	// create the blockptrs file 
	AppendFile("blockchain\\blocksptr", genese, 8); // no need to update it is file 0 offset 0 :) 
	// we re done. free memory
	DeleteDirectory("utxos");
	// create utxos file 
	_wmkdir(L"utxos");
	//create first utxo file 
	unsigned char futxo[72];
	memset(futxo, 0, 72);
	FILE* f = fopen("utxos\\0", "ab");
	if (f == NULL) return;
	rewind(f);
	fwrite(futxo, 1, 72, f);
	fclose(f);
	// + secure
	DeleteDirectory("sc");
	DeleteDirectory("tmp");
	_wmkdir(L"sc");
	_wmkdir(L"sc\\tmp");
	_wmkdir(L"tmp");
	std::cout << "Genesis block created." << std::endl;

}






