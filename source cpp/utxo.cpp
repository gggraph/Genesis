#include "utxo.h"
#include "consensus.h"
/*
UTXO STRUCT  :  (72) or (76 virtual) / before it was 540 or 544
PUKEY      64 B
TOU        4  B
SOLD       4  B
VPTR       4  B ( only on virtual UTXO ) 
*/

unsigned char * GetUtxoPuKey(unsigned char * utxo) // return ptr (0) not used ? 64 bytes
{
	return utxo;
}
uint32_t GetUtxoTOU(unsigned char * utxo)
{
	return BytesToUint(utxo + 64);
}
uint32_t GetUtxoSold(unsigned char * utxo)
{
	return BytesToUint(utxo + 68);
}
uint32_t  GetVirtualUtxoOffset(unsigned char * utxo) // virtual set ptr 4 fast overwriting
{
	return BytesToUint(utxo + 72);
}


uint32_t GetUtxoPointer(unsigned char * puKey) // really slow . only work with utxos//0 4 the moment 
{
	std::cout << "START SEARCHING UTXO PTR" << std::endl;
	uint32_t filenum = 0;
	uint32_t boff = 0;

	std::ostringstream s;
	s << "utxos\\" << filenum;
	std::string ss = s.str();
	const char * filePath = ss.c_str();
	FILE* f = fopen(filePath, "rb");
	if (f == NULL) {  return 0; }
	// obtain file size:
	fseek(f, 0, SEEK_END);
	uint32_t lSize = ftell(f);
	rewind(f);
	// secure 
	uint32_t utxop = 0;
	unsigned char fdata[64];
	while ( boff < lSize)
	{
		fseek(f, boff, SEEK_SET);
		fread(fdata, 1, 64, f);
		if (memcmp(puKey, fdata, 64) == 0)
		{
			std::cout << "UTXO FOUND" << std::endl;
			return utxop;
		}
		utxop++;
		boff += 72;
	}
	fclose(f);
	std::cout << "NOT FOUND" << std::endl;
	return 0;
}

bool isUtxoNull(unsigned char * utxo)
{
	// yes if pukey is full of 0 
	for (int i = 0; i < 72 ; i++ )
	{
		if (utxo[i] != 0) {
			return false;
		}
	}
	return true;
}

void GetUtxo(uint32_t index, unsigned char * buff) // have to arg a buff pointer address
{
	// WARNING IT NOT NULLIFY
	// [0] getting utxo set file
	uint32_t mfs = MAX_FILE_SIZE;
	uint32_t filenum = (72 * index) / mfs;
	uint32_t utxonumperfile = mfs / 72; // it is equal to 1 
	uint32_t bOff = (72 * index) - (filenum*utxonumperfile);
	std::ostringstream s;
	s << "utxos\\" << filenum;
	std::string ss = s.str();
	const char * filePath = ss.c_str();
	FILE* f = fopen(filePath, "rb");
	// [1] security
	if (f == NULL) { return; } // unvalid file
	// obtain file size:
	fseek(f, 0, SEEK_END);
	uint32_t lSize = ftell(f);
	rewind(f);
	// secure 
	if (lSize < bOff + 72)
	{
		fclose(f);
		return;
	}
	// [2] load 
	fseek(f, bOff, SEEK_SET);
	fread(buff, 1, 72, f);
	fclose(f);
}

void GetVirtualUtxoInTempFile(unsigned char * buff, uint32_t index, unsigned char * puKey )  // give me big shit 
// Reading tmp file until we found virutal utxo.  
{
	FILE* f = fopen("utxos\\tmp", "rb"); 
	if (f == NULL) { return; } // throw error if cannot read
	// obtain file size:
	fseek(f, 0, SEEK_END);
	uint32_t lSize = ftell(f);
	rewind(f);
	uint32_t boff = 0;
	uint32_t vIndex = 0;
	while (boff < lSize) 
	{
		unsigned char fdata[72]; 
		if ( index == 0 )
		{
			fseek(f, boff+4, SEEK_SET);
			fread(fdata, 1, 72, f);
			if (memcmp(puKey, fdata, 64) == 0)
			{
				memcpy(buff, fdata, 72);
				UintToBytes(vIndex, buff + 72); // add virtual set index
				fclose(f);
				return;
			}
		}
		else
		{
			fseek(f, boff, SEEK_SET);
			fread(fdata, 1, 4, f);
			if ( BytesToUint(fdata) == index )
			{
				fseek(f, boff+4, SEEK_SET);
				fread(buff, 1, 72, f);
				UintToBytes(vIndex, buff + 72); // add virtual set index
				fclose(f);
				return;
			}
		}

		boff += 76;
	 	vIndex++;
	}
	fclose(f);
	std::cout << "RETURNING NULLIFY POINTER";
	return;
}

