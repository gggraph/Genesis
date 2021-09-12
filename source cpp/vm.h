#pragma once
#include "params.h"
#include "sha256.h"

bool LoadContract(uint32_t bIndex, uint32_t TxIndex, bool _newcontract = true);
bool InitVM(unsigned char * cs, uint32_t length, uint32_t startaddress = 0x5D); // should have an entry num or 0
int RunVM(int gas, int _guserlimit, bool _debugMode = false);
bool RunVMAtPtr(uint32_t memaddr, int gas, int _guserlimit);
bool PushArgument(uint32_t arg);

void TestVM(); // unused
int TestContract(unsigned char* tContract, uint32_t cSize, unsigned char* tRequest, uint32_t rSize);

unsigned char *  GetREGaddr(unsigned char val, bool s, bool b16);
void PrintReg();
void PrintMem(uint32_t add);
void PrintTopStack();
void PrintStackRange(int r);
