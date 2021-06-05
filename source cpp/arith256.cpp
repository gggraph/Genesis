/*
For POW we just need this :
mul 256b with uint
div 256b with uint
cmp 256b with 256b
(create from hex string) 
(to hex ) 
*/
#include <stdint.h>
#include "arith256.h"
#include <sstream>

 
void mul_256(unsigned char * A, uint32_t B)
{
	uint64_t carry = 0;
	for (int i = 0; i < 32; i++) {
		uint64_t n = carry + (uint64_t)B * A[i];
		A[i] = n & 0xffffffff;
		carry = n >> 32;
	}
}

unsigned int bits_256(unsigned char * A)
{
	for (int pos = 32 - 1; pos >= 0; pos--) {
		if (A[pos]) {
			for (int nbits = 31; nbits > 0; nbits--) {
				if (A[pos] & 1U << nbits)
					return 32 * pos + nbits + 1;
			}
			return 32 * pos + 1;
		}
	}
	return 0;
}
void div_256(unsigned char * A, uint32_t B)
{ 

	// if b 0 return
	unsigned char dividend[32];
	memcpy(dividend, A, 32);
	unsigned char divisor[32];
	divisor[31] = 2;
	memset(A, 0, 32); 

	// this is used for fast perf
	int num_bits = bits_256(dividend);
	int div_bits = bits_256(divisor);
	int shift = num_bits - div_bits;
	
	shl_256(divisor, shift);
	while ( shift >= 0 )
	{
		if (cmp_256(dividend, divisor) > -1 )
		{
			sub_256(dividend, divisor);
			A[shift / 32] |= (1 << (shift & 31));
		}
		shift--;
		shr_256(divisor, 1);
	}

	return;

}
void neg_256 (unsigned char * A)
{
	
	for (int i = 0; i < 32; i++)
		A[i] = ~A[i];
	
	inc_256(A);
}

void inc_256(unsigned char * A ) 
{
	// prefix operator
	int i = 0;
	while (i < 32 && ++A[i] == 0)
		i++;
}
void add_256(unsigned char * A, unsigned char * B)
{
	uint64_t carry = 0;
	for (int i = 0; i < 32; i++)
	{
		uint64_t n = carry + A[i] + B[i];
		A[i] = n & 0xffffffff;
		carry = n >> 32;
	}
}

void sub_256(unsigned char * A, unsigned char * B )  
{
	neg_256(B);
	add_256(A, B);
	neg_256(B); // undo negate. maybe do memcpy for better perf ?
}

void shl_256(unsigned char * A,unsigned int shift )
{
	unsigned char a[32];
	memcpy(a, A, 32);
	for (int i = 0; i < 32; i++)
		A[i] = 0;
	int k = shift / 32;
	shift = shift % 32;
	for (int i = 0; i < 32; i++) {
		if (i + k + 1 < 32 && shift != 0)
			A[i + k + 1] |= (a[i] >> (32 - shift));
		if (i + k < 32)
			A[i + k] |= (a[i] << shift);
	}
}
void shr_256(unsigned char * A, unsigned int shift)
{
	unsigned char a[32];
	memcpy(a, A, 32);
	for (int i = 0; i < 32; i++)
		A[i] = 0;
	int k = shift / 32;
	shift = shift % 32;
	for (int i = 0; i < 32; i++) {
		if (i - k - 1 >= 0 && shift != 0)
			A[i - k - 1] |= (a[i] << (32 - shift));
		if (i - k >= 0)
			A[i - k] |= (a[i] >> shift);
	}
}

// cmp function. 0 is equal. +1 is A higher, -1 is A lower
int cmp_256(unsigned char * A, unsigned char * B)
{
	for (int i = 32 - 1; i >= 0; i--) {
		if (A[i] < B[i])
			return -1;
		if (A[i] > B[i])
			return 1;
	}
	return 0; 
}
// construct from hex . 
void c_256(unsigned char * buffer, const char * hexstring) 
{

}
// get hex ( same as hash to hex )
void hx_256(unsigned char * buffer,  char * hexstring)
{
	std::ostringstream s;
	for (int i = 0; i < 32; i++) {
		s << ("0123456789abcdef"[buffer[i] >> 4]);
		s << ("0123456789abcdef"[buffer[i] & 0xf]);
		s << " ";
	}
	strcpy(hexstring, s.str().c_str());
}