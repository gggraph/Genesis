#pragma once
#include "bloc.h"
#include "params.h"
#include "tx.h";

void ComputeHashTarget(uint32_t TimeStampB, unsigned char * prevbloc, unsigned char * buff);
void GetRelativeHashTarget(uint32_t index, unsigned char * buff);

bool IsBlockValid(unsigned char * b, unsigned char * prevb, uint32_t firstblockindex, uint32_t MIN_TIME_STAMP, uint32_t bsize, unsigned char * reqtargetunsigned, const char* filePath);
void ProccessBlocksFile(const char * filePath);
bool VerifyBlocksFile(const char * filePath, uint32_t lastOfficialindex, uint32_t firstfblockindex, uint32_t lastfblockindex);
void UpdateBlockchain(const char * filePath);
uint32_t GetRequiredTimeStamp(int index,uint32_t firstbfIndex, unsigned char * lBlock, unsigned char * cBlock, const char * filePath);
void GetRequiredTarget(unsigned char * buff, uint32_t firstbfIndex, unsigned char * cBlock, unsigned char * lBlock, const char * filePath);

uint32_t GetMiningReward(uint32_t index);