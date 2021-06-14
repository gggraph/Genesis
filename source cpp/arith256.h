#pragma once
#include "Utils.h"

void mul_256(unsigned char * A, uint32_t B);
void mulhack_256(unsigned char * A, float B);

void div_256(unsigned char * A, uint32_t B);
int cmp_256(unsigned char * A, unsigned char * B);

unsigned int bits_256(unsigned char * A);
void neg_256(unsigned char * A);
void inc_256(unsigned char * A);
void add_256(unsigned char * A, unsigned char * B);
void add_256B(unsigned char * A, uint32_t num);
void sub_256(unsigned char * A, unsigned char * B);
void shl_256(unsigned char * A, unsigned int shift);
void shr_256(unsigned char * A, unsigned int shift);

void c_256(unsigned char * buffer, const char * hexstring);
void hx_256(unsigned char * buffer, char * hexstring);



