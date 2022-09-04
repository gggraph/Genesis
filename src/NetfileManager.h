#pragma once
#include "Utils.h"
#include "Consensus.h"
#include "sha256.h"

#define MAX_QFILE 255
bool AddQFile(unsigned char* id, unsigned char flag);
bool ProccessNextQFile();

extern int currentQPtr;