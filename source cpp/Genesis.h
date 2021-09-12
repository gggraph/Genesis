#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "NetServer.h"
#include "NetClient.h"
#include "sha256.h"
#include "Mine.h"
#include <windows.h>
#include "vm.h"
#include "arith256.h"
#include "tx.h"
#include "NetfileManager.h"
#include "UI.h";

bool VerifyFiles();
void CreateGenesisBlock();
void InitChain();
void Demo();

