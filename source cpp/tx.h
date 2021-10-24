#pragma once
#include "Bloc.h"
#include "uECC.h"

bool IsTransactionValid(unsigned char* TX, uint32_t blockindextime, int* gas, unsigned char* block, uint32_t txindex, const char* filePath);
bool VerifyHeaderTransaction(unsigned char * TX, uint32_t blockindextime);
bool VerifyDFT(unsigned char * TX, uint32_t blockindextime, int * gas);
bool VerifyCST(unsigned char* TX, uint32_t blockindextime, int* gas, unsigned char* block, uint32_t txindex);
bool VerifyCRT(unsigned char* TX, uint32_t blockindextime, int* gas, unsigned char* block, uint32_t txindex, const char* filePath);
bool isSignatureValid(unsigned char* TX);
bool FastApplyEXCH(unsigned char* TX, uint32_t Receiver_ptr, uint32_t amount, int blockindextime, bool _rev);

void RefreshPTXFileFromVirtualUtxoSet();
bool AddTransactionToPTXFile(unsigned char* TX, int TXSIZE);

void PrintTransaction(unsigned char* TX);
void printSignature(unsigned char* buff);

bool MakeSECP256K1PairKeys();
bool CreateDefaultTransaction(unsigned char* prKey, uint32_t utxop, uint32_t nTOU, uint32_t PurishmentTime, uint32_t fee,
	uint32_t  amount, uint32_t rutxop, unsigned char* rpuKey);


// misc
void GetRandomValidPublicKey(unsigned char* buffer);

