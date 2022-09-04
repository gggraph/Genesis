#pragma comment(lib,"winmm.lib")
#include "Utils.h"

short int* RecordMic(int seconds);
void PlayRecord(unsigned char* data, int seconds);