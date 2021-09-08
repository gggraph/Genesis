#pragma once
#include "Bloc.h"
#include "uECC.h"

bool IsTransactionValid(unsigned char * TX, uint32_t blockindextime, int * gas, uint32_t blocindex, uint32_t txindex);
bool VerifyHeaderTransaction(unsigned char * TX, uint32_t blockindextime);
bool VerifyDFT(unsigned char * TX, uint32_t blockindextime, int * gas);
bool VerifyCST(unsigned char * TX, uint32_t blockindextime, int * gas, uint32_t blocindex, uint32_t txindex);
bool VerifyCRT(unsigned char * TX, uint32_t blockindextime, int * gas);
bool isSignatureValid(unsigned char* TX);

void RefreshPTXFileFromVirtualUtxoSet();
bool AddTransactionToPTXFile(unsigned char* TX, int TXSIZE);

void PrintTransaction(unsigned char* TX);
void printSignature(unsigned char* buff);

bool MakeSECP256K1PairKeys();
bool CreateDefaultTransaction(unsigned char* prKey, uint32_t utxop, uint32_t nTOU, uint32_t PurishmentTime, uint32_t fee,
	uint32_t  amount, uint32_t rutxop, unsigned char* rpuKey);


// misc
void GetRandomValidPublicKey(unsigned char* buffer);

