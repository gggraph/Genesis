#include "Mine.h"
#include "arith256.h"

void Mine(unsigned char * puKey, uint32_t max_purishing, uint32_t utxop, char * fpathbuffer) // return the name of the mined block path
{


	char bfpath[255];
	GetLatestBlockFilePath(bfpath);
	std::cout << " file path : " << bfpath << std::endl;
	unsigned char * lastblock = NULL;
	bool _continuefork = false;
	uint32_t firstfblockindex, lastfblockindex;

	if (strstr(bfpath, "blockchain") != NULL)
		lastblock = GetLatestBlock(true); 
	else
	{
		std::cout << " will get new block " << bfpath << std::endl;
		unsigned char ubuff[8];
		ReadFile(bfpath, 0, 8, ubuff);
		firstfblockindex = BytesToUint(ubuff);
		lastfblockindex = BytesToUint(ubuff+4);
		_continuefork = true;
		lastblock = GetUnofficialBlock(bfpath, lastfblockindex);
		
	}

	if (lastblock == NULL)
		return ;

	unsigned char * txs = GetTransactionsForNewBlock();
	uint16_t txn = 0; 
	uint32_t datasize = 0;
	uint32_t txsize = 0;
	if (txs != NULL)
	{
		txn = BytesToUint(txs);
		datasize = BytesToUint(txs+2);
		txsize = datasize;
	}
	if (!utxop)
		datasize += 175; // i dk really where is those 2 bytes flying 
	else
		datasize += 115;  

	// start creating the block header 
	unsigned char * nblock = (unsigned char *)malloc(datasize);
	unsigned char * ublock = (unsigned char *)malloc(datasize-36); // unp block ( without hash & nonce )
	unsigned char buff[32];

	/*
	Reminder : index (4o) . hash (32o) . phash (32o) . timestamp (4o) . hashtarget (32o) .  nonce (4 o) .  miner token [can be either 4+1 o or 64+1 o] .
	. txn ( 2o ) . txs (variable)
	*/

	// prepare block header to hash

	uint32_t nIndex = GetBlockIndex(lastblock) + 1;
	UintToBytes(nIndex, buff);
	memcpy(ublock, buff, 4); // index
	memcpy(ublock+4, GetBlockHash(lastblock), 32); // phash
	UintToBytes(GetTimeStamp(), buff);
	memcpy(ublock+36, buff, 4); // ts

	if (nIndex % TARGET_CLOCK && nIndex >= TARGET_CLOCK)
	{

		ComputeHashTarget(BytesToUint(buff), GetOfficialBlock(nIndex - TARGET_CLOCK), buff);
		memcpy(ublock + 40, buff, 32);
	}
	else
	{
		

		memcpy(ublock+40, GetBlockHashTarget(lastblock), 32); 
	
	}

	int boff;
	if ( utxop != 0 )
	{
		memset(ublock + 72,0, 1);
		memcpy(ublock + 73, &utxop, 4);
		boff = 77;
	}
	else
	{
		memset(ublock + 72, 1, 1);
		memcpy(ublock + 73, puKey, 64); // puKey is 64b
		boff = 137;
	}

	memcpy(ublock + boff, &txn, 2); // txn
	if (txs != NULL)
	{
		memcpy(ublock + boff + 2, txs, txsize);
		free(txs);
	}


	//SHA256
	Sha256.init();
	Sha256.write((char *)ublock, datasize - 36);
	memcpy(buff, Sha256.result(), 32);

	

	// _________________ MINING _________________
	
	uint32_t golden_nonce = 0;
	memcpy(nblock + 4, buff, 32); // copy head
	std::cout << "start mining" << std::endl;
	std::cout << "target required : " << std::endl;
	for (int i = 0; i < 32; i++) {
		std::cout << (int)*(ublock + 40 + i) << "-";
	}
	std::cout << std::endl;
	srand(time(NULL));
	while (1)
	{
		// we can define here a strategy. increment nonce is ok right now .... but we can use random bytes also ...
		golden_nonce++;//golden_nonce = rand() % UINT_MAX;
		UintToBytes(golden_nonce, nblock); // copy nonce to nblock 
		Sha256.init();
		Sha256.write((char *)nblock, 36); // hash first 36 bytes ( nonce + header ) 
		// for double hash func,  we can add here : Sha256.init(); Sha256.write(Sha256.result(), 32);
		if (cmp_256(Sha256.result(), ublock + 40) <= 0) {
			break;
		}
	}
	
	// _________________ PROCCESS FILE _________________
	memcpy(nblock, ublock, 4);
	memcpy(nblock + 4, buff, 32);
	memcpy(nblock + 36, ublock + 4, 68);
	memcpy(nblock + 104, &golden_nonce, 4); // put nonce at 104 ptr
	memcpy(nblock + 108, ublock + 72, datasize - 108); 


	char hashpath[255];
	GetHashString(buff, hashpath);
	std::ostringstream s;
	s << "tmp\\" << hashpath;
	std::string ss = s.str();
	const char * bname = ss.c_str();
	std::cout << bname << std::endl;
	// Append Header data for unofficial blocks file
	

	if ( _continuefork ) 
	{
		// [0] Get old file size and copy memory
		FILE* f = fopen(bfpath, "rb");
		if (f == NULL) { return; } // throw error if cannot read
		fseek(f, 0, SEEK_END);
		uint32_t lSize = ftell(f);
		rewind(f);
		unsigned char * fbuffer = (unsigned char*)malloc(lSize);
		fread(fbuffer, 1, lSize, f);
		fclose(f);

		// [1] Create Winner Block
		f = fopen(bname, "ab");
		if (f == NULL) { return; } // throw error if cannot read

		// [2] Update header in fbuffer
		UintToBytes(nIndex, fbuffer + 4); // update last index  ( at 4 o ) 
		uint32_t header_size = 8 + ((lastfblockindex - firstfblockindex) + 1) * 4;
		IncrementUnofficialBlockFileHeader((lastfblockindex - firstfblockindex) + 1, fbuffer); // update header ptrs 

		//[3] Append old refreshed header
		fseek(f, 0, SEEK_SET);
		fwrite(fbuffer, 1, header_size, f);

		//[4] Append new block pointer to header
		UintToBytes(lSize+4, buff); // acting weird if buff is more than 2550
		fwrite(buff, 1, 4, f);

		//[5] Append old blocks data
		fwrite(fbuffer + header_size, 1, lSize - header_size, f); 
		free(fbuffer);

		//[6] Append new block data
		fwrite(nblock, 1, datasize, f);

		//[7] Verify Size 
		int nfsize = ftell(f);
		int sizeneeded = (header_size + 4) + lSize - header_size + datasize;
		if ( nfsize != sizeneeded )
		{
			std::cout << "BAD FILE CREATED CORRUPTED : " << nfsize << "/" << sizeneeded << std::endl;
			fclose(f);
			while (1) {}
		}
		
		fclose(f);

	}
	else
	{
		std::cout << "new file " << std::endl;
		UintToBytes(nIndex, buff);
		UintToBytes(nIndex, buff + 4);
		UintToBytes(12, buff + 8);
		AppendFile(bname, buff, 12);
		AppendFile(bname, nblock, datasize);

	}
	

	free(lastblock);
	free(nblock);
	free(ublock);
	std::cout << "[CONGRATS] New block mined at " << bname << std::endl;
	strcpy(fpathbuffer, bname);
	
}

void IncrementUnofficialBlockFileHeader(uint32_t oBnum,  unsigned char * fdata )
{
	int boff = 8; // first 8 bytes are 
	for (int i = 0; i < oBnum; i++)
	{
		uint32_t nptr = BytesToUint(fdata+boff) + 4 ;
		std::cout << "header update: " << (nptr - 4) << " to " << nptr << std::endl;
		UintToBytes(nptr, fdata + boff);
		boff += 4;
	}

}



unsigned char * GetTransactionsForNewBlock() // need to be free cause malloc
{
	//just searching ptx with best size / fee ratio.will return a memblock whith 6 bytes header indicates txn and whole memblock size
	return NULL;
}