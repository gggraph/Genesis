#include "utxo.h"


unsigned char * GetUtxoPuKey(unsigned char * utxo) // return ptr (0) not used ?
{
	return utxo;
}
uint32_t GetUtxoTOU(unsigned char * utxo)
{
	return BytesToUint(utxo + 532);
}
uint32_t GetUtxoSold(unsigned char * utxo)
{
	return BytesToUint(utxo + 536);
}
uint32_t  GetVirtualUtxoOffset(unsigned char * utxo) // virtual set ptr 4 fast overwriting
{
	return BytesToUint(utxo + 540);
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
	unsigned char fdata[532];
	while ( boff < lSize)
	{
		fseek(f, boff, SEEK_SET);
		fread(fdata, 1, 532, f);
		if (memcmp(puKey, fdata, 532) == 0)
		{
			std::cout << "UTXO FOUND" << std::endl;
			return utxop;
		}
		utxop++;
		boff += 540;
	}
	fclose(f);
	std::cout << "NOT FOUND" << std::endl;
	return 0;
}

bool isUtxoNull(unsigned char * utxo)
{
	// yes if pukey is full of 0 
	for (int i = 0; i < 540 ; i++ )
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
	uint32_t filenum = (540 * index) / mfs;
	uint32_t utxonumperfile = mfs / 540; // it is equal to 1 
	uint32_t bOff = (540 * index) - (filenum*utxonumperfile);
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
	if (lSize < bOff + 540)
	{
		fclose(f);
		return;
	}
	// [2] load 
	fseek(f, bOff, SEEK_SET);
	fread(buff, 1, 540, f);
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
		unsigned char fdata[540]; 
		if ( index == 0 )
		{
			fseek(f, boff+4, SEEK_SET);
			fread(fdata, 1, 540, f);
			if (memcmp(puKey, fdata, 532) == 0)
			{
				memcpy(buff, fdata, 540);
				UintToBytes(vIndex, buff + 540); // add virtual set index
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
				fread(buff, 1, 540, f);
				UintToBytes(vIndex, buff + 540); // add virtual set index
				fclose(f);
				return;
			}
		}

		boff += 544;
	 	vIndex++;
	}
	fclose(f);
	std::cout << "RETURNING NULLIFY POINTER";
	//*buff = 0; // setting it to null 
	return;
}

void GetVirtualUtxo(uint32_t utxop, uint32_t blockindextime, unsigned char * rvUtxo, unsigned char * nUtxo)
// Everything start here : 
// add official utxo to virtual set if not already existing (at from specific index time ) and return a virtual utxo [544 bytes]
{
	unsigned char utxo[544]; // NOT WORKIN
	memset(utxo, 0, 540); // always zeroing utxo

	if (utxop != 0)
	{
		// if not new utxo : GetVirtualUtxoInTempFile 
		GetVirtualUtxoInTempFile(utxo, utxop);
		
		if (!isUtxoNull(utxo)) // means if utxo[0]
		{
			// return utxo found in vutxos
			memcpy(rvUtxo, utxo, 544);
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
			// append 540 bytes ( utxo ) 
			fwrite(utxo, 1, 540, f);
			// copy to return utxo
			memcpy(rvUtxo, utxo, 540);
			uint32_t lSize = ftell(f);
			// add utxo count ... seems ok 
			UintToBytes((lSize / 544)-1, rvUtxo + 540); // add extra info bytes (544)
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
		fwrite(nUtxo, 1, 540, f);
		memcpy(rvUtxo, nUtxo, 540); // ok

		uint32_t lSize = ftell(f);
		UintToBytes((lSize / 544) -1, rvUtxo + 540);
		fclose(f);
		return;
	
	}
	 
}

void DowngradeUtxoAtSpecificBlockTime(unsigned char * utxo , uint32_t index)
{

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

	unsigned char buffer[540];

	while (boff < lSize )
	{
		//fseek(f, boff , SEEK_SET);
		fread(buffer, 1, 4, f); // advance curso 4 
		uint32_t utxop = BytesToUint(buffer);
		std::cout << "________________UTXOP F : " << utxop << std::endl;
		if ( utxop == 0 )
		{
			// append
			fread(buffer, 1, 540, f);
			std::cout << "APPENDING TOU:" << BytesToUint(buffer + 532) << " SOLD: " << BytesToUint(buffer + 536) << std::endl;
			AddUtxo(buffer);
			
		}
		else
		{
			// overwrite at 
			fread(buffer, 1, 540, f);
			std::cout << "APPENDING TOU:" << BytesToUint(buffer + 532) << " SOLD: " << BytesToUint(buffer + 536) << std::endl;
			OverWriteUtxo(buffer, utxop);

		}
		boff += 544;
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
	fwrite(nUtxo, 1, 540, f);
	fclose(f);

}
void OverWriteUtxo ( unsigned char * nUtxo, uint32_t index )
{
	
	// [0] getting utxo set file
	uint32_t mfs = MAX_FILE_SIZE;
	uint32_t filenum = (540 * index) / mfs;
	uint32_t utxonumperfile = mfs / 540; // it is equal to 1 
	uint32_t bOff = (540 * index) - (filenum*utxonumperfile);
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
	if (lSize < bOff + 540)
	{
		std::cout << "issue " << bOff << std::endl;
		fclose(f);
		return;
	}
	std::cout << "OVERWRITING UTXO AT " << bOff << std::endl;
	// [2] write 
	fseek(f, bOff, SEEK_SET);
	fwrite(nUtxo, 1, 540, f);
	fclose(f);
}


void OverWriteVirtualUtxo(unsigned char * nUtxo)
{
	uint32_t boff = GetVirtualUtxoOffset(nUtxo) * 544;
	std::cout << "OVERWRITING TOU:" << BytesToUint(nUtxo + 532) << " SOLD: " << BytesToUint(nUtxo + 536) <<  " at " << boff <<std::endl;
	OverWriteFile("utxos\\tmp", boff + 4 , nUtxo, 540); // don't overwrite header so offset 4 bytes
}


