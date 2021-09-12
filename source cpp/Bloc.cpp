#include "Bloc.h"
#include "sha256.h";
#include "tx.h"
unsigned char* BLOCKSPTR;
uint32_t LatestBlockIndex;


void PrintBlockInfo(unsigned char * b )
{
	/*
	Reminder : index (4o) . hash (32o) . phash (32o) . timestamp (4o) . hashtarget (32o) .  nonce (4 o) .  miner token [can be either 4+1 o or 532+1 o] .
	. txn ( 2o ) . txs (variable)
	*/
	std::cout << "-_-_-_-_-_-_-_-_-_block info_-_-_-_-_-_-_-"  << std::endl;
	uint32_t val = GetBlockIndex(b);
	std::cout << "index :" << val << std::endl;
	std::cout << "hash          :";  printHash(GetBlockHash(b));
	std::cout << "previous hash :";  printHash(GetBlockPreviousHash(b));
	val = GetBlockTimeStamp(b);
	std::cout << "ts :" << val << std::endl;
	std::cout << "ht :";
	for (int i = 0; i < 32; i++) { std::cout << (int)*(b + 72 + i) << "-"; }
	std::cout << std::endl;
	val = GetBlockNonce(b);
	std::cout << "nonce :" << val << std::endl;
	val = GetTransactionNumber(b);
	std::cout << "TXN :" << val << std::endl;
	for (int i = 0; i < val; i++) {
		PrintTransaction(GetBlockTransaction(b, i));
	}
	std::cout << "-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-" << std::endl;
}


void LoadBlockPointers()
{
	if ( BLOCKSPTR != NULL)
		free(BLOCKSPTR);

	FILE* f = fopen("blockchain\\blocksptr", "rb");
	if (f == NULL) { std::cout << "error reading..." << std::endl; return; } // throw error if cannot read
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	rewind(f);
	BLOCKSPTR = (unsigned char*)malloc(fsize);
	fread(BLOCKSPTR, 1, fsize, f);
	fclose(f);
}

uint32_t GetLatestBlockIndex(bool official) // only Official (need to be perform at every update and at start ) 
{
	unsigned char uintbuff[4];
	uint32_t result = 0;
	uint32_t cu = 0;

	if ( !official)
	{
		for (const auto & entry : std::experimental::filesystem::directory_iterator("fork"))
		{
			ReadFile(entry.path().string().c_str(), 4, 4, uintbuff);
			cu = BytesToUint(uintbuff);
			if (cu > result)
			{
				result = cu;
			}
		}
	
	}
	if (result == 0) // or official ;)
	{
		FILE* f = fopen("blockchain\\blocksptr", "rb");
		if (f == NULL) { std::cout << "error reading..." << std::endl; return 0; } // throw error if cannot read
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fclose(f);
		return (fsize / 8) - 1;
	}
	else
	{
		free(uintbuff);
		return result;
	}

}

void GetLatestBlockFilePath(char * buffer)// utility for miner  // ????????????????
{
	unsigned char uintbuff[4];
	uint32_t result = 0;
	uint32_t cu = 0;
	for (const auto & entry : std::experimental::filesystem::directory_iterator("fork"))
	{
		ReadFile(entry.path().string().c_str(), 4, 4, uintbuff);
		cu = BytesToUint(uintbuff);
		if (cu > result)
		{
			result = cu;
			strcpy(buffer, entry.path().string().c_str());
			std::cout << entry.path().string().c_str() << std::endl;
		}
	}
	if (result == 0)
	{
		// return latest official path
		// result is in BLOCKSPTR 
		FILE* f = fopen("blockchain\\blocksptr", "rb");
		if (f == NULL) { std::cout << "error reading..." << std::endl; return ; } // throw error if cannot read
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		rewind(f);
		fseek(f, fsize-8, SEEK_SET);
		fread(uintbuff, 1, 4, f); // getting last num
		// concat name 
		std::ostringstream s;
		s << "blockchain\\" << BytesToUint(uintbuff);
		std::string ss = s.str();
		std::cout << "returning : " << ss.c_str() << std::endl;
		strcpy(buffer, ss.c_str());

	
	}
}



