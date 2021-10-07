#include "tx.h"
#include "params.h"
#include "vm.h"

/*
[TX header] ( 85 bytes )
PuKey Pointer   (4 bytes) +0
PrKey Signature (64 bytes) +4
TOU             (4 bytes) +68
Purishment   	(4 byte ) +72
Fee             (4 byte ) (reward for miner) +76
Byte ID         (1 byte ) + 77
Data Size       (4 bytes) +78

[TX data]
[depends]
*/
void PrintTransaction(unsigned char* TX) {
	unsigned char* txdata = GetTXData(TX);
	std::cout << "-_-_-_-_-_-_-_-_-TX info_-_-_-_-_-_-_-" << std::endl;
	std::cout << "UTXOP           : " << BytesToUint(TX) << std::endl;
	printSignature(GetTXSignature(TX));
	std::cout << "TOU             : " << GetTXTokenOfUniqueness(TX) << std::endl;
	std::cout << "Purishment time : " << GetTXPurishmentDate(TX) << std::endl;
	std::cout << "Fee			  : " << GetTXFee(TX) << std::endl;
	std::cout << "Data Size		  : " << GetTXDataSize(TX) << std::endl;
	switch (GetTXByteID(TX)) {
	case 0 : 
		std::cout << "IS DEFAULT TRANSACTION TYPE" << std::endl;
		
		std::cout << "INFOREC  		  : " << (int) txdata[0]  << std::endl;
		std::cout << "Amount  		  : " << BytesToUint(txdata + 1) << std::endl;
		break;
	case 1: 
		break;
	case 2: break;

	}
	std::cout << "-_-_-_-_-_-_-_-_--_-_-_-_-_-_-_-_-_-" << std::endl;

}
void printSignature(unsigned char* buff)
{
	for (int i = 0; i < 64; i++) {
		std::cout << ("0123456789abcdef"[buff[i] >> 4]);
		std::cout << ("0123456789abcdef"[buff[i] & 0xf]);
	}
	std::cout << std::endl;

}

bool IsTransactionValid(unsigned char * TX, uint32_t blockindextime, int * gas, unsigned char * block, uint32_t txindex, const char * filePath)
{
	
	if ( !VerifyHeaderTransaction(TX, blockindextime) )
		return false;

	switch (GetTXByteID(TX))
	{
		case 0:  return VerifyDFT(TX, blockindextime, gas);
		case 1:  return VerifyCST(TX, blockindextime, gas, block, txindex);
		case 2:  return VerifyCRT(TX, blockindextime, gas, block, txindex, filePath);
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
		// check if new receiver key is valid secp256k1 ... 
		if (!uECC_valid_public_key(TXDATA + 5, uECC_secp256k1()))
		{

			std::cout << "[WRONG TX] Invalid Receiver Public Key for SECP256K1 standard." << std::endl;
			return false;

		}
		unsigned char nutxo[72];
		memcpy(nutxo, TXDATA + 5, 64); // set pukey for new utxo 
		memset(nutxo + 64, 0, 8); // zeroing TOU & sold 
		GetVirtualUtxo(0, blockindextime, rutxo, nutxo);
		MIN_FEE += 50;
	}
	
	uint32_t txfee = GetTXFee(TX);
	uint32_t totCost = BytesToUint(TXDATA + 1) + txfee;
	
	if (txfee < MIN_FEE) {
		std::cout << "[BLOCK REFUSED] Insuffisant fee in a transaction";  
		return false;
	}
		

	if (totCost > GetUtxoSold(sutxo)) {
		std::cout << "[BLOCK REFUSED] Insuffisant sold in a transaction : " << totCost << " but " << GetUtxoSold(sutxo); 
		return false;
	}

	// update sender sold and tou. update receiver sold.
	
	UintToBytes(GetTXTokenOfUniqueness(TX), sutxo + 64);
	UintToBytes(GetUtxoSold(sutxo) - totCost, sutxo + 68);
	UintToBytes(GetUtxoSold(rutxo) + BytesToUint(TXDATA + 1), rutxo + 68);
	OverWriteVirtualUtxo(sutxo);
	OverWriteVirtualUtxo(rutxo);
	*gas += 60;
	return true;

}

