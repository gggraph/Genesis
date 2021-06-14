#pragma once
#include "Bloc.h"

bool IsTransactionValid(unsigned char * TX, uint32_t blockindextime, int * gas, uint32_t blocindex, uint32_t txindex);
bool VerifyHeaderTransaction(unsigned char * TX, uint32_t blockindextime);
bool VerifyDFT(unsigned char * TX, uint32_t blockindextime, int * gas);
bool VerifyCST(unsigned char * TX, uint32_t blockindextime, int * gas, uint32_t blocindex, uint32_t txindex);
bool VerifyCRT(unsigned char * TX, uint32_t blockindextime, int * gas);

void RefreshPTXFileFromVirtualUtxoSet();