unsigned char* GetLatestBlock(bool official) 
{
	unsigned char uintbuff[4];
	uint32_t result = 0;
	uint32_t cu = 0;
	const char * r = NULL;
	unsigned char * block = NULL;

	if ( ! official )
	{
		for (const auto & entry : std::experimental::filesystem::directory_iterator("fork"))
		{
			ReadFile(entry.path().string().c_str(), 4, 4, uintbuff);
			cu = BytesToUint(uintbuff);
			if (cu > result)
			{
				result = cu;
				r = entry.path().string().c_str();
			}
		}
	}
	if (result == 0)
		block = GetOfficialBlock(GetLatestBlockIndex(true));
	else
		block = GetUnofficialBlock(r, result);

	return block;
}
	

unsigned char* GetOfficialBlock(uint32_t index) // return a char*
{
	// NEED TO HANDLE ERROR IF BLOCK IS NOT FOUND !

	unsigned char uintbuff[4];
	memcpy(uintbuff, BLOCKSPTR + (index * 8), 4);
	uint32_t filenum = BytesToUint(uintbuff);
	memcpy(uintbuff, BLOCKSPTR + (index * 8) + 4, 4);
	uint32_t bOff = BytesToUint(uintbuff);

	// concat name 
	std::ostringstream s;
	s << "blockchain\\" << filenum;
	std::string ss = s.str();
	const char * filePath = ss.c_str();

	unsigned char * block = GetBlock(filePath, bOff);

	return block;
}

unsigned char * GetUnofficialBlock(const char *filePath, uint32_t index)
{
	// unofficial block file have two uint prefix & includes blocksptr in the file itself :
	// firstindex . lastindex.  blocksptr (4o) . data

	unsigned char uintbuff[8];
	ReadFile(filePath, 0, 8, uintbuff);
	uint32_t fIndex = BytesToUint(uintbuff);
	uint32_t lIndex = BytesToUint(uintbuff+4);
	if (index > lIndex || index < fIndex)
	{
		std::cout << "[Wrong index] last index in file : " << lIndex << " | index asked : " <<  index << std::endl;
		return NULL;
	}
	int ptr = 8 + ((index - fIndex) * 4);
	ReadFile(filePath, ptr, 4, uintbuff);
	uint32_t bOff = BytesToUint(uintbuff);
	std::cout << "(GetUnofficialBlock) boff is  : " << bOff << " found at " <<ptr <<  std::endl;
	unsigned char * block = GetBlock(filePath, bOff);

	return block;
}

unsigned char * GetBlock(const char *filePath, uint32_t bOff)
{
	std::cout << "start reading at " << bOff << " in " << filePath << std::endl;
	/*
	Reminder : index (4o) . hash (32o) . phash (32o) . timestamp (4o) . hashtarget (32o) .  nonce (4 o) .  miner token [can be either 4+1 o or 64+1 o] .
	. txn ( 2o ) . txs (variable)

	*/
	unsigned char uintbuff[4];
	ReadFile(filePath, bOff, 4, uintbuff);
	std::cout << "INDEX " << BytesToUint(uintbuff) << std::endl;

	uint32_t eOff = bOff;
	// read 1b+108 to get minertoken flag
	eOff += 108;
	ReadFile(filePath, eOff, 1, uintbuff);
	if (uintbuff[0] == 1)
	{
		std::cout << "Dust needed" << std::endl;
		eOff += 65;
	}
	else
	{
		std::cout << "No dust ..." << std::endl;
		eOff += 5;
	}

	// read txn 
	ReadFile(filePath, eOff, 2, uintbuff);
	uint32_t txn = BytesToShort(uintbuff);
	std::cout << "TXN is " << txn << std::endl;
	eOff += 2; // adding ushort tx numbre
	while (txn > 0)
	{
		eOff += 81; // jump to datasize 
		ReadFile(filePath, eOff, 4, uintbuff); // cause error
		eOff += BytesToUint(uintbuff) + 4;
		txn--;
	}
	// so now i know exactly the number of bytes 
	/*
	[TX header] (81 bytes )
	PuKey Pointer   (4 bytes)
	PrKey Signature (64 bytes)
	TOU             (4 bytes)
	Purishment   	(4 byte )
	Fee             (4 byte ) (corresponding to all cost + additional reward for miner 
	Byte ID         (1 byte )
	[TX data]
	Data Size       (4 bytes)
	Data            (Data Size)
	*/
	eOff -= bOff;
	unsigned char * block = (unsigned char*)malloc(eOff);
	std::cout << "allocate " << eOff << " bytes for new block in heap " << std::endl;
	ReadFile(filePath, bOff, eOff, block);

	std::cout << "block read success read at " << bOff << " (size " << eOff << " bytes)" << std::endl;
	return block;

}

