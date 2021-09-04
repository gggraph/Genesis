#include "Utils.h"


uint32_t GetTimeStamp()
{
	const auto p1 = std::chrono::system_clock::now();
	return (uint32_t)std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
}


uint32_t nearestmultiple(uint32_t numToRound, uint32_t multiple, bool flr)
{
	if (multiple == 0)
		return numToRound;

	uint32_t remainder = numToRound % multiple;
	if (remainder == 0)
		return numToRound;

	if (!flr)
		return numToRound + multiple - remainder;
	else
		return numToRound - remainder;
}

// we should not invert 
void SetBit(int p, bool b, unsigned char * byte)// not working here cause only for int 
{
	if (b)
		*byte |= 1UL << (7 - p); 
	else
		*byte &= ~(1UL << (7 - p));

}
bool IsBitSet(int p, unsigned char byte) 
{
	return (byte >> (7 - p)) & 1U;// not working here 
}


uint32_t BytesToUint(unsigned char *arr)
{
	uint32_t foo;
	// big endian
	foo = (uint32_t)arr [3] << 24;
	foo |= (uint32_t)arr[2] << 16;
	foo |= (uint32_t)arr[1] << 8;
	foo |= (uint32_t)arr[0];
	
	return foo;
}

void UintToBytes(uint32_t v, unsigned char *a )
{
	a[3] = v >> 24;
	a[2] = v >> 16;
	a[1] = v >> 8;
	a[0] = v;
}

uint16_t BytesToShort(unsigned char *arr)
{
	return  ((arr[1] << 8) | arr[0]);
}


bool FileExists(const char * fileName)
{
	return _access(fileName, 0) == 0;
}

bool DeleteDirectory(const char * dirName)
{
	if (!FileExists(dirName))
		return false;
	
	std::experimental::filesystem::remove_all(dirName);
	return true;
}

void ReadFile(const char * fname, long position, int length, unsigned char *data)
{
	FILE* f = fopen(fname, "rb");
	if (f == NULL) {
		return;
	}
	fseek(f, position, SEEK_SET);
	size_t n = fread(data, 1, length, f);
	if ( n != length )
	{	
		if( feof(f))
		{
			std::cout << "error file end. fread stop at " << n << " : " << position<< ":" << length<<  std::endl;
		}
		if (ferror(f))
		{
			std::cout << "error file read " << n << std::endl;
		}
		fclose(f);
		return; 
	}
	fclose(f);
}
void OverWriteFile(const char * fname, long position, unsigned char *data, int length)
{
	FILE* f = fopen(fname, "r+b");
	if (f == NULL) return;
	fseek(f, position, SEEK_SET);
	int nsize = fwrite(data, 1, length, f);
	fclose(f);
}
void AppendFile(const char * fname, unsigned char *data, int length)
{
	FILE* f = fopen(fname, "ab");
	if (f == NULL) return;
	fseek(f, 0, SEEK_END);
	fwrite(data, sizeof(unsigned char), length, f);
	fclose(f);
}

void ReadFileB()
{
	FILE* f = fopen("File.txt", "r");
	if (f == NULL) { exit(1); } // throw error if cannot read
	/*
	// obtain file size:
	fseek(f, 0, SEEK_END);
	lSize = ftell(f);
	rewind(f);
	*/
	// set file pointer to 2nd position(0 - indexed)
	fseek(f, 1, SEEK_SET);
	// prepare buf of 0 bytes
	char buf[10];
	memset(buf, 0, 10); // buffer = (char*)malloc(sizeof(char)*lSize); <--------- should do malloc to free after it : 	free(buffer);

	// read 9 members of size 1 (characters) from f into part
	fread(buf, 1, 9, f);
	fclose(f);
}
