#pragma once
#include "Utils.h"

bool InitVM(unsigned char * cs, uint32_t length);
bool RunVM();
unsigned char *  GetREGaddr(unsigned char val, bool s, bool b16);
void PrintReg();
void PrintMem(uint32_t add);
