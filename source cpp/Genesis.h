#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


bool VerifyFiles();
void CreateGenesisBlock();
void InitChain();
void Demo();
/*
unsigned char GENESIS_TARGET [] =
{
	0x2F,0x95,0x5C,0x98,0x3D,0x33,0x7E,0xE6,
	0x97,0xFD,0xD,0x65,0xE7,0x37,0xC,0x62,
	0xC0,0xB,0x4,0x45,0x98,0x90,0xC2,0x7D,
	0xAC,0x75,0x65,0xBD,0x93,0x5,0x0,0x0
};
*/

unsigned char GENESIS_TARGET[] =
{
	0x2F,0x95,0x5C,0x98,0x3D,0x33,0x7E,0xE6,
	0x97,0xFD,0xD,0x65,0xE7,0x37,0xC,0x62,
	0xC0,0xB,0x4,0x45,0x98,0x90,0xC2,0x7D,
	0xAC,0x75,0x65,0xBD,0x93,0x5,0xA,0x0
};