bool FastApplyEXCH(uint32_t SUTXOP, uint32_t RUTXOP, uint32_t amount, int blockindextime, bool _rev) 
{
	unsigned char  sutxo[76];
	unsigned char  rutxo[76];
	GetVirtualUtxo(SUTXOP, blockindextime, sutxo);
	GetVirtualUtxo(RUTXOP, blockindextime, rutxo);
	if (amount > GetUtxoSold(sutxo)) {
		std::cout << "EXCH CANNOT BE DONE. INSUFFISANT SOLD." << GetUtxoSold(sutxo);
		return false;
	}
	// update sender sold and tou. update receiver sold.

	UintToBytes(GetUtxoSold(sutxo) - amount, sutxo + 68);
	UintToBytes(GetUtxoSold(rutxo) + amount, rutxo + 68);
	OverWriteVirtualUtxo(sutxo);
	OverWriteVirtualUtxo(rutxo);
	return true;
}

bool VerifyCST(unsigned char * TX, uint32_t blockindextime, int * gas, unsigned char * block, uint32_t txindex)
{	/*
		entries length (4 bytes)
		entries        (entries length * 4 )
		smart contract code (the "real entry")
	*/
	// [0] Verify Sender has suffisant sold to pay the fee for the contract submission 
	unsigned char  sutxo[76];
	GetVirtualUtxo(BytesToUint(TX), blockindextime, sutxo);
	uint32_t txfee = GetTXFee(TX);
	if (GetUtxoSold(sutxo) < txfee) 
	{
		std::cout << "[BLOCK REFUSED] Insuffisant sold in a transaction";  
		return false;
	}
	// [?] Verify CST contract size is not exceeding max contract size ( currently 16KO ) 
	uint32_t dtsize = GetTXDataSize(TX);
	/*
	if ( dtsize > MAX_CONTRACT_SIZE)
		return false;
	*/
	*gas += 30 + (dtsize)/4;

	// create the storage 
	char str[255];
	unsigned char buffer[8];
	uint32_t bIndex = GetBlockIndex(block);
	memcpy(buffer, &bIndex, 4); // bloc index is offset 0 
	memcpy(buffer + 4, &txindex, 4);
	Sha256.init();
	Sha256.write((char*)buffer, 8);
	GetHashString(Sha256.result(), str);
	std::ostringstream s;
	s << "sc\\" << str;
	FILE* f = fopen(s.str().c_str(), "ab");
	fclose(f);
	// [?] Run Contract first code 
	int gused = RunCST(TX, dtsize + 85, block, blockindextime, txindex, *gas, true, false);

	// [?] If insuffisant gas return false
	if (!gused) {
		std::cout << "[BLOCK REFUSED] VM return error. ";
		
		remove(s.str().c_str());

		return false;
	}
	// [?] Update gas
	*gas += gused;
	// UPDATE SOLD AND TOU
	UintToBytes(GetTXTokenOfUniqueness(TX), sutxo + 64);
	UintToBytes(GetUtxoSold(sutxo) - txfee, sutxo + 68);
	OverWriteVirtualUtxo(sutxo);
	return true;
}

