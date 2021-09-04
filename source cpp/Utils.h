#pragma once
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <chrono>
#include <io.h>
#include <filesystem>
#include <experimental/filesystem>
#include <iostream>

typedef unsigned char byte;
#define MAX_FILE_SIZE 4294965097; // en pratique ne peut pax exceder uint32 max value ( 4gb ) fat32 format and &other stuff 

uint32_t GetTimeStamp();

uint32_t nearestmultiple(uint32_t numToRound, uint32_t multiple, bool flr);

void SetBit(int p, bool b, unsigned char * byte);
bool IsBitSet(int p, unsigned char byte);

uint16_t BytesToShort(unsigned char *arr);
uint32_t BytesToUint(unsigned char *arr);
void UintToBytes(uint32_t v, unsigned char *a);

bool DeleteDirectory(const char * dirName);
bool FileExists(const char * fileName);

void ReadFile(const char * fname, long position, int length, unsigned char *data);
void OverWriteFile(const char * fname, long position, unsigned char *data, int length);
void AppendFile(const char * fname, unsigned char *data, int length);