/*		
		NEW BLOCK STRUCTURE :   -----> (can save 32o per block ... )  needed ???
		index (4o) . hash (32o) . phash (32o) . timestamp (4o) . nonce (4 o). txn. (2o) . flag (1o) 
		miner token (either 5 or 65 ) . txs ( variable ) 

		Reminder : index (4o) . hash (32o) . phash (32o) . timestamp (4o) . hashtarget (32o) .nonce (4 o).    .  miner token [can be either 4+1 o or 532+1 o] .
		. txn ( 2o ) . txs (variable)
	*/

uint32_t GetBlockIndex(unsigned char * block)
{
	return BytesToUint(block);
}
unsigned char * GetBlockHash(unsigned char * block) // return ptr
{
	return block + 4;
}
unsigned char * GetBlockPreviousHash(unsigned char * block) // return ptr
{
	return block + 36;
}
uint32_t GetBlockTimeStamp(unsigned char * block)
{
	return BytesToUint(block + 68);
}
unsigned char * GetBlockHashTarget( unsigned char * block) // return ptr
{
	return block + 72;
}
uint32_t GetBlockNonce(unsigned char * block)
{
	return BytesToUint(block + 104);
}
unsigned char GetMinerTokenFlag(unsigned char * block)
{
	return *(block+108);
}


uint16_t GetTransactionNumber(unsigned char * block)
{

	int off;
	if (GetMinerTokenFlag(block) == 0)
		off = 113;
	else
		off = 173;

	std::cout << "txn read at " << off << std::endl; // generate an error here 
	std::cout << "txn is " << BytesToShort(block + off) << std::endl;
 
	return BytesToShort(block + off);
}

unsigned char * GetBlockTransaction(unsigned char * block, uint32_t index) // return ptr in b data
{
	if (index > GetTransactionNumber(block))
		return NULL;


	int off;
	// probleme here so add two bytes ... because txn is not used
	if (GetMinerTokenFlag(block) == 0)
		off = 115; // 113
	else
		off = 175; // 173// the starting offset if pub key is delivered in minertoken 

	while ( index > 0 )
	{
		off += 81; // jump through tx data size 
		off += BytesToUint(block+ off) + 4;  // jump tx data size + [4 ( the data of datasize information )]
		index--;
	}

	return block + off;
	
} 
/*
[TX header]
PuKey Pointer   (4 bytes)
PrKey Signature (64 bytes)
TOU             (4 bytes)
Purishment   	(4 byte )
Fee             (4 byte ) (corresponding to all cost + additional reward for miner
Byte ID         (1 byte )
[TX data]
Data Size       (4 bytes)
Data            (Data Size)

Signature is the whole data except signature : PUKEYPTR TOOU PURSIGHMENT FEE BYTE ID DATASIZE DATA ETC
*/
void GetTXsPuKey(unsigned char * buffer, unsigned char * TX) // need a buffer[64]
{
	uint32_t utxop = BytesToUint(TX);
	unsigned char utxo[72];
	memset(utxo, 0, 72);
	GetUtxo(utxop, utxo);
	
	if (isUtxoNull(utxo)) {
		buffer == NULL;
		free(utxo);
		return;
	}
	memcpy(buffer, GetUtxoPuKey(utxo), 64); // i dont remember if i  use hash of pukey to do the signature ?

}

