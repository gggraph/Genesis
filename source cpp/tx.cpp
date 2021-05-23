#include "tx.h"


/*
[TX header]
PuKey Pointer   (4 bytes)
PrKey Signature (512 bytes)
TOU             (4 bytes)
Purishment   	(4 byte )
Fee             (4 byte ) (corresponding to all cost + additional reward for miner)
Byte ID         (1 byte )
[TX data]
Data Size       (4 bytes)
Data            (Data Size)
*/

bool IsTransactionValid(unsigned char * TX, uint32_t blockindextime)
{
	if ( !VerifyHeaderTransaction )
		return false;

	switch (GetTXByteID(TX))
	{
		case 0: return VerifyDFT(TX, blockindextime);
		case 1: return VerifyCST(TX, blockindextime);
		case 2: return VerifyCRT(TX, blockindextime);
	}

	return false; 
}

bool VerifyHeaderTransaction(unsigned char * TX)
{
	//[0] Verify RSA Signature ( type of sign should depend on byteflag )
	//[1] Verify Token Of Uniqueness
	//[2] Verify Purishment Time
	return true;
}

bool VerifyDFT(unsigned char * TX, uint32_t blockindextime)
{
	/*
		DFT Structure : 
		byte flag (1o) indicates if receiver exists in bc or not 
		amount                  (4o)
		receiver utxop or puKey (4o or 532 o ) 
	
	*/
	//[0] Append/Get sUTXO and rUTXO 
	unsigned char  sutxo[544];
	unsigned char  rutxo[544];
	GetVirtualUtxo(BytesToUint(TX), blockindextime, sutxo);

	uint32_t MIN_FEE = 0; // native fee for proccess DFT
	unsigned char * TXDATA = GetTXData(TX); // just a ptr
	if (*TXDATA == 0)
	{
		GetVirtualUtxo(BytesToUint(TXDATA + 5), blockindextime, rutxo);
	}
	else
	{
		unsigned char nutxo[540];
		memcpy(nutxo, TXDATA + 5, 532);
		memset(TXDATA + 537, 0, 8);
		GetVirtualUtxo(0, blockindextime, rutxo, nutxo);
		MIN_FEE += 50; // + penality for writing new account
	}

	uint32_t txfee = GetTXFee(TX);
	uint32_t totCost = BytesToUint(TXDATA + 1) + txfee;

	if (txfee < MIN_FEE)
		std::cout << "[BLOCK REFUSED] Insuffisant fee in a transaction";  return false;

	if ( totCost  < GetUtxoSold(sutxo))
		std::cout << "[BLOCK REFUSED] Insuffisant sold in a transaction";  return false;

	// update sender sold and tou. update receiver sold.  
	UintToBytes(GetTXTokenOfUniqueness(TX), sutxo + 532);
	UintToBytes(GetUtxoSold(sutxo) - totCost, sutxo + 536);
	UintToBytes(GetUtxoSold(rutxo) + BytesToUint(TXDATA + 1), rutxo + 536);
	OverWriteVirtualUtxo(sutxo);
	OverWriteVirtualUtxo(rutxo);
	// return
	return true;

}

bool VerifyCST(unsigned char * TX, uint32_t blockindextime)
{
	return true;
}

bool VerifyCRT(unsigned char * TX, uint32_t blockindextime)
{
	return true;
}

bool isSignatureValid(unsigned char * TX)
{
	// Verify Signature ( hash the TX without the signature then verify . 
	/*Well, the RSA operation can't handle messages longer than the modulus size. That means that if you have a 2048 bit RSA key, 
	you would be unable to directly sign any messages longer than 256 bytes long (and even that would have problems, because of lack of padding).
	*/
	// sign = s=(h^d)*(mod n)
	// if correct : (this is true : )
	// h'=s^e and mod n = (h^d)^e and mod n = h
	return true;
}

void RefreshPTXFileFromVirtualUtxoSet()
{
	// [0] load ptx file in memory (use malloc)
	FILE* f = fopen("ptx", "rb");
	if (f == NULL) { std::cout << "error reading..." << std::endl; return; }
	fseek(f, 0, SEEK_END);
	uint32_t fsize = ftell(f);
	rewind(f);

	unsigned char * tmpdat = (unsigned char *)malloc(fsize);
	fread(tmpdat, 1, fsize, f);
	fclose(f);

	// [0] create copy
	f = fopen("nptx", "ab");
	if (f == NULL) { std::cout << "error reading..." << std::endl; return; }
	rewind(f);
	// [1] iterating through all ptx in file
	uint32_t boff = 0; 
	unsigned char utxo[544];
	memset(utxo, 0, 544);
	while ( boff < fsize)
	{
		// don't append if ( TOU is LOWER than TOU get from UNOFFICIAL UTXO SET ), don't append if Purishing Time ...
		uint32_t sutxop = BytesToUint(tmpdat + boff);
		GetVirtualUtxoInTempFile(utxo, sutxop);
		if (isUtxoNull(utxo)) {
			std::cout << "CANNOT REFRESH PTX" << std::endl;
			return;
		}
		uint32_t ctou = GetUtxoTOU(utxo);
		uint32_t ptou = GetTXTokenOfUniqueness(tmpdat + boff);
		
		uint32_t cts = GetTimeStamp();
		uint32_t pts = GetTXPurishmentDate(tmpdat + boff);

		// GET THE WHOLE TX LENGTH 

		
		uint32_t TXsize = 533 + GetTXDataSize(tmpdat + boff);
		if ( ctou < ptou && pts > cts )
		{
			// append 
			fwrite(tmpdat + boff, 1, TXsize, f);
		}
		boff += TXsize;
	
	}
	free(tmpdat);
	// rename nptx to ptx. del ptx.
	remove("ptx");
	rename("nptx", "ptx");
	std::cout << "PTX file Refreshed with success " << std::endl;

}