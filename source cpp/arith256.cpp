
#include <stdint.h>
#include "arith256.h"
#include <sstream>
/*
shiftdiv OK
cmp		 OK

*/

int cmp_256(unsigned char* A, unsigned char* B)
{
	// return -1 if A is lesser than B. return 1 if greater , return 0 if equal
	for (int i = 31; i >= 0; i--) {
		
		if (A[i] < B[i]) {
			
			return -1;
		}

		if (A[i] > B[i]) {

			return 1;
		}
			
	}
	return 0;
}

bool add_256(uint8_t* dst, const uint8_t* src, uint8_t len, bool carry)
{
	while (len--) {
		uint8_t pre = *dst;
		uint8_t post = pre + (*src++) + carry;
		*dst++ = post;
		carry = (post < pre) || (carry && (post == pre));
	}
	return carry;
}

void mul_256(unsigned char* A, int  mul)
{
	// a bit hacky i know
	if (mul == 0) {
		memset(A, 0, 32); return;
	}
	if (mul == 1) {
		 return;
	}
	mul--; 

	unsigned char B[32];
	memcpy(B, A, 32);
	for (int i = 0; i < mul; i++) {
		add_256(A, B, 32);
	}
}

void shiftdiv_256(unsigned char* A, int shift)
{
	
	for (int i = 0; i < shift; i++) {
		byte carry = 0;
		int k;
		for (k = 32 - 1; k >= 0; k--) {
			bool ncarry = false;
			if (A[k] & 0x01) {
				// carry as to be set 
				ncarry = true;
			}
			A[k] = (A[k] >> 1) | carry; // bit shift right by one and OR the carry  (0x80) 
			// update carry 
			if (!ncarry) {
				carry = 0;
			}
			else {
				carry = 0x80;
			}
		}

	}
	
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