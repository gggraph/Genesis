
#include <stdint.h>
#include "arith256.h"
#include <sstream>
/*

For POW we just need this :
library is broken. 

only cmp work or seems too.
div is not working at all. 

(to hex )

REMARK: this was taken from bitcoin core.


RIEN NE MARCHE. BITCOIN SEMBLE FAIRE BEAUCOUP D APPROXIMATION . 
*/


void mulhack_256( unsigned char * A, float B)
{
	int dp_ctr = 0;
	while ( B - int(B) != 0)
	{
		B *= 10;
		dp_ctr++;
	}
	unsigned char ext[34]; // extended  2 byte ( but could be more if needed ) 
	memset(ext, 0, 2);
	memcpy(ext + 2, A, 32);

	uint64_t carry = 0;

	for (int i = 0; i < 34; i++) {
		uint64_t n = carry + (uint64_t)B * ext[i];
		ext[i] = n & 0xffffffff;
		carry = n >> 32;
	}

	/*look */
	int64_t invDivisor = 0x1999999A;
	carry = 0;
	for (int i = 0; i < dp_ctr; i++) {
		uint64_t n = carry + (uint64_t)invDivisor * ext[i];
		ext[i] = n & 0xffffffff;
		carry = n >> 32;
	}

	memcpy(A, ext, 32);
	// now get out those bad number 
}




void mul_256(unsigned char * A, uint32_t mult)
{

	// left shift is mul by 2¨1 = 2. 
	// i need to find the 

}

// we get highest square of uint256  for fast division.
unsigned int bits_256(unsigned char * A)
{
	for (int pos = 31; pos >= 0; pos--) {
		if (A[pos]) { // means byte is not 0
			for (int nbits = 31; nbits> 0; nbits--) {
				if (A[pos] & 1U << nbits) // means 
					return 32 * pos + nbits + 1;
			}
			return 32 * pos + 1;
		}
	}
	// big endian
	/*
	for (int pos = 0 ; pos < 32; pos++) {
		if (A[pos]) { // means byte is not 0
			for (int nbits = 0; nbits <31; nbits++) {
				if (A[pos] & 1U << nbits) // means 
					return 32 * pos + nbits + 1;
			}
			return 32 * pos + 1;
		}
	}
	*/
	return 0;
}
void div_256(unsigned char * A, uint32_t B)
{ 
	if (!B) // illegal
		return;

	unsigned char dividend[32];
	memcpy(dividend, A, 32);
	unsigned char divisor[32];
	memset(divisor, 0, 32);
	memcpy(divisor+28, &B, 4); // big endianness
	memset(A, 0, 32);
	// ok until here





	char buff[255];
	hx_256(divisor, buff);
	std::cout << buff << std::endl;
	shl_256(divisor, 20);
	hx_256(divisor, buff);
	std::cout << buff << std::endl;
	while (1) {}

	/*
	// shl is not working ... 
	shl_256(divisor,4 );// is like multiply per 2 
	shl_256(divisor, 4);
	hx_256(divisor, buff);
	std::cout << buff;
	// new tactics get closest square 2 of B. // so get its first bit position. ( cause big endian ) 
	
	while (true )
	{
	
	
	}

	*/
	return; 
	// this is used for fast perf
	int num_bits = bits_256(dividend);
	int div_bits = bits_256(divisor);
	int shift = num_bits - div_bits;
	std::cout << num_bits << std::endl;
	std::cout << div_bits << std::endl;
	std::cout << shift << std::endl;
	shl_256(divisor, shift); // how many times i have to shift r 
	hx_256(divisor, buff);
	std::cout << buff;

	while ( shift >= 0 )
	{
		if (cmp_256(dividend, divisor) > -1 )
		{
			sub_256(dividend, divisor); // why sub and not shifting ????
			A[shift / 32] |= (1 << (shift & 31));  // set the bit 
		}
		shift--;
		shr_256(divisor, 1);
		std::cout << shift << std::endl; // it shift x times but it is not working 
	}
	
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
void add_256B(unsigned char * A, uint32_t num)
{
	unsigned char B[32]; 
	memset(B, 0, 28);
	memcpy(B + 28, &num, 4);
	
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
	uint64_t o1, o2, o3, o4, i1, i2, i3, i4;


	memcpy(&i4, A, 8);
	memcpy(&i3, A + 8, 8);
	memcpy(&i2, A + 16, 8);
	memcpy(&i1, A + 24, 8);

	o1 = i1 << shift | i2 >> (32 - shift);
	o2 = i2 << shift | i3 >> (32 - shift);
	o3 = i3 << shift | i4 >> (32 - shift);
	o4 = i4 << shift;

	memcpy(A, &o4, 8);
	memcpy(A + 8, &o3, 8);
	memcpy(A + 16, &o2, 8);
	memcpy(A + 24, &o1, 8);

	return;


	memcpy(&i4, A, 8);
	memcpy(&i3, A+8, 8);
	memcpy(&i2, A+16, 8);
	memcpy(&i1, A+24, 8);

	o4 = i4 << shift | i3 >> (32 - shift);
	o3 = i3 << shift | i2 >> (32 - shift);
	o2 = i2 << shift | i1 >> (32 - shift);
	o1 = i1 << shift;

	memcpy(A,    &o4, 8);
	memcpy(A+8,  &o3, 8);
	memcpy(A+16, &o2, 8);
	memcpy(A+24, &o1, 8);

	return;
	
	// those aren't working ...

	unsigned char a[32];
	memcpy(a, A, 32);
	memset(A, 0, 32);

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
// this is the only thing that work in this lib...
int cmp_256(unsigned char * A, unsigned char * B)
{
	// big endianness.
	for (int i = 0; i < 32 ; i++) {
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
void add_dddd(int x, int y , char * str)
{


}
void dc_256( char * hex, char * string)
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