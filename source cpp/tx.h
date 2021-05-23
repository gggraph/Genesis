#pragma once
#include "Bloc.h"

bool IsTransactionValid(unsigned char * TX, uint32_t blockindextime);
bool VerifyHeaderTransaction(unsigned char * TX);
bool VerifyDFT(unsigned char * TX, uint32_t blockindextime);
bool VerifyCST(unsigned char * TX, uint32_t blockindextime);
bool VerifyCRT(unsigned char * TX, uint32_t blockindextime);

void RefreshPTXFileFromVirtualUtxoSet();

