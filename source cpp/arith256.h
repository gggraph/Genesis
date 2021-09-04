#pragma once
#include "Utils.h"

void shiftdiv_256(unsigned char* A, int shift);
int cmp_256(unsigned char* A, unsigned char* B);
void mul_256(unsigned char* A, int  mul);
bool add_256(uint8_t* dst, const uint8_t* src, uint8_t len, bool carry = 0);


void c_256(unsigned char * buffer, const char * hexstring);
void hx_256(unsigned char * buffer, char * hexstring);



