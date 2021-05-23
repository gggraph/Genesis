#ifndef Sha256_h
#define Sha256_h

#include <inttypes.h>
#include <iostream>
#include <sstream>

#define HASH_LENGTH 32
#define BLOCK_LENGTH 64

union _buffer {
	uint8_t b[BLOCK_LENGTH];
	uint32_t w[BLOCK_LENGTH / 4];
};
union _state {
	uint8_t b[HASH_LENGTH];
	uint32_t w[HASH_LENGTH / 4];
};

class Sha256Class 
{
public:
	void init(void);
	uint8_t* result(void);
	void write(char* data, int size);
private:
	void pad();
	void addUncounted(uint8_t data);
	void hashBlock();
	uint32_t ror32(uint32_t number, uint8_t bits);
	_buffer buffer;
	uint8_t bufferOffset;
	_state state;
	uint32_t byteCount;
};
extern Sha256Class Sha256;

void printHash(unsigned char * buff);
void GetHashString(unsigned char * buff, char * stringbuffer);

#endif