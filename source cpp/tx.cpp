#include "tx.h"
#include "params.h"
#include "vm.h"

/*
[TX header] ( 85 bytes )
PuKey Pointer   (4 bytes)
PrKey Signature (64 bytes)
TOU             (4 bytes)
Purishment   	(4 byte )
Fee             (4 byte ) (reward for miner)
Byte ID         (1 byte )
Data Size       (4 bytes)
[TX data]
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
	//[0] Verify SECP256K1 Signature ( type of sign should depend on byteflag )
	if (!isSignatureValid(TX)) {
		return false;
	}
	//[1] Verify Token Of Uniqueness
	unsigned char  sutxo[76];
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
		receiver utxop or puKey (4o or 64 o ) 
	
	*/
	//[0] Append/Get sUTXO and rUTXO 
	unsigned char  sutxo[76];
	unsigned char  rutxo[76];
	GetVirtualUtxo(BytesToUint(TX), blockindextime, sutxo);
	
	uint32_t MIN_FEE = 0; // native fee for proccess DFT. 
	unsigned char * TXDATA = GetTXData(TX); // just a ptr
	if (*TXDATA == 0)
	{
		GetVirtualUtxo(BytesToUint(TXDATA + 5), blockindextime, rutxo);
	}
	else
	{
		unsigned char nutxo[72];
		memcpy(nutxo, TXDATA + 5, 64);
		memset(TXDATA + 69, 0, 8);
		GetVirtualUtxo(0, blockindextime, rutxo, nutxo);
		MIN_FEE += 50; // + penality for writing new account
	}
	
	uint32_t txfee = GetTXFee(TX);
	uint32_t totCost = BytesToUint(TXDATA + 1) + txfee;
	
	if (txfee < MIN_FEE) {
		std::cout << "[BLOCK REFUSED] Insuffisant fee in a transaction";  return false;
	}
		

	if (totCost > GetUtxoSold(sutxo)) {
		std::cout << "[BLOCK REFUSED] Insuffisant sold in a transaction";  return false;
	}

	// update sender sold and tou. update receiver sold.
	
	UintToBytes(GetTXTokenOfUniqueness(TX), sutxo + 64);
	UintToBytes(GetUtxoSold(sutxo) - totCost, sutxo + 68);
	UintToBytes(GetUtxoSold(rutxo) + BytesToUint(TXDATA + 1), rutxo + 68);
	OverWriteVirtualUtxo(sutxo);
	OverWriteVirtualUtxo(rutxo);
	
	return true;

}

bool VerifyCST(unsigned char * TX, uint32_t blockindextime, int * gas, uint32_t blocindex, uint32_t txindex)
{	
	// [0] Verify Sender has suffisant sold to pay the fee
	unsigned char  sutxo[76];
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
	unsigned char puKey[64];
	
	GetTXsPuKey(puKey, TX);
	
	// verify if puKey is Valid SECP
	if (!uECC_valid_public_key(puKey, uECC_secp256k1())) {
		std::cout << "[WRONG TX] Invalid Public Key for SECP256K1 standard." << std::endl;
		return false;
	}

	
	// craft TX HASH
	int ds = GetTXDataSize(TX);
	unsigned char * data = (unsigned char*)malloc(21+ds);
	memcpy(data, TX, 4);
	std::cout << 68 + 17 + ds << std::endl;
	memcpy(data + 4, TX + 68, 17 + ds);
	Sha256.init();
	Sha256.write((char*)data, 21 + ds); // hash transaction 
	unsigned char hash[32];
	memcpy(hash, Sha256.result(), 32);
	free(data); // release heap alloc
	//verify 
	if (uECC_verify(puKey, hash, 32, GetTXSignature(TX), uECC_secp256k1()) == 1) {
		return true;
	}
	else {
		std::cout << "[WRONG TX] Invalid Signature." << std::endl;
		return false;
	}
	
}

//WILL BE REMOVED
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
	unsigned char utxo[76];
	memset(utxo, 0, 76);
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

		uint32_t TXsize = 65 + GetTXDataSize(tmpdat + boff);
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
                                            // ______________________________ TRANSACTION CREATOR __________________________
/*
[TX header] ( 85 bytes )
PuKey Pointer   (4 bytes)
PrKey Signature (64 bytes)
TOU             (4 bytes)
Purishment   	(4 byte )
Fee             (4 byte ) (reward for miner)
Byte ID         (1 byte )
Data Size       (4 bytes)
[TX data]
Data            (Data Size)


DFT Structure :
		byte flag (1o) indicates if receiver exists in bc or not
		amount                  (4o)
		receiver utxop or puKey (4o or 64 o )
*/

bool AddTransactionToPTXFile( unsigned char * TX ) 
{
	// Verify signature. Verify Data Size && add it ... 
	return false;
}

// this algo will sort every TRANSACTION IN PTX file BY THE FOLLOWING FORMAT : 
/*
    - tx blocks per average fee volume in descending order 
	[ SORT BY PUKEY ID                 ]
	[ >SORT BY TOU                     ]
	[ >>COMPUTE ACTUALLY UTXO FROM SOLD]
	
*/