void GetVirtualUtxo(uint32_t utxop, uint32_t blockindextime, unsigned char * rvUtxo, unsigned char * nUtxo)
// Everything start here : 
// add official utxo to virtual set if not already existing (at from specific index time ) and return a virtual utxo [76 bytes]
{
	unsigned char utxo[76]; // NOT WORKIN
	memset(utxo, 0, 72); // always zeroing utxo

	if (utxop != 0)
	{
		// if not new utxo : GetVirtualUtxoInTempFile 
		GetVirtualUtxoInTempFile(utxo, utxop);
		
		if (!isUtxoNull(utxo)) // means if utxo[0]
		{
			// return utxo found in vutxos
			memcpy(rvUtxo, utxo, 76);
			return;
		}
		else
		{
			// not found? add it 
			FILE* f = fopen("utxos\\tmp", "ab");
			if (f == NULL) {  return; }
			
			fseek(f, 0, SEEK_END);
			// append utxop (4 bytes ) 
			UintToBytes(utxop, utxo);
			fwrite(utxo, 1, 4, f); //< not working... it is weird ... 
			// get official utxo
			GetUtxo(utxop, utxo);  
			if (isUtxoNull(utxo)) { 
				return; 
			}
			// downgrade if needed
			DowngradeUtxoAtSpecificBlockTime(utxo, blockindextime);
			// append 72 bytes ( utxo ) 
			fwrite(utxo, 1, 72, f);
			// copy to return utxo
			memcpy(rvUtxo, utxo, 72);
			uint32_t lSize = ftell(f);
			// add utxo count ... seems ok 
			UintToBytes((lSize / 76)-1, rvUtxo + 72); // add extra info bytes (76)
			fclose(f);
			
			return;
		}
	}
	else
	{
		if (nUtxo == NULL) // ok here can we can send a null pointer
		{
			std::cout << "fatal error : no utxo provide ... ." << std::endl; 
			while (1){}
			return; // fatal error
		}
		
		FILE* f = fopen("utxos\\tmp", "ab");
		if (f == NULL) {  return; }

		memset(utxo, 0, 4); // zeroing index 
		fseek(f, 0, SEEK_END);
		fwrite(utxo, 1, 4, f); // write index 0 
		fwrite(nUtxo, 1, 72, f);
		memcpy(rvUtxo, nUtxo, 72); // ok

		uint32_t lSize = ftell(f);
		UintToBytes((lSize / 76) -1, rvUtxo + 72);
		fclose(f);
		return;
	
	}
	 
}

void DowngradeUtxoAtSpecificBlockTime(unsigned char * utxo , uint32_t index)
{
	uint32_t bi = GetLatestBlockIndex(true);
	for (int i = bi - 1; i >= index; i--) {
		unsigned char* currentb = GetOfficialBlock(i); 
		uint16_t txn = GetTransactionNumber(currentb);
		uint32_t totfee = GetMiningReward(i);
		for (int n = txn - 1; n >= 0; n--) {
			unsigned char* TX = GetBlockTransaction(currentb, n);
			DownGradeUtxoFromSpecificTransaction(utxo, TX);
			totfee += GetTXFee(TX);
		}
		// if utxo is miner of the block lower fee + mining reward ...
		unsigned char  mutxo[72];
		unsigned char tokenflag = GetMinerTokenFlag(currentb); 
		if (tokenflag == 1) {
			memcpy(mutxo, currentb + 109, 64);
		}
		else {
			GetUtxo(BytesToUint(currentb + 109), mutxo);
		}
		if (memcmp(GetUtxoPuKey(utxo), mutxo, 64) == 0) {
			uint32_t oldsold = GetUtxoSold(utxo);
			oldsold -= totfee; // lower sold of miner
			memcpy(mutxo + 68, &oldsold, 4);
			std::cout << "lowering by  "<< totfee << std::endl;
		}
		free(currentb);
	}
}

