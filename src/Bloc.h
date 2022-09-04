#pragma once
#include "utxo.h"


void PrintBlockInfo(unsigned char * b);

unsigned char*  GetOfficialBlock(uint32_t index);
unsigned char * GetUnofficialBlock(const char *filePath, uint32_t index);
unsigned char * GetBlock(const char *filePath, uint32_t bOff);

void GetLatestBlockFilePath(char * buffer); // <* not used 
uint32_t GetLatestBlockIndex(bool official);
unsigned char* GetLatestBlock(bool official);

uint32_t GetBlockIndex(unsigned char * block);
unsigned char * GetBlockHash(unsigned char * block);
unsigned char * GetBlockPreviousHash(unsigned char * block);
uint32_t GetBlockTimeStamp(unsigned char * block);
unsigned char * GetBlockHashTarget(unsigned char * block);
uint32_t GetBlockNonce(unsigned char * block);
unsigned char GetMinerTokenFlag(unsigned char * block);
uint16_t GetTransactionNumber(unsigned char * block);

unsigned char * GetBlockTransaction(unsigned char * block, uint32_t index);
void GetTXsPuKey(unsigned char * buffer, unsigned char * TX);
unsigned char * GetTXSignature(unsigned char * TX);
uint32_t GetTXTokenOfUniqueness(unsigned char * TX);
uint32_t GetTXPurishmentDate(unsigned char * TX);
uint32_t GetTXFee(unsigned char * TX);
unsigned char GetTXByteID(unsigned char * TX);
uint32_t GetTXDataSize(unsigned char * TX);
unsigned char * GetTXData(unsigned char * TX);
void LoadBlockPointers();

void UpdateOfficialChain(const char * filePath);


extern unsigned unsigned char* BLOCKSPTR;
