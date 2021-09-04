#include "consensus.h"
#include "sha256.h"
#include "arith256.h"
#include <chrono>

void ProccessBlocksFile(const char * filePath)
{
	// create virtual utxo set 
	FILE* f = fopen("utxos\\tmp", "ab");
	if (f == NULL)
	{
		std::cout << "[Block Refused] Cannot create tmp file";  return;
	}
	fclose(f);


	uint32_t lastOfficialindex = GetLatestBlockIndex(true);
	// firstindex . lastindex.  blocksptr (4o) . data
	unsigned char buff[8];
	ReadFile(filePath, 0, 8, buff);
	uint32_t firstfblockindex = BytesToUint(buff);
	uint32_t lastfblockindex = BytesToUint(buff + 4);
	std::cout << "first block index " << firstfblockindex << std::endl;
	std::cout << "last block index " << lastfblockindex << std::endl;
	if (!VerifyBlocksFile(filePath, lastOfficialindex, firstfblockindex, lastfblockindex)) 
	{
		// clear temp file
		remove("utxos\\tmp");
		remove(filePath);
	}
	else
	{
		std::cout << "comparing " << lastfblockindex << " and " << lastOfficialindex + RUN_DISTANCE << std::endl;
		if ( lastfblockindex >= lastOfficialindex + RUN_DISTANCE )
		{
			//update the chain...
			// IT IS A WIN !
			std::cout << "_____________[BLOCKCHAIN WILL BE UPDATED]____________";
			
			UpdateBlockchain(filePath);
		}
		else
		{
			//put in in fork
			size_t oldpsize = strlen(filePath); // path name is tmp\\ need to move it to fork ...
			std::ostringstream s;
			s << "fork";
			s << std::string(filePath, 3, oldpsize);
			std::string ss = s.str();
			std::cout <<"renaming : " << ss << std::endl;
			int result = rename(filePath, ss.c_str());
			if (result == 0)
				puts("Blocks file moved");
			else
				perror("Error renaming file");

			remove("utxos\\tmp");
			return;

		}
	}
}

void UpdateBlockchain(const char * filePath)
{
	// [0] Update utxo set with temp utxo // seems not working 
	UpdateUtxoSet();
	// [0a] [MINER] Delete PTX based on pukey and TOU based on every TX. WARNING THIS WILL NOT CHECK amount and other stuff
	RefreshPTXFileFromVirtualUtxoSet(); //< gives error reading seems ok ( 0 bytes ) 
	// [1] Delete temp utxo
	remove("utxos\\tmp");
	// [2] Procceed SmartContract Request (x) SmartContract Submission
	//...
	// [3] Append Block Data to official chain 
	UpdateOfficialChain(filePath);
	// [4] Delete Temp Block File
	remove(filePath);
	// [5] Empty Fork Directory
	DeleteDirectory("fork");
	_wmkdir(L"fork");
	// [6] Refresh PTRS file and Blocks Cache
	LoadBlockPointers();
	// CONGRATS
	std::cout << "congrats! " << std::endl;

}

