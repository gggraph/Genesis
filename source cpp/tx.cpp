#include "tx.h"
#include "params.h"
#include "vm.h"

/*
[TX header]
PuKey Pointer   (4 bytes)
PrKey Signature (512 bytes)
TOU             (4 bytes)
Purishment   	(4 byte )
Fee             (4 byte ) (reward for miner)
Byte ID         (1 byte )
[TX data]
Data Size       (4 bytes)
Data            (Data Size)
*/

bool IsTransactionValid(unsigned char * TX, uint32_t blockindextime, int * gas, uint32_t blocindex, uint32_t txindex)
{
	
	if ( !VerifyHeaderTransaction(TX, blockindextime) )
		return false;

	switch (GetTXByteID(TX))
	{
		case 0:  return VerifyDFT(TX, blockindextime, gas);
		case 1:  return VerifyCST(TX, blockindextime, gas, blocindex, txindex);
		case 2:  return VerifyCRT(TX, blockindextime, gas);
	}

	return false; 
}

bool VerifyHeaderTransaction(unsigned char * TX, uint32_t blockindextime)
{
	//[0] Verify RSA Signature ( type of sign should depend on byteflag )
	//....
	//[1] Verify Token Of Uniqueness
	unsigned char  sutxo[544];
	GetVirtualUtxo(BytesToUint(TX), blockindextime, sutxo);
	if (GetTXTokenOfUniqueness(TX) != GetUtxoTOU(sutxo) + 1) 
	{
		std::cout << "Wrong TOU" << std::endl;
		return false;
	}
	//[2] Verify Purishment Time
	if ( GetTXPurishmentDate(TX) < GetTimeStamp())
	{
		std::cout << "TX has purished" << std::endl;
		return false;
	}
	return true;
}

bool VerifyDFT(unsigned char * TX, uint32_t blockindextime, int * gas)
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

	uint32_t MIN_FEE = 0; // native fee for proccess DFT. 
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

	if ( totCost > GetUtxoSold(sutxo))
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

bool VerifyCST(unsigned char * TX, uint32_t blockindextime, int * gas, uint32_t blocindex, uint32_t txindex)
{	
	// [0] Verify Sender has suffisant sold to pay the fee
	unsigned char  sutxo[544];
	GetVirtualUtxo(BytesToUint(TX), blockindextime, sutxo);
	uint32_t txfee = GetTXFee(TX);
	if (GetUtxoSold(sutxo) < txfee) 
	{
		std::cout << "[BLOCK REFUSED] Insuffisant sold in a transaction";  
		return false;
	}
	// [1] Verify CST contract size is not exceeding gas limit ( we write 1 gas per byte write ) we can adjust this later
	uint32_t dtsize = GetTXDataSize(TX);
	if (*gas + dtsize * 1 > MAX_GAS_SIZE)
		return false;

	// [2] Verify CST contract is not over 16ko. 
	if ( dtsize > MAX_CONTRACT_SIZE)
		return false;

	// [3] Create Contract Storage ( i need block index and tx index ) 
	char str[255];
	unsigned char buffer[8];
	memcpy(buffer, &blocindex, 4); // bloc index is offset 0 
	memcpy(buffer + 4, &txindex, 4);
	Sha256.init();
	Sha256.write((char*)buffer, 8);
	GetHashString(Sha256.result(), str);
	std::ostringstream s;
	s << "sc\\tmp\\" << str; // it is temporary so i need to put it in the sc\tmp folder 
	std::string ss = s.str();
	FILE* f = fopen(ss.c_str(), "wb");
	if (f == NULL) return false; // contract was not created.
	fclose(f);

	// [4] Update gas
	*gas += dtsize * 1;

	// [3b] Run Contract ?
	
	return true;
}

bool VerifyCRT(unsigned char * TX, uint32_t blockindextime, int * gas)
{
	/*
	Smart Contract pointer    (8 bytes)
		[*] Push  Operations  (4 bytes) -> repeated a specific amount of time determined by validator with data size
	Smart Contract Entry Jump (4 bytes)
	User Gas Limit            (4 bytes)
	*/
	// [0] Verify contract exist

	unsigned char * txdata = GetTXData(TX);
	uint32_t cbi = BytesToUint(txdata);
	uint32_t cti = BytesToUint(txdata + 4);
	// first check if we can get this block.
	unsigned char * contractbloc = NULL;
	if (cbi > blockindextime)
		contractbloc = GetUnofficialBlock("", cbi); // aie i need a file path ;'(
	else
		contractbloc = GetOfficialBlock(cbi);
	if (contractbloc == NULL)
		return false;
	if (GetTransactionNumber(contractbloc) + 1 < cti) // i dont know if i need the +1 :: i should verify 
		return false;
	
	unsigned char * contractTransaction = GetBlockTransaction(contractbloc,cti);
	if (contractTransaction == NULL)
		return false;

	// [1] Verify entry exist
	uint32_t argnum = (GetTXDataSize(TX) - 12) / 4; // every push are 32bit value. thats all .
	uint32_t entrynum = BytesToUint(txdata + (argnum * 4) + 8); // get the entry in the smartcontract data in the bloc
	int _guserlimit = BytesToUint(txdata + (argnum * 4) + 12); 
	unsigned char * contractdata = GetTXData(contractTransaction);
	if (BytesToUint(contractdata) < entrynum) // bad entry
		return false;
	uint32_t entrymem = BytesToUint(contractdata + 4 + (entrynum * 4)); // entry mem is update for 0x5d in assembler i guess

	// [2] Init Virtual Machine ('-')

	InitVM(contractdata + 4 + (BytesToUint(contractdata)*4), 
		GetTXDataSize(contractdata)  - (4 + (BytesToUint(contractdata) * 4)), 
		entrymem);

	// [3] Push arguments to VM Stack
	for (int i = 0; i < argnum; i++) 
		PushArgument(BytesToUint(txdata + (i * 4) + 8));

	// [4] Run

	int rs = RunVM(*gas, _guserlimit);
	if (!rs)
		return false;

	// [5] Update Gas
	*gas += rs;

	free(contractbloc);
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