void DownGradeUtxoFromSpecificTransaction(unsigned char* utxo, unsigned char* TX) 
{
	// will depend either if it is a specific DFT, CST  or CRT for the sender
	// process sender UTXO 
	unsigned char keybuff[64];
	unsigned char* TXDATA = GetTXData(TX);
	uint32_t oldsold = GetUtxoSold(utxo);
	GetTXsPuKey(keybuff, TX);
	if (memcmp(GetUtxoPuKey(utxo), keybuff, 64) == 0) {
		// first get back fee & tou
		uint32_t oldtou = GetUtxoTOU(utxo);
		oldtou--;
		oldsold += GetTXFee(TX);  // get back fee
		memcpy(utxo + 64, &oldtou, 4);
		switch (GetTXByteID(TX)) {
		case 0: 
			// get back the amount
			oldsold += BytesToUint(TXDATA + 1); // get back amount
			std::cout << "adding by  " << BytesToUint(TXDATA + 1) << std::endl;
			break;
		}
		// update sold 
		memcpy(utxo + 68, &oldsold, 4);
	}
	// process receiver UTXO  
	if (GetTXByteID(TX) == 0) {
		
		// update receiver sold if needed ( lower the amount ) 
		if (TXDATA[0] == 0) { // receiver exist  
			// seems kinda slow here ...
			unsigned char  candidate[72]; 
			GetUtxo(BytesToUint(TXDATA + 5), candidate);
			if (memcmp(GetUtxoPuKey(utxo), GetUtxoPuKey(candidate), 32) == 0) {
				oldsold -= BytesToUint(TXDATA + 1);
				memcpy(utxo + 68, &oldsold, 4);
			}
		}
		else {
			if (memcmp(GetUtxoPuKey(utxo), TXDATA + 5, 32) == 0) {
				oldsold -= BytesToUint(TXDATA + 1);
				memcpy(utxo + 68, &oldsold, 4);
			}
		}
		
	}
	
}

void UpdateUtxoSet() // JUST UPDATING VIRTUAL UTXO SET WITH utxo//tmp 
{
	
	// vutxo format : 
	std::cout << "UTXO starting updating" << std::endl;
	
	FILE* f = fopen("utxos\\tmp", "rb");
	if (f == NULL) { return; } // throw error if cannot read
	// obtain file size:
	fseek(f, 0, SEEK_END);
	uint32_t lSize = ftell(f);
	rewind(f);
	uint32_t boff = 0;

	unsigned char buffer[72];

	while (boff < lSize )
	{
		//fseek(f, boff , SEEK_SET);
		fread(buffer, 1, 4, f); // advance curso 4 
		uint32_t utxop = BytesToUint(buffer);
		std::cout << "________________UTXOP F : " << utxop << std::endl;
		if ( utxop == 0 )
		{
			// append
			fread(buffer, 1, 72, f);
			std::cout << "APPENDING TOU:" << BytesToUint(buffer + 64) << " SOLD: " << BytesToUint(buffer + 68) << std::endl;
			AddUtxo(buffer);
			
		}
		else
		{
			// overwrite at 
			fread(buffer, 1, 72, f);
			std::cout << "APPENDING TOU:" << BytesToUint(buffer + 64) << " SOLD: " << BytesToUint(buffer + 68) << std::endl;
			OverWriteUtxo(buffer, utxop);

		}
		boff += 76;
		std::cout << "ADD ONE UTXO" << std::endl;

	}
	fclose(f);
	std::cout << "UTXO SUCCESSFULLY UPDATED" << std::endl;
	return;

}

void AddUtxo(unsigned char * nUtxo)
{

	FILE* f = fopen("utxos\\0", "ab");
	// [1] security
	if (f == NULL) { return; } // unvalid file
	fseek(f, 0, SEEK_END);
	// [2] write 
	fwrite(nUtxo, 1, 72, f);
	fclose(f);

}
void OverWriteUtxo ( unsigned char * nUtxo, uint32_t index )
{
	
	// [0] getting utxo set file
	uint32_t mfs = MAX_FILE_SIZE;
	uint32_t filenum = (72 * index) / mfs;
	uint32_t utxonumperfile = mfs / 72; // it is equal to 1 
	uint32_t bOff = (72 * index) - (filenum*utxonumperfile);
	std::ostringstream s;
	s << "utxos\\" << filenum;
	std::string ss = s.str();
	const char * filePath = ss.c_str();
	FILE* f = fopen(filePath, "r+b");
	// [1] security
	if (f == NULL) {
		std::cout << "file Null"; while ( 1){}
		return; } // unvalid file
	// obtain file size:
	fseek(f, 0, SEEK_END);
	uint32_t lSize = ftell(f);
	rewind(f);
	// secure 
	if (lSize < bOff + 72)
	{
		std::cout << "issue " << bOff << std::endl;
		fclose(f);
		return;
	}
	std::cout << "OVERWRITING UTXO AT " << bOff << std::endl;
	// [2] write 
	fseek(f, bOff, SEEK_SET);
	fwrite(nUtxo, 1, 72, f);
	fclose(f);
}


void OverWriteVirtualUtxo(unsigned char * nUtxo)
{
	uint32_t boff = GetVirtualUtxoOffset(nUtxo) * 76;
	std::cout << "OVERWRITING TOU:" << BytesToUint(nUtxo + 64) << " SOLD: " << BytesToUint(nUtxo + 68) <<  " at " << boff <<std::endl;
	OverWriteFile("utxos\\tmp", boff + 4 , nUtxo, 72); // don't overwrite header so offset 4 bytes
}