bool VerifyBlocksFile(const char * filePath, uint32_t lastOfficialindex, uint32_t firstfblockindex, uint32_t lastfblockindex) 
// need to work with 4gb++ size file
{
	std::cout << "Start validating " << filePath;
;
	unsigned char buff[8];
	if (lastfblockindex <= lastOfficialindex ) // except if files is split (4gb max ... need to handle this next time ) 
	{
		std::cout << "[Block Refused] Lower index than official" << std::endl;
		return false;
	}
	if (firstfblockindex-1 > lastOfficialindex)
	{
		std::cout << "[Block Refused] Can't proccess block. Index too high." << std::endl;
		return false;
	}

	if (firstfblockindex == 0 )
	{
		std::cout << "[Block Refused] Genesis not allowed" << std::endl;
		return false;
	}
	unsigned char * cBlock;
	unsigned char * lBlock;
	// verify first block 
	for (int i = firstfblockindex; i <= lastfblockindex; i++ )
	{
		std::cout << "Validating block # " << i << std::endl;;
		// load block in memory (2mb max ) 
		if ( i == firstfblockindex)
		{
			std::cout << "reading official block # " << (i-1) << std::endl;;
			lBlock = GetOfficialBlock(i - 1);
		}
		else
		{
			std::cout << "reading unofficial block # " << (i - 1) << std::endl;;
			lBlock = GetUnofficialBlock(filePath, i - 1);
		}
			

		cBlock = GetUnofficialBlock(filePath, i);
		
		if ( cBlock == NULL || lBlock == NULL){
			std::cout << "[Block Refused] Block not found in file or file corrupted." << std::endl; 
			return false;
		}
		// verify index corectness
		if ( GetBlockIndex(cBlock) != i )
		{
			std::cout << "[Block Refused] Invalid index." << std::endl;
			free(lBlock);
			free(cBlock);
			return false;
		}
		
		// get timestamp requirement 
		uint32_t MIN_TIMESTAMP = GetRequiredTimeStamp(i, firstfblockindex, lBlock, cBlock, filePath);
		if ( MIN_TIMESTAMP == 0 )
		{
			std::cout << "[Block Refused] Can't verify timestamp" << std::endl;
			free(lBlock);
			free(cBlock);
			return false;
		}

		// get hash target requirement
		unsigned char reqtarget[32];
		memset(reqtarget, 0, 32);
		GetRequiredTarget(reqtarget, firstfblockindex, cBlock,lBlock, filePath);
		
		// get b size [ shitty but workin ? ]

		// there is a problem with firstfblockindex ... 
		uint32_t boff1, boff2;
		ReadFile(filePath, 8 + (i-firstfblockindex)*4, 4, buff);
		std::cout << "will get bloc#" << (i) << " offset at " << (8 + ((i) - firstfblockindex) * 4) << std::endl;
		boff1 = BytesToUint(buff);
		if ( i != lastfblockindex )
		{
			std::cout << "will get bloc#" << (i + 1) << " offset at " << (8 + ((i+1) - firstfblockindex) * 4) << std::endl;
			ReadFile(filePath, 8 + (((i + 1) - firstfblockindex) * 4), 4, buff);
			boff2 = BytesToUint(buff);
		}
		else
		{
			FILE* f = fopen(filePath, "rb");
			if (f == NULL) { 
				std::cout << "error reading..." << std::endl;
				free(lBlock);
				free(cBlock);
				return false; } // throw error if cannot read
			fseek(f, 0, SEEK_END);
			long fsize = ftell(f);
			fclose(f);
			boff2 = fsize;
		
		}
		std::cout << "current block offset" << boff1 << std::endl;
		std::cout << "next block offset" << boff2 << std::endl; // problem here 
		uint32_t bsize = boff2 - boff1;
		std::cout << "block size is " << bsize << std::endl;
		// verify 
		if (!IsBlockValid(cBlock,lBlock,firstfblockindex,MIN_TIMESTAMP, bsize, reqtarget))
		{
			free(lBlock);
			free(cBlock);
			return false;
		}
		
		free(lBlock);
		free(cBlock);
	}
	std::cout << "Blocks file is valid." << std::endl;
	return true;
	
}
void GetRequiredTarget(unsigned char * buff, uint32_t firstbfIndex, unsigned char * cBlock, unsigned char * pBlock, const char * filePath)
{
	uint32_t bindex = GetBlockIndex(cBlock);

	if(bindex % TARGET_CLOCK && bindex >= TARGET_CLOCK)
	{
		uint32_t lindex = bindex - TARGET_CLOCK;
		unsigned char * lblock = NULL;
		if ( lindex >= firstbfIndex )
		{
			lblock = GetUnofficialBlock(filePath, lindex);
		}
		else
		{
			lblock = GetOfficialBlock(lindex);
		}
		if (lblock == NULL)
			return;

		ComputeHashTarget(GetBlockTimeStamp(cBlock), lblock, buff);
		free(lblock);
	}
	else
	{
		memcpy(buff, GetBlockHashTarget(pBlock), 32);
	}
}

