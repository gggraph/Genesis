#pragma once
#include "Bloc.h"
#include "sha256.h"
#include "consensus.h"
#include "params.h"

void Mine(unsigned char * puKey, uint32_t max_purishing, uint32_t utxop, char * fpathbuffer);
unsigned char * GetTransactionsForNewBlock();
void IncrementUnofficialBlockFileHeader(uint32_t oBnum, unsigned char * fdata);
