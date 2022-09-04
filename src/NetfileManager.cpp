/*
will be a list of 32byte hash corresponding to file in tmp.
*/
#include "NetfileManager.h"

struct QFile {

	unsigned char id[32]; 
	unsigned char flag;
};

QFile* QPTRS[MAX_QFILE];
int currentQPtr = 0;

bool AddQFile(unsigned char * id, unsigned char flag)
{
	if (currentQPtr == MAX_QFILE) 
	{
		std::cout << "Pending queue limit reached." << std::endl;
		return false;
	}
	QPTRS[currentQPtr] = new QFile;
	QPTRS[currentQPtr]->flag = flag;
	memcpy(QPTRS[currentQPtr]->id, id, 32);
	currentQPtr++;
}

bool ProccessNextQFile() 
{
	// process last
	if (!currentQPtr)
		return false;

	unsigned char qid[32];
	unsigned char qflag = QPTRS[currentQPtr - 1]->flag;
	memcpy(qid, QPTRS[currentQPtr - 1]->id, 32); 
	// clean it
	delete QPTRS[currentQPtr - 1];
	currentQPtr--;

	char hashpath[255];
	GetHashString(qid, hashpath);
	std::ostringstream s;
	s << "tmp\\" << hashpath;
	std::string ss = s.str();
	const char* fname = ss.c_str();

	switch (qflag) {
	case 1:
		ProccessBlocksFile(fname);
		break;
	}
	
	return true;
}