uint32_t GetRequiredTimeStamp(int index , uint32_t firstbfIndex, unsigned char * lBlock, unsigned char * cBlock, const char * filePath )
{
	uint32_t result = 0;

	if (index - (TIMESTAMP_TARGET / 2) < firstbfIndex || (TIMESTAMP_TARGET / 2) > index)
	{
		unsigned char * tBlock = NULL;
		if ((TIMESTAMP_TARGET / 2) > index)
			tBlock = GetOfficialBlock(0);
		else
			tBlock = GetOfficialBlock(index - (TIMESTAMP_TARGET / 2));

		if (tBlock == NULL)
		{
			std::cout << "[Block Refused] Cannot proccess timestamp requirement [A]" << std::endl;
			return 0;
		}
		else {
			result = GetBlockTimeStamp(tBlock);
			free(tBlock);
		}
	}
	else
	{
		unsigned char * tBlock = GetUnofficialBlock(filePath, index - (TIMESTAMP_TARGET / 2));
		if (tBlock == NULL)
		{
			std::cout << "searching block #" << (TIMESTAMP_TARGET / 2) << "in inofficial" << std::endl;
			std::cout << "[Block Refused] Cannot proccess timestamp requirement [B]" << std::endl;
			return 0;

		}
		result =  GetBlockTimeStamp(tBlock);
		free(tBlock);

	}
	return result;
}


//IsBlockValid(Block b, Block prevb, uint MIN_TIME_STAMP, byte[] HASHTARGET, byte[] reqtarget, List<UTXO> vUTXO)
bool IsBlockValid(unsigned char * b, unsigned char * prevb, uint32_t firstblockindex, uint32_t MIN_TIME_STAMP, uint32_t bsize, unsigned char * reqtarget)
{
	PrintBlockInfo(b);
	uint32_t bindex = GetBlockIndex(b);
	// [0] verify hash root 
	unsigned char * ublock = (unsigned char *)malloc(bsize - 36);
	unsigned char buff[36];
	// memcpy everything except hash & nonce 
	/*
	Reminder : index (4o) . hash (32o) . phash (32o) . timestamp (4o) . hashtarget (32o) .  nonce (4 o) .  miner token [can be either 4+1 o or 64+1 o] .
	. txn ( 2o ) . txs (variable)
	*/
	memcpy(ublock, b, 4);
	memcpy(ublock+4, b+36, 68);
	memcpy(ublock + 72, b + 108, bsize - 108);
	Sha256.init();
	Sha256.write((char *)ublock, bsize - 36);
	memcpy(buff, Sha256.result(), 32);
	if (memcmp(buff, GetBlockHash(b), 32) != 0)
	{
		std::cout << "[Block Refused] Wrong hash root" << std::endl; 
		printHash(buff);
		printHash(GetBlockHash(b));
		free(ublock);
		return false;
	}
		
	free(ublock);

	// [1] verify previous hash
	if ( memcmp(GetBlockPreviousHash(b), GetBlockHash(prevb), 32) != 0)
	{
		
		std::cout << "[Block Refused] Wrong previous hash" << std::endl;
		printHash(GetBlockHash(prevb));
		printHash(GetBlockPreviousHash(b));

		return false;
	}
		
	// [2] verify timestamp 
	uint32_t bts = GetBlockTimeStamp(b);
	uint32_t cts = GetTimeStamp();
	if ( bts < MIN_TIME_STAMP || bts > cts + MAX_TIME_UP)
	{
		std::cout << "[Block Refused] Wrong time stamp" << std::endl; 
		return false;
	}
		
	// [3] verify nonce x hashtarget 
	//  here put the nonce && the header hash in buff then compare to reqtarget...
	memcpy(buff + 4, buff, 32); 
	UintToBytes(GetBlockNonce(b), buff);
	Sha256.init();
	Sha256.write((char*)buff, 36); // hash first 36 bytes ( nonce + header ) 
	if (cmp_256(Sha256.result(), reqtarget) > 0) {
		std::cout << "[Block Refused] Invalid Nonce." << std::endl;
		while ( true ){}
		return false;
	}
	// [4] verify every tx 
	uint32_t mReward = GetMiningReward(GetBlockIndex(b));
	int gas = 0;
	for (int i = 0 ; i < GetTransactionNumber(b); i++ )
	{
		unsigned char * txp = GetBlockTransaction(b, i); // don't need to free it. it's in b mem.
		if ( !IsTransactionValid(txp, firstblockindex - 1, &gas, bindex, i)  )
		{
			std::cout << "[Block Refused] Transaction not valid." << std::endl; 
			return false;
		
		}
		if (gas > MAX_GAS_SIZE)
		{
			std::cout << "[Block Refused] Transaction not valid. Gas Out." << std::endl;
			return false;
		}
			

		mReward += GetTXFee(txp);
	}
	// [5] Update miner virtual utxo with reward + mining reward (if no dust )
	
	unsigned char  mutxo[76];
	memset(mutxo, 0, 72);
	if ( GetMinerTokenFlag(b) == 1 ) // +108 means access to miner token  
	{
		unsigned char nutxo[72];
		memcpy(nutxo, b + 109, 64); // set PU KEY for new utxo
		memset(nutxo + 64, 0, 8);  // set 2 zeroed uint for new utxo ( TOU and sold )
		GetVirtualUtxo(0, firstblockindex - 1, mutxo, nutxo); 
		mReward /= 2; // PENALITY FOR NEW MINER ( REAL HARD ? )
	
	}
	else
	{
		uint32_t mutxop = BytesToUint(b + 109);
		GetVirtualUtxo(mutxop, firstblockindex - 1, mutxo);
	}
	
	if (isUtxoNull(mutxo)) // never happen ... cause utxo never null
	{
		std::cout << "[Block Refused] Cannot proccess miner UTXO." << std::endl; 
		return false;
	}
	UintToBytes(GetUtxoSold(mutxo) + mReward, mutxo + 68);
	std::cout << GetUtxoSold(mutxo) << std::endl; 
	OverWriteVirtualUtxo(mutxo);

	return true;
}


