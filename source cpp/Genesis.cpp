
#include "Genesis.h"
#include "sha256.h"
#include "Mine.h"
#include <windows.h>


int main(int argv, char** args)
{

	InitChain();
	VerifyFiles();
	LoadBlockPointers();
	
	// Mine stuff
	unsigned char pukey[532]; 
	int i = 1;
	while ( i < 7 )
	{
		char wblockpath[255];
		Mine(pukey, 5000, 0, wblockpath);

		/* print block info ... */
		unsigned char buffer[255]; 
		ReadFile(wblockpath, 0, 8, buffer);
		uint32_t fi = BytesToUint(buffer);
		uint32_t li = BytesToUint(buffer+4);
		uint32_t blocknum = (li - fi) + 1;
		std::cout << "____________________TMP INFO________________________" << std::endl;
		std::cout << "Number of blocks : " << blocknum << " | first block index " << fi << " | last block index " << li << std::endl;
		for (int a = 0 ; a < blocknum; a++ )
		{
			int ptr = 8 + (a * 4);
			ReadFile(wblockpath, ptr, 4, buffer);
			std::cout << "ptr found at block #" << fi + a << " : " << BytesToUint(buffer) << "(reading at " << ptr << ")" << std::endl;
		}
		std::cout << "____________________________________________________" << std::endl;
		
		unsigned char * b = GetUnofficialBlock(wblockpath, i); // it is deleted ....
		if (b != NULL )
		{
			PrintBlockInfo(b);
			free(b);
			
		}
		else
		{
			std::cout << "BLOCK NULL ... ";
			//while (1) {}
		}
		ProccessBlocksFile(wblockpath);
		i++;
	}
	// load pukey at ptr 0 

	// ALL SEEMS OK BUT I DONT UNDERSTANT
	uint32_t utxop =  GetUtxoPointer(pukey);
	std::cout << " Pointer found at : " << utxop << std::endl; // not working :) 
	unsigned char utxobuff[540];
	GetUtxo(utxop, utxobuff);
	std::cout << " sold : " << GetUtxoSold(utxobuff) << std::endl;
	std::cout << "latest index " <<GetLatestBlockIndex(true) << std::endl;
	unsigned char * b = GetOfficialBlock(6); // it is deleted ....
	if (b != NULL)
	{
		PrintBlockInfo(b);
		free(b);

	}
	else
	{
		std::cout << "BLOCK NULL ... ";
		//while (1) {}
	}

	while ( i < 20 ) // issue
	{
		char wblockpath[255];
		Mine(pukey, 5000, utxop, wblockpath);
		unsigned char * b = GetUnofficialBlock(wblockpath, i); // it is deleted ....
		if (b != NULL)
		{
			PrintBlockInfo(b);
			free(b);

		}
		else
		{
			std::cout << "BLOCK NULL ... ";
			while (1) {}
		}
		ProccessBlocksFile(wblockpath);
		i++;
	}
	memset(utxobuff, 0, 540);
	GetUtxo(utxop, utxobuff);
	std::cout << " sold : " << GetUtxoSold(utxobuff) << std::endl;
	std::cout << "latest index " << GetLatestBlockIndex(true) << std::endl;
	while ( 1 ){}
	

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
		Reminder : index (4o) . hash (32o) . phash (32o) . timestamp (4o) . hashtarget (32o) .  nonce (4 o) .  miner token [can be either 4+1 o or 532+1 o] .
		. txn ( 2o ) . txs (variable) 
	*/
	// ULTRA FAST IMP
	unsigned char genese[115];
	memset(genese, 0, 115);
	uint32_t ts = (uint32_t)FIRST_UNIX_TIMESTAMP;
	memcpy(genese + 68, &ts, 4);

	// create the first blockchain file
	AppendFile("blockchain\\0", genese, 115);
	// create the blockptrs file 
	AppendFile("blockchain\\blocksptr", genese, 8); // no need to update it is file 0 offset 0 :) 
	// we re done. free memory
	DeleteDirectory("utxos");
	// create utxos file 
	_wmkdir(L"utxos");
	//create first utxo file 
	unsigned char futxo[540];
	memset(futxo, 0, 540);
	FILE* f = fopen("utxos\\0", "ab");
	if (f == NULL) return;
	rewind(f);
	fwrite(futxo, 1, 540, f);
	fclose(f);
	// + secure
	DeleteDirectory("sc");
	DeleteDirectory("tmp");
	_wmkdir(L"sc");
	_wmkdir(L"tmp");
	std::cout << "genesis created.";

}