bool VerifyCRT(unsigned char* TX, uint32_t blockindextime, int* gas, unsigned char* block, uint32_t txindex, const char * filePath)
{
	/*
	Smart Contract pointer    (8 bytes)
		[*] Push  Operations  (4 bytes) -> repeated a specific amount of time determined by validator with data size
	Smart Contract Entry Jump (4 bytes)
	User Gas Limit            (4 bytes)
	*/
	// 

	unsigned char* txdata = GetTXData(TX);
	uint32_t cbi = BytesToUint(txdata);
	uint32_t cti = BytesToUint(txdata + 4);
	unsigned char* contractBlock = NULL;
	
	if (cbi > blockindextime) {
		contractBlock = GetUnofficialBlock(filePath, cbi);
	}
	else {
		contractBlock = GetOfficialBlock(cbi);
	}
	if (contractBlock == NULL) {
		std::cout << "[TX INVALID] Contract Block not exists.";
		return 0;
	}
	if (GetTransactionNumber(contractBlock) + 1 < cti) // i dont know if i need the +1 :: i should verify 
	{
		std::cout << "[TX INVALID] Contract Block Transaction not exists.";
		return 0;
	}
	unsigned char* tContract = GetBlockTransaction(contractBlock, cti);

	// verify the sold
	unsigned char  sutxo[76];
	GetVirtualUtxo(BytesToUint(TX), blockindextime, sutxo);
	uint32_t txfee = GetTXFee(TX);
	if (GetUtxoSold(sutxo) < txfee)
	{
		std::cout << "[BLOCK REFUSED] Insuffisant sold in a transaction";
		return false;
	}

	// [?] Verify CST contract size is not exceeding max contract size ( currently 16KO ) 
	uint32_t dtsize = GetTXDataSize(TX);
	*gas += 30 + (dtsize) / 4;

	// [?] Run Contract first code 
	int gused = RunCRT(tContract, GetTXDataSize(tContract) + 85 , TX, GetTXDataSize(TX) + 85 , contractBlock, block, blockindextime, cti, *gas, true, false);
	
	free(contractBlock);

	if (!gused) {
		std::cout << "[BLOCK REFUSED] VM return error. ";
		
		return false;
	}
	// [?] Update gas
	*gas += gused;

	// UPDATE SOLD AND TOU
	UintToBytes(GetTXTokenOfUniqueness(TX), sutxo + 64);
	UintToBytes(GetUtxoSold(sutxo) - txfee, sutxo + 68);
	OverWriteVirtualUtxo(sutxo);

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



// this algo will sort every TRANSACTION IN PTX file BY THE FOLLOWING FORMAT : 
/*
    - tx blocks per average fee volume in descending order 
	[ SORT BY PUKEY ID                 ]
	[ >SORT BY TOU                     ]
	[ >>COMPUTE ACTUALLY UTXO FROM SOLD]

	HOW TO ?
	SORT TX BY PUKEY ID IN A SPLIT FILE CONTAINING IN FIRST 4 BYTES THE TOTAL AMOUNT OF FEES.
	VERIFY EACH TX SPLIT FILES. (CREATING A CUSTOM UTXO INSTANCE)
	ADD THEM BY TOTAL FEE ORDER
	
*/

// UNUSED
void RecraftPTX(uint32_t _purishOffset) {

	// [0] load ptx file in memory (use malloc)
	FILE* f = fopen("ptx", "rb");
	if (f == NULL) { std::cout << "error reading..." << std::endl; return; }
	fseek(f, 0, SEEK_END);
	uint32_t fsize = ftell(f);
	rewind(f);

	// [1] Create a copy.
	unsigned char* tmpdat = (unsigned char*)malloc(fsize);
	fread(tmpdat, 1, fsize, f);
	fclose(f);

	uint32_t boff = 0;
	unsigned char utxobuffer[72];
	while (boff < fsize) {
		// sorting them by TOU
		if (GetTimeStamp() + _purishOffset < GetTXPurishmentDate(tmpdat + boff)) 
		{
			// now resolve the puzzle. TX has to be in ascending order.
			// so get all ID sorting by ptr in file ... if new id create new id entry .  
		}
		boff += GetTXDataSize(tmpdat + boff) + 85;
		// /!\ every signature has been already verified. 

		/*
		bool _isValid = true;

		// [ 0 ]  verify purishment time 
		if (GetTimeStamp() + _purishOffset < GetTXPurishmentDate(tmpdat + boff))
			_isValid = false;
		// [ 1 ] verify TOU based on official blockchain only. We will need to write a TOKENPUZZLESOLVERFUNCTION if needed 
		GetUtxo(BytesToUint(tmpdat + boff), utxobuffer);
		if (GetUtxoTOU(utxobuffer) != GetTXTokenOfUniqueness(tmpdat + boff) - 1)
			_isValid = false;
		*/
	}
}

bool AddTransactionToPTXFile(unsigned char* TX, int TXSIZE)
{
	// Verify signature. 
	if (!isSignatureValid(TX)) {
		return false;
	}
	//Verify Data Size
	if (TXSIZE != GetTXDataSize(TX) + 85) {
		return false;
	}
	// append brutaly?
	FILE* f = fopen("ptx", "ab");
	if (f == NULL) return false;
	rewind(f);
	fwrite(TX, 1, TXSIZE, f);
	fclose(f);
	std::cout << "PTX APPENDED" << std::endl;


}

// Long proccess that verify each tx.
// UNUSED
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

	if (AddTransactionToPTXFile(signed_data, msg_size + 64)) {
		std::cout << "New transaction has been created. " << std::endl;
	}
	

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

void GetRandomValidPublicKey(unsigned char* buffer) 
{
	uint8_t pukey[64];
	uint8_t prkey[32];
	uECC_make_key(pukey, prkey, uECC_secp256k1());
	memcpy(buffer, pukey, 64);
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