// Long proccess that verify each 
void RefreshPendingTransactionFile(int _purishOffset = 0) {

	// verify each header TOU, Purishment,  ... etc.
	// 
	// [0] load ptx file in memory (use malloc)
	FILE* f = fopen("ptx", "rb");
	if (f == NULL) { std::cout << "error reading..." << std::endl; return; }
	fseek(f, 0, SEEK_END);
	uint32_t fsize = ftell(f);
	rewind(f);

	unsigned char* tmpdat = (unsigned char*)malloc(fsize);
	fread(tmpdat, 1, fsize, f);
	fclose(f);
	// [0] create a copy
	f = fopen("nptx", "ab");
	if (f == NULL) { std::cout << "error reading..." << std::endl; return; }
	rewind(f);
	uint32_t boff = 0;
	unsigned char utxobuffer[72];
	while (boff < fsize) {
		// /!\ every signature has been already verified. 
		
		bool _isValid = true;

		// [ 0 ]  verify purishment time 
		if (GetTimeStamp() + _purishOffset < GetTXPurishmentDate(tmpdat + boff))
			_isValid = false;
		// [ 1 ] verify TOU based on official blockchain only. We will need to write a TOKENPUZZLESOLVERFUNCTION if needed 
		GetUtxo(BytesToUint(tmpdat + boff), utxobuffer);
		if (GetUtxoTOU(utxobuffer) != GetTXTokenOfUniqueness(tmpdat + boff) - 1)
			_isValid = false;
	}

}

bool CreateDefaultTransaction(unsigned char * prKey, uint32_t utxop,  uint32_t nTOU, uint32_t PurishmentTime, uint32_t fee, 
							 uint32_t  amount, uint32_t rutxop, unsigned char * rpuKey) {

	
	// [0] build unsign data
	int msg_size = 30;
	if (rutxop == 0) 
		msg_size += 60;

	PurishmentTime += GetTimeStamp(); // apply timestamp to pushment time 
	unsigned char* unsigned_data = (unsigned char*)malloc(msg_size); // do some alloc 
	
	memcpy(unsigned_data, &utxop, 4);
	memcpy(unsigned_data+4, &nTOU, 4);
	memcpy(unsigned_data+8, &PurishmentTime, 4);
	memcpy(unsigned_data + 12, &fee, 4);
	unsigned_data[16] = 0; // flag byte for DFT
	// setting datasize depending of receiver existence in db,  byteflag and co ..
	if (rutxop == 0) {
		UintToBytes(69, unsigned_data + 17);
		unsigned_data[21] = 1; // receiver not existing
		memcpy(unsigned_data + 22, &amount, 4);
		memcpy(unsigned_data + 26, rpuKey, 64);
	}
	else 
	{
		UintToBytes(9, unsigned_data + 17);
		unsigned_data[21] = 0; // receiver exist
		memcpy(unsigned_data + 22, &amount, 4);
		memcpy(unsigned_data + 26, &rutxop, 4);
	}
	// [1] hash 
	Sha256.init();
	Sha256.write((char*)unsigned_data, msg_size); // hash transaction 
	unsigned char hash[32];
	memcpy(hash, Sha256.result(), 32);
	
	// [1] sign 
	uint8_t sign[64];
	uECC_sign(prKey, hash, 32, sign, uECC_secp256k1());
	
	// craft the tx data file
	unsigned char* signed_data = (unsigned char*)malloc(msg_size + 64);
	memcpy(signed_data, unsigned_data, 4);
	memcpy(signed_data + 4 , sign, 64);
	memcpy(signed_data + 68 , unsigned_data + 4, msg_size - 4); // copy the rest
	free(unsigned_data); // release heap alloc

	if (!isSignatureValid(signed_data))
		return false;

	AddTransactionToPTXFile(signed_data, msg_size + 64);
	

	return true;
	// 
	// go further if you want a backup file
	// 
	// now build file at roots
	FILE* f = fopen("newtransaction", "ab");
	if (f == NULL) return false;
	rewind(f);
	fwrite(signed_data, 1, msg_size + 64, f);
	fclose(f);
	
	free(signed_data); // release heap alloc
	return true;
}

bool MakeSECP256K1PairKeys() {

	uint8_t pukey[64];
	uint8_t prkey[32];
	uECC_make_key(pukey, prkey, uECC_secp256k1());
	if (!uECC_valid_public_key(pukey, uECC_secp256k1())) {
		std::cout << "Something happened wrong" << std::endl;
		return false;
	}
	// check if key files already exists ... 
	if (FileExists("puk") || FileExists("prk")) {
		std::cout << "Pair Keys file has been already found. aborted." << std::endl;
		return false;
	}
	// now put them in root directory
	FILE* f = fopen("puk", "ab");
	if (f == NULL) return false;
	rewind(f);
	fwrite(pukey, 1, 64, f);
	fclose(f);
	f = fopen("prk", "ab");
	if (f == NULL) return false;
	rewind(f);
	fwrite(prkey, 1, 32, f);
	fclose(f);
	std::cout << "[SUCCESS] SECP256K1 Public and private keys are located in root folder as prk and puk files." << std::endl;

	return true;
}