#pragma once
#include "params.h"
#include "sha256.h"

// unused 
bool LoadContract(uint32_t bIndex, uint32_t TxIndex, bool _newcontract = true);
bool RunVMAtPtr(uint32_t memaddr, int gas, int _guserlimit);

// core function
bool InitVM(unsigned char* cs, uint32_t length, uint32_t startaddress = 0x5D); // should have an entry num or 0
int RunVM(int gas, int _guserlimit, unsigned char* block, unsigned char* TX, int blockindextime, bool _revertMode, bool _safeMode);

int RunCRT
(
	unsigned char* tContract, uint32_t cSize, unsigned char* tRequest, uint32_t rSize, unsigned char* contractBlock,
	unsigned char* cblock, int blockindextime, int txindex, int currentgas, bool _safeMode, bool _revMode
);
int RunCST(unsigned char* tContract, uint32_t cSize, unsigned char* cblock, int blockindextime, int txindex, int currentgas, bool _safeMode, bool _revMode);

bool PushArgument(uint32_t arg);
int TestContract(unsigned char* tContract, uint32_t cSize, unsigned char* tRequest, uint32_t rSize, bool _reloadContract = true);

int ReadSafeStorage(int storageAddr, int memOffset, int elementCount, bool _revMode);
int WriteSafeStorage(int storageAddr, int memOffset, unsigned char elementSize, int elementCount, bool _revMode, bool _Append);
bool UpdateStatesFromSafeStorage(uint32_t blocindex, uint32_t txindex);
bool UpdateStatesFromSafeStorage(char* STORAGEPATH);
unsigned char *  GetREGaddr(unsigned char val, bool s, bool b16);
void PrintReg();
void PrintMem(uint32_t add);
void PrintTopStack();
void PrintStackRange(int r);
