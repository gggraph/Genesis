
#include "Genesis.h"
#include "sha256.h"
#include "Mine.h"
#include <windows.h>
#include "vm.h"
#include "arith256.h"
#include "tx.h"

/*
TODO 
	* THINK OF PTX FILE DATA STRUCTURE 
	* CREATE PTX ADDING FUNCTION TO PTX FILE
	* CREATE PTXFILLING DURING MINING PROCCESS. 
	* JUST TEST IT
	* 
	* APPLY NETWORKING
	* APPLY SOME SMARTCONTRACT STUFF TO TEST IT
	* APPLY COMMAND LINE
	* DONE. REVIEW COMMENT AND CODE
*/

int main(int argv, char** args)
{
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

	// proccess blocks file
	ProccessBlocksFile(wblockpath);
	// Start Mining 7 times to get a Offical Blockchain update.
	for (int i = 0; i < 7; i++) {
		Mine(pukey, 5000, 0, wblockpath);
		ProccessBlocksFile(wblockpath);
	}
	// Get My Utxo Pointer
	uint32_t utxop = GetUtxoPointer(pukey);
	std::cout << " Pointer found at : " << utxop << std::endl;
	unsigned char utxobuff[72];
	GetUtxo(utxop, utxobuff);
	std::cout << " sold : " << GetUtxoSold(utxobuff) << std::endl;
	std::cout << "latest index " << GetLatestBlockIndex(true) << std::endl;

	
	// create a transaction
	unsigned char prkey[32]; //  
	ReadFile("prk", 0, 32, prkey); // fill pukey buffer gmme erroe here
	CreateDefaultTransaction(prkey, utxop, GetUtxoTOU(utxobuff) + 1, 5000, 0, 10, utxop, NULL);
	while (1) {}



	// Mine 60 times with my pointer to get my reward
	for (int i = 0; i < 60; i++) {
		Mine(pukey, 5000, utxop, wblockpath);
		ProccessBlocksFile(wblockpath);
	}

	// print my super new updated sold ...
	GetUtxo(utxop, utxobuff);
	std::cout << " sold : " << GetUtxoSold(utxobuff) << std::endl;
	std::cout << "latest index " << GetLatestBlockIndex(true) << std::endl;

	

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
	std::cout << "genesis created.";

}






