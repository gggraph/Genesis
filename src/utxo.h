#pragma once
#include "Utils.h"
#include "Bloc.h"
#include <sstream>

void PrintUTXO(unsigned char* UTXO);

uint32_t GetUtxoPointer(unsigned char * puKey);
void GetUtxo(uint32_t index, unsigned char *  buff);
void GetVirtualUtxo(uint32_t utxop, uint32_t blockindextime, unsigned char * rvUtxo, unsigned char * nUtxo = NULL);
void GetVirtualUtxoInTempFile(unsigned char * buff, uint32_t index, unsigned char * puKey = NULL);

void DowngradeUtxoAtSpecificBlockTime(unsigned char * utxo, uint32_t index);
void DownGradeUtxoFromSpecificTransaction(unsigned char* utxo, unsigned char* TX);

void UpdateUtxoSet();
void OverWriteVirtualUtxo(unsigned char * nUtxo);
void OverWriteUtxo(unsigned char * nUtxo, uint32_t index);
void AddUtxo(unsigned char * nUtxo);

bool isUtxoNull(unsigned char * utxo);
unsigned char * GetUtxoPuKey(unsigned char * utxo);
uint32_t GetUtxoTOU(unsigned char * utxo);
uint32_t  GetVirtualUtxoOffset(unsigned char * utxo);
uint32_t GetUtxoSold(unsigned char * utxo);