uint32_t GetMiningReward(uint32_t index )
{
	uint32_t Reward = NATIVE_REWARD;
	while (index >= HALVING_CLOCK)
	{
		index -= HALVING_CLOCK;
		Reward /= 2;
	}
	return Reward;
}

void ComputeHashTarget(uint32_t TimeStampB, unsigned char* prevbloc, unsigned char* buff)
{
	uint32_t TimeStampA = GetBlockTimeStamp(prevbloc);
	uint32_t TimeSpent = TimeStampB - TimeStampA;

	uint32_t QPLUS = TARGET_TIME * TARGET_FACTOR;
	uint32_t QMINUS = TARGET_TIME / TARGET_FACTOR;

	if (TimeSpent > QPLUS)
		TimeSpent = QPLUS;

	if (TimeSpent < QMINUS)
		TimeSpent = QMINUS;

	memcpy(buff, GetBlockHashTarget(prevbloc), 32);
	mul_256(buff, TimeSpent);
	shiftdiv_256(buff, POW_TARGET_TIME);

	// then adjust if sup than max target.  
	unsigned char ge_tar[] =
	{
	0x2F,0x95,0x5C,0x98,0x3D,0x33,0x7E,0xE6,
	0x97,0xFD,0xD,0x65,0xE7,0x37,0xC,0x62,
	0xC0,0xB,0x4,0x45,0x98,0x90,0xC2,0x7D,
	0xAC,0x75,0x65,0xBD,0x93,0x5,0xA,0x0

	};
	if (cmp_256(buff, ge_tar) > -1) {
		memcpy(buff, ge_tar, 32);
	}
}
		


void GetRelativeHashTarget(uint32_t index, unsigned char * buff) //needed if new block struct system.
{
	// IT IS NOT USED.
	uint32_t ni = nearestmultiple(index, TARGET_CLOCK, true);
	unsigned char * b = GetOfficialBlock(ni);
	memcpy(buff, GetBlockHashTarget(b), 32);
	free(b);
}