unsigned char * GetTXSignature(unsigned char * TX) // return ptr
{
	return TX + 4;
}
uint32_t GetTXTokenOfUniqueness(unsigned char * TX)
{
	return BytesToUint(TX + 68);
}
uint32_t GetTXPurishmentDate(unsigned char * TX)
{
	return BytesToUint(TX + 72);
}
uint32_t GetTXFee(unsigned char * TX)
{
	return BytesToUint(TX + 76);
}
unsigned char GetTXByteID(unsigned char * TX)
{
	return TX[80];
}
uint32_t GetTXDataSize(unsigned char * TX)
{
	return BytesToUint(TX + 81);
}
unsigned char * GetTXData(unsigned char * TX) // return ptr
{
	return TX + 85;
}


void UpdateOfficialChain(const char * filePath )
{
	
	// get last blockchain file size 
	FILE* lastblockchainpath = fopen("blockchain\\0", "rb");
	if (lastblockchainpath == NULL) { std::cout << "error reading..." << std::endl; return; } // throw error if cannot read
	fseek(lastblockchainpath, 0, SEEK_END);
	uint32_t bptrsize = ftell(lastblockchainpath);
	fclose(lastblockchainpath);
	
	// UPDATE BLOCKSPTRS FILE ( from Header file info ) ....
	FILE* f = fopen(filePath, "rb");
	if (f == NULL) {
		return;
	}
	unsigned char buffer[8];
	fseek(f, 0, SEEK_SET);
	fread(buffer, 1, 8, f);

	uint32_t fi = BytesToUint(buffer);
	uint32_t li = BytesToUint(buffer + 4);
	uint32_t blocknum = (li - fi) + 1;
	uint32_t headersize = 8 + (blocknum * 4);

	// append to first block ptr 
	memset(buffer, 0, 4);
	UintToBytes(bptrsize, buffer+4);
	AppendFile("blockchain\\blocksptr", buffer, 8 ); //0 and 8 
	std::cout << " ADDING PTR " << bptrsize << std::endl;
	// adding  8 bytes 
	fread(buffer, 1, 4, f);
	uint32_t lastbptr = headersize ; // <<< header size ... ???.....

	for (int a = 1; a < blocknum; a++) // NOT WORKIN
	{
		fread(buffer, 1, 4, f);
		uint32_t newbptr = BytesToUint(buffer);
		uint32_t lastblocksize = newbptr - lastbptr;
		lastbptr = newbptr;
		bptrsize += lastblocksize;
		std::cout << "BLOCKSIZE is " << lastblocksize << " ADDING PTR " << bptrsize << std::endl;
		
		// append last block to blocptr
		memset(buffer, 0, 4);
		UintToBytes(bptrsize, buffer + 4);
		AppendFile("blockchain\\blocksptr", buffer, 8); //0 and 8 
	}
	// so now we should be
	// ADDING BLOCK DATA of SPECIFIC FILE ( need Header length so number of blocks )  
	fseek(f, 0, SEEK_END);
	uint32_t fsize = ftell(f);
	fseek(f, headersize, SEEK_SET);
	unsigned char * bdata = (unsigned char *)malloc(fsize - headersize);
	fread(bdata, 1, fsize - headersize, f);
	AppendFile("blockchain\\0", bdata, fsize - headersize);
	free(bdata);

	std::cout << "Blockchain files successfully update" << std::endl;
	fclose(f);


}

