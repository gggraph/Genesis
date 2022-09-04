#include "Utils.h"
#include "Special.h"
#include <mciapi.h>
#include <thread> 
#include "3d.h"
//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")

const char* storagePath = "C:\\Users\\gaelg\\source\\repos\\Genesis\\Genesis\\sc\\74b4dda3624aed85d808e91d84b08aad88563b02fe290e0d327865c33d2bafbd";
const char* outputDir = "C:\\Users\\gaelg\\source\\repos\\Genesis\\Genesis\\AUTOCRT\\";


struct WALLET
{
	uint32_t      ptr;
	unsigned char puKey[64];
	uint32_t      utxop;
	uint32_t      items[100];

};
struct ART
{
	uint32_t      ptr;
	unsigned char _marketmode;
	uint32_t      price;
	uint32_t      _creatorWalletPtr;
	unsigned char sign[64];
	uint32_t      unixTimeStamp;
	uint32_t      _lastpriceUpdate;
	uint32_t      type;
	uint32_t      dataSize;

};


std::vector<WALLET> wallets;
std::vector<ART>    arts;
unsigned char myPUK[64];
uint32_t myUTXO = 0;
unsigned char myPRK[64];
bool _reg = false;
unsigned char placeHolder_BIN[289839];

uint32_t BytesToUint(unsigned char* arr); 
void UintToBytes(uint32_t v, unsigned char* a); 
void PrintRawBytes(unsigned char* ptr, int length);
uint32_t nearestmultiple(uint32_t numToRound, uint32_t multiple, bool flr);

void REG_ME_CRT(int utxo);
void REG_ART_CRT(char* fpath, int walletptr);
void UNLOCK_CRT(int walletptr, int itemptr);
void LOCK_CRT(int walletptr, int itemptr);
void UPDATE_PRICE_CRT(int walletptr, int itemptr);
void SAVAGE_BUY_CRT();
void SET_CRT_HEADER(unsigned char* CRTDATA, int TXSIZE, int CRTSIZE);


std::wstring s2ws(const std::string& s);
void ReadStorage();
void PrintKey(unsigned char* buff);
void ShowItems(unsigned char sortMode, bool _ascending, bool onlySavage, int selpos, int selline, int onlyType = -1, bool _enterPressed = false);
void GetArtData(const ART& a, unsigned char* buffer);
bool compareArtByPrice_A(const ART& a, const ART& b);
bool compareArtByPrice_D(const ART& a, const ART& b);
bool compareArtByTimeStamp_A(const ART& a, const ART& b);
bool compareArtByTimeStamp_D(const ART& a, const ART& b);
bool compareArtByPtr_A(const ART& a, const ART& b);
bool compareArtByPtr_D(const ART& a, const ART& b);
void DisplayArtInfo(const ART& A);
void GetKeyString(unsigned char* buff, char* stringbuffer);
bool GetSDLInput(std::string msg, char* result, unsigned char entryMode, bool _acceptDrop = false);
int GetWalletIndexfromPuk(unsigned char* PUK);
void RegisterMe();
bool DoesWalletContainsItem(int wallptr, int itemptr);
int GetArtIndexFromPtr(int ptr);
int GetWalletPossessingArt(int iptr);
void SetBit(int p, bool b, unsigned char* byte);
bool IsBitSet(int p, unsigned char byte);
void ShowMenu();
int GetWalletIndexFromPtr(int ptr);
int GetFileType(char* fpath);
void GetTypeString(int type, char* buffer);
void LoadPlaceHolders();
void GetPathInfo();
void GetRandomHashString(char * buff);

void Special_BIM();
void AUTOREG_ART_CRT(unsigned char* data, int size, int walletptr, int nonce);

SDL_Event e;
#define LOG(format, ...) wprintf(format L"n", __VA_ARGS__)
int main(int argv, char** args)
{

	GetPathInfo();
	InitSDL();
	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}

	ReadStorage();
	LoadPlaceHolders();
	ShowMenu();
	
	while(1){}
}
unsigned char kState[6];
// up right down left enter escape

void LoadPlaceHolders() 
{
	FILE* f = fopen("placeHolder_BIN.jpg", "rb");
	if (f == NULL) {
		return;
	}
	fread(placeHolder_BIN, 1, 289839, f);
	fclose(f);
}
void GetInputStates()
{
	SDL_PollEvent(&e);
	// select stuff
	const Uint8* state = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));
	// bit 1 state 
	// bit 2 was changed this 
	for (int i = 0; i < 6; i++) {
		uint8_t* st = NULL;
		switch (i) {
		case 0:
			st = (uint8_t*)state[SDL_SCANCODE_UP];
			break;
		case 1:
			st = (uint8_t*)state[SDL_SCANCODE_RIGHT];
			break;
		case 2:
			st = (uint8_t*)state[SDL_SCANCODE_DOWN];
			break;
		case 3:
			st = (uint8_t*)state[SDL_SCANCODE_LEFT];
			break;
		case 4:
			st = (uint8_t*)state[SDL_SCANCODE_RETURN];
			break;
		case 5:
			st = (uint8_t*)state[SDL_SCANCODE_ESCAPE];
			break;
		}
		if (st) {
			if (!IsBitSet(0, kState[i])) {
				SetBit(1, true, &kState[i]);
			}
			else {
				SetBit(1, false, &kState[i]);
			}
			SetBit(0, true, &kState[i]);
		}
		else {
			if (IsBitSet(0, kState[i])) {
				SetBit(1, true, &kState[i]);
			}
			else {
				SetBit(1, false, &kState[i]);
			}
			SetBit(0, false, &kState[i]);
		}

	}
}

//void wallet 1 art is 470 unlockart is 2 . CRT2
void UPDATE_PRICE_CRT(int walletptr, int itemptr)
{
	
	std::ostringstream s;
	
	char answ[255];
	s << "Type new price";
	GetSDLInput(s.str(), answ, 2);
	int price = atoi(answ);
	price -=  arts[GetArtIndexFromPtr(itemptr)].price; 
	std::cout << "NEW PRICE OFFSET IS " << price<<std::endl;
	unsigned char CRT[16 + 85 + 16];

	UintToBytes(1, CRT); // set UTXO ID which is 1
	memset(CRT + 4, 0, 77);  // set blank header TX
	UintToBytes(16 + 16, CRT + 81); // ok its smartcontract ptr (8) + all push op (4bytes foreach) + (4o) entry jump +  gas limit (4o) 
	// set Smart Contract pointer to 0 
	memset(CRT + 85, 0, 8);
	uint32_t bOff = 93; // 85 + 8 (bi ti) 
	// copy art ptr
	UintToBytes(itemptr, CRT + bOff);  bOff += 4;
	// copy wallet  ptr 
	UintToBytes(walletptr,  CRT + bOff);  bOff += 4;
	// copy time offset
	UintToBytes(0, CRT + bOff);  bOff += 4;
	// copy price
	UintToBytes(price, CRT + bOff);  bOff += 4;

	// put entry 4  and gas to 0 
	UintToBytes(4, CRT + bOff);  bOff += 4;
	memset(CRT + bOff, 0, 4);

	SET_CRT_HEADER(CRT, 16 + 85 + 16, 16 + 16);

	s.str("");
	char rdname[65];
	GetRandomHashString(rdname);
	s << outputDir << rdname;
	FILE* f = fopen(s.str().c_str(), "ab");
	if (f == NULL) return;
	fwrite(CRT, 1, 16 + 85 + 16, f);
	fclose(f);
	s.str("");
	s << "CRT DONE";
	GetSDLInput(s.str(), answ, 0);
	
}


void SAVAGE_BUY_CRT()
{

	char argstr[20];
	std::cout << "Type your wallet ptr :" << std::endl;
	std::cin.getline(argstr, 21);
	int walletptr = atoi(argstr);
	std::cout << "Type holder wallet ptr :" << std::endl;
	std::cin.getline(argstr, 21);
	int holderptr = atoi(argstr);
	std::cout << "Type item ptr :" << std::endl;
	std::cin.getline(argstr, 21);
	int itemptr = atoi(argstr);
	std::cout << "Type your new offset price :" << std::endl;
	std::cin.getline(argstr, 21);
	int price = atoi(argstr);

	unsigned char CRT[16 + 85 + 16];

	UintToBytes(1, CRT); // set UTXO ID which is 1
	memset(CRT + 4, 0, 77);  // set blank header TX
	UintToBytes(16 + 16, CRT + 81); // ok its smartcontract ptr (8) + all push op (4bytes foreach) + (4o) entry jump +  gas limit (4o) 
	// set Smart Contract pointer to 0 
	memset(CRT + 85, 0, 8);
	uint32_t bOff = 93; // 85 + 8 (bi ti) 

	// copy new price
	UintToBytes(price, CRT + bOff);  bOff += 4;
	// copy item  ptr 
	UintToBytes(itemptr, CRT + bOff);  bOff += 4;
	// copy holder ptr
	UintToBytes(holderptr, CRT + bOff);  bOff += 4;
	// copy wall ptr
	UintToBytes(walletptr, CRT + bOff);  bOff += 4;

	// put entry 4  and gas to 0 
	UintToBytes(5, CRT + bOff);  bOff += 4;
	memset(CRT + bOff, 0, 4);
	std::ostringstream s;
	s.str("");
	char rdname[65];
	GetRandomHashString(rdname);
	s << outputDir << rdname;
	FILE* f = fopen(s.str().c_str(), "ab");
	if (f == NULL) return;
	fwrite(CRT, 1, 16 + 85 + 16, f);
	fclose(f);
	std::cout << "CRT SUCCESSFULLY CRAFT..." << std::endl;
	getchar();
}
void UNLOCK_CRT(int walletptr, int itemptr) 
{
	
	unsigned char CRT[8 + 85 + 16];

	UintToBytes(1, CRT); // set UTXO ID which is 1
	memset(CRT + 4, 0, 77);  // set blank header TX
	UintToBytes(16 + 8, CRT + 81); // ok its smartcontract ptr (8) + all push op (4bytes foreach) + (4o) entry jump +  gas limit (4o) 
	// set Smart Contract pointer to 0 
	memset(CRT + 85, 0, 8);
	uint32_t bOff = 93; // 85 + 8 (bi ti) 
	// copy item ptr 
	UintToBytes(itemptr, CRT + bOff);  bOff += 4;
	// copy wallet ptr 
	UintToBytes(walletptr, CRT + bOff);  bOff += 4;

	// put entry 2  and gas to 0 
	UintToBytes(2, CRT + bOff);  bOff += 4;
	memset(CRT + bOff, 0, 4);

	SET_CRT_HEADER(CRT, 8 + 85 + 16, 8 + 16);


	std::ostringstream s;
	s.str("");
	char rdname[65];
	GetRandomHashString(rdname);
	s << outputDir << rdname;
	FILE* f = fopen(s.str().c_str(), "ab");
	if (f == NULL) return;
	fwrite(CRT, 1, 8 + 85 + 16, f);
	fclose(f);
	s.str("");
	s << "CRT DONE";
	GetSDLInput(s.str(), NULL, 0);
}

void LOCK_CRT(int walletptr, int itemptr)
{

	unsigned char CRT[8 + 85 + 16];

	UintToBytes(1, CRT); // set UTXO ID which is 1
	memset(CRT + 4, 0, 77);  // set blank header TX
	UintToBytes(16 + 8, CRT + 81); // ok its smartcontract ptr (8) + all push op (4bytes foreach) + (4o) entry jump +  gas limit (4o) 
	// set Smart Contract pointer to 0 
	memset(CRT + 85, 0, 8);
	uint32_t bOff = 93; // 85 + 8 (bi ti) 
	// copy item ptr 
	UintToBytes(itemptr, CRT + bOff);  bOff += 4;
	// copy wallet ptr 
	UintToBytes(walletptr, CRT + bOff);  bOff += 4;

	// put entry 2  and gas to 0 
	UintToBytes(3, CRT + bOff);  bOff += 4;
	memset(CRT + bOff, 0, 4);

	SET_CRT_HEADER(CRT, 8 + 85 + 16, 8 + 16);

	std::ostringstream s;
	s.str("");
	char rdname[65];
	GetRandomHashString(rdname);
	s << outputDir << rdname;
	FILE* f = fopen(s.str().c_str(), "ab");
	if (f == NULL) return;
	fwrite(CRT, 1, 8 + 85 + 16, f);
	fclose(f);
	s.str("");
	s << "CRT DONE";
	GetSDLInput(s.str(), NULL, 0);
}


void GetArtData(const ART& a, unsigned char * buffer) 
{
	FILE* f = fopen(storagePath, "rb");
	if (f == NULL) { return; } // throw error if cannot read
	fseek(f, a.ptr + 89, SEEK_SET);
	fread(buffer, 1, a.dataSize, f);
	fclose(f);
}

void ReadStorage() 
{
	
	wallets.clear();
	arts.clear();
	FILE* f = fopen(storagePath, "rb");
	if (f == NULL) { return; } // throw error if cannot read
	fseek(f, 0, SEEK_END);
	uint32_t lSize = ftell(f);
	rewind(f);
	unsigned char* strdata = (unsigned char*)malloc(lSize);
	fread(strdata, 1, lSize, f);
	fclose(f);

	int bOff = 0;
	while (bOff < lSize) 
	{

		unsigned char b = *(strdata + bOff);
		std::cout << "trailing byte = " <<(int)b << std::endl;
		if (*(strdata + bOff) == 0) 
		{
			
			std::cout << "[WALLET #" << bOff+1 << "]" << std::endl;
			std::cout << "Public Key :" << std::endl;
			PrintKey(strdata + bOff + 1);
			std::cout << "UTXO : " << BytesToUint(strdata+bOff+65) << std::endl;
			std::cout << "Items possessed :" << std::endl;
			int ictr = bOff + 69;
			for (int i = 0; i < 100; i++) {
				if (BytesToUint(strdata + ictr) != 0) {
					std::cout << BytesToUint(strdata + ictr) << std::endl;
					
				}
				ictr += 4;
			}
			wallets.push_back(WALLET());
			wallets[wallets.size() - 1].ptr = bOff + 1;
			memcpy(wallets[wallets.size() - 1].puKey, strdata + bOff + 1, 64);
			wallets[wallets.size() - 1].utxop = BytesToUint(strdata + bOff + 65);
			memcpy(wallets[wallets.size() - 1].items, strdata + bOff + 69, 400);



			bOff += 469;
			std::cout << std::endl;
		}
		else 
		{
			std::cout << "[ART #"<< bOff+1 <<"]" << std::endl;
			if (*(strdata + bOff + 1) == 1) {
				std::cout << "Art is in private market." << std::endl;
			}
			else 
			{
				std::cout << "Art is in savage  market." << std::endl;
			}
			std::cout << "Price : " << BytesToUint(strdata + bOff + 2) << std::endl;
			std::cout << "Creator Walled ID : " << BytesToUint(strdata + bOff + 6) << std::endl;
			std::cout << "Sign : " << std::endl;// << BytesToUint(strdata + bOff + 6) << std::endl;
			PrintKey(strdata + bOff + 10);
			std::cout << "Unix Timestamp : " << BytesToUint(strdata + bOff + 74) << std::endl;
			std::cout << "Last Price Update : " << BytesToUint(strdata + bOff + 78) << std::endl;
			std::cout << "Type : " << BytesToUint(strdata + bOff + 82) << std::endl;
			int dtsize = BytesToUint(strdata + bOff + 86);
			//----
			//PrintStuff(strdata + bOff + 90, dtsize, 0, 0);
			//----
			std::cout << "Size : " << dtsize<< std::endl;
			
			arts.push_back(ART());
			arts[arts.size() - 1].ptr = bOff + 1;
			arts[arts.size() - 1]._marketmode = *(strdata + bOff + 1);
			arts[arts.size() - 1].price = BytesToUint(strdata + bOff + 2);
			arts[arts.size() - 1]._creatorWalletPtr = BytesToUint(strdata + bOff + 6);
			memcpy(arts[arts.size() - 1].sign, strdata + bOff + 10, 64);
			arts[arts.size() - 1].unixTimeStamp = BytesToUint(strdata + bOff + 74);
			arts[arts.size() - 1]._lastpriceUpdate = BytesToUint(strdata + bOff + 78);
			arts[arts.size() - 1].type = BytesToUint(strdata + bOff + 82);
			arts[arts.size() - 1].dataSize = dtsize;

			bOff += dtsize + 90;
			
			std::cout<<std::endl;
		}
		
	}
	


}
/// <summary>
/// sort by : timestamp, price, only savage, type
/// 
/// </summary>
/// 
/// 
/* struct ART
{
	uint32_t      ptr;
	unsigned char _marketmode;
	uint32_t      price;
	uint32_t      _creatorWalletPtr;
	unsigned char sign[64];
	uint32_t      unixTimeStamp;
	uint32_t      _lastpriceUpdate;
	uint32_t      type;
	uint32_t      dataSize;

};
*/

void DisplayWalletInfo(const WALLET& W) 
{
	// display puk, display utxo index // display all arts 

	int selpos = 0;
	SDL_Color c = { 255, 255, 255 };

	int inpctr = 0;
	std::ostringstream s;
	while (1) 
	{
		FillRectangle(144, 10, 440, 620, 0, 0, 0);
		s.str("");
		s << "Key : ";
		char hexstr[129];
		GetKeyString((unsigned char*)W.puKey, hexstr);
		s << hexstr;
		PrintTXT(s.str(), 164, 10, 16 * s.tellp(), 20, c);
		
		s.str("");
		s << "Blockchain wallet ptr : " << W.utxop;
		PrintTXT(s.str(), 164, 30, 16 * s.tellp(), 20, c);
		
		// all items possessed
		int ipossessed = 0;
		int iamount = 0;
		for (int i = 0; i < 100; i++) {
			if (W.items[i] != 0) {
				ipossessed++; 
				iamount += arts[GetArtIndexFromPtr(W.items[i])].price;
			}
		}
		s.str("");
		s << "Arts possessed: " << ipossessed;
		PrintTXT(s.str(), 164, 50, 16 * s.tellp(), 20, c);
		s.str("");
		s << "Arts total value: " << iamount;
		PrintTXT(s.str(), 164, 70, 16 * s.tellp(), 20, c);
		

		

		int x = 164;
		int y = 90;
		int xstart = 48;
		int ictr = 0;
		int ptrsel = 0;
		for (int i = 0; i < 100; i++) {
			if (W.items[i] != 0) {
				int artindex = GetArtIndexFromPtr(W.items[i]);
				if (arts[artindex].type == 0) {
					// get art data
					if (selpos == ictr) {
						FillRectangle(x-5, y-5, SQUARE_TEX_SIZE + 10, SQUARE_TEX_SIZE + 10, 0, 0, 255);
						ptrsel = artindex;
					}
					unsigned char* data = (unsigned char*)malloc(arts[artindex].dataSize);
					GetArtData(arts[artindex], data);
					PrintImage(data, arts[artindex].dataSize, x, y, SQUARE_TEX_SIZE, SQUARE_TEX_SIZE);
					free(data);
					ictr++;
				}
			}
			
		}
		SDL_PollEvent(&e);
		// select stuff
		GetInputStates();
		const Uint8* state = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));
		if (IsBitSet(0, kState[3]) && IsBitSet(1, kState[3])) {
			SetBit(1, false, &kState[3]);
			selpos--;
		}
		if (state[SDL_SCANCODE_F5]) {
			RegisterMe();
			
		}
		if (IsBitSet(0, kState[1]) && IsBitSet(1, kState[1]))
		{
			SetBit(1, false, &kState[1]);
			selpos++;
		}
		if (selpos < 0) {
			selpos = ipossessed - 1;
		}
		if (selpos > ipossessed - 1) {
			selpos = 0; 
		}
		if (IsBitSet(0, kState[4]) && IsBitSet(1, kState[4]))
		{
			SetBit(1, false, &kState[4]);
			DisplayArtInfo(arts[ptrsel]);
		}
		if (IsBitSet(0, kState[5]) && IsBitSet(1, kState[5])) {
			SetBit(1, false, &kState[5]);
			std::cout << "close called" << std::endl;
			return;
		}
		Sleep(100);
			
	}
}
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}


void DisplayArtInfo(const ART& A) 
{

	int selpos = 0;
	SDL_Color c = { 255, 255, 255 };

	int inpctr = 0;
	std::ostringstream s;
	bool _reprintAll = true;
	int cpossessor = GetWalletPossessingArt(A.ptr);
	while (1)
	{
		if (_reprintAll) {
			FillRectangle(144, 10, 440, 620, 0, 0, 0);
			if (A.type == 0) {
				// get art data
				unsigned char* data = (unsigned char*)malloc(A.dataSize);
				GetArtData(A, data);
				PrintImage(data, A.dataSize, 164, 20, 400, 400);
				free(data);
			}
			else {
				PrintImage(placeHolder_BIN, 289839, 164, 20, 400, 400);
				if (A.type == 1) {
					s.str("");
					s << A.ptr << ".mp3";

					if (_access(s.str().c_str(), 0)) {
						FILE* f = fopen(s.str().c_str(), "ab");
						unsigned char* fdata = (unsigned char*)malloc(A.dataSize);
						GetArtData(A, fdata);
						fwrite(fdata, 1, A.dataSize, f);
						fclose(f);
						free(fdata);
					}
					s.str("");
					s << "play " << A.ptr << ".mp3 repeat";

					std::wstring stemp = s2ws(s.str());
					LPCWSTR b = stemp.c_str();
					mciSendString(b, NULL, 0, 0);
				}
				else if (A.type == 254) {
					unsigned char* fdata = (unsigned char*)malloc(A.dataSize);
					GetArtData(A, fdata);
					int seconds = A.dataSize / (44100 * 2);

					std::thread audiothd(PlayRecord, fdata, seconds);
					audiothd.detach();
					//PlayRecord(fdata, seconds);
				}
			}
			_reprintAll = false;
		}
	

		s.str("");
		FillRectangle(144, 420, 440, 220, 0, 0, 0);
		
		s << "Art identifier : " << A.ptr;
		PrintTXT(s.str(), 164, 420, 16 * s.tellp(), 20, c);
		s.str("");
		s << "Price : " << A.price << " gold";
		if (selpos == 0) {
			FillRectangle(164, 440, 32 * s.tellp(), 20, 0, 0, 255);
		}
		PrintTXT(s.str(), 164, 440, 16 * s.tellp(), 20,c );
		s.str("");

		if (A._marketmode == 1) {
			s << "Art available in private market";
			if (selpos == 1) {
				FillRectangle(164, 460, 32 * s.tellp(), 20, 0, 0, 255);
			}
			else {
				FillRectangle(164, 460, 32 * s.tellp(), 20, 0, 0, 0);
			}
			PrintTXT(s.str(), 164, 460, 16 * s.tellp(), 20,c );
		}
		else {
			
			s << "Art is inside savage market";
			if (selpos == 1) {
				FillRectangle(164, 460, 32 * s.tellp(), 20, 0, 0, 255);
			}
			else {
				FillRectangle(164, 460, 32 * s.tellp(), 20, 0, 0, 0);
			}
			PrintTXT(s.str(), 164, 460, 16 * s.tellp(), 20,c );
		}
		s.str("");
		s << "Sign : ";
		char hexstr[129];
		GetKeyString((unsigned char*)A.sign, hexstr);
		s << hexstr;
		if (selpos == 2) {
			FillRectangle(164, 480, 16 * s.tellp(), 20, 0, 0, 255);
		}
		else {
			FillRectangle(164, 480, 16 * s.tellp(), 20, 0, 0, 0);
		}
		PrintTXT(s.str(), 164, 480, 16 * s.tellp(), 20, c);
		
		s.str("");
		s << "HORODATAGE: " << A.unixTimeStamp;
		PrintTXT(s.str(), 164, 500, 16 * s.tellp(), 20,c );
		s.str("");
		s << "LAST PRICE UPDATE: " << A._lastpriceUpdate;
		PrintTXT(s.str(), 164, 520, 16 * s.tellp(), 20, c);
		s.str("");

		char tname[25];
		GetTypeString(A.type, tname);
		s << tname;
		PrintTXT(s.str(), 164, 540, 16 * s.tellp(), 20, c);

		s.str("");
		s << A.dataSize << " bytes";
		PrintTXT(s.str(), 164, 560, 16 * s.tellp(), 20, c);
		
		if (cpossessor > -1) {
			s.str("");
			s << "Holder : ";
			GetKeyString(wallets[cpossessor].puKey, hexstr);
			s << hexstr;
			if (selpos == 3) {
				FillRectangle(164, 580, 16 * s.tellp(), 20, 0, 0, 255);
			}
			else {
				FillRectangle(164, 580, 16 * s.tellp(), 20, 0, 0, 0);
			}
			PrintTXT(s.str(), 164, 580, 16 * s.tellp(), 20, c);
		}
		
		
		s.str("");
		s << "[BUY]";
		if (selpos == 4) {
			FillRectangle(164, 600, 32 * s.tellp(), 20, 0, 0, 255);
		}
		PrintTXT(s.str(), 164, 600, 32 * s.tellp(), 20, c);


		s.str("");
		s << "[EXTRACT]";
		if (selpos == 5) {
			FillRectangle(164, 620, 32 * s.tellp(), 20, 0, 0, 255);
		}
		PrintTXT(s.str(), 164, 620, 32 * s.tellp(), 20, c);
		
		inpctr++;
		if (inpctr == 1) {
			inpctr = 0;
			SDL_PollEvent(&e);
			// select stuff
			const Uint8* state = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));
			GetInputStates();
			if (IsBitSet(0, kState[4]) && IsBitSet(1, kState[4]))
			{
				SetBit(1, false, &kState[4]);
				switch (selpos) {
				case 0: 
					s.str("");
					if (!_reg) {
						s << "-> PLS REGISTER [F5] <-";
						GetSDLInput(s.str(), NULL, 0);
						
					}
					else 
					{
						int wi = GetWalletIndexfromPuk(myPUK);
						if (!DoesWalletContainsItem(wi, A.ptr)) {
							s << "NOT POSSESSED";
							GetSDLInput(s.str(), NULL, 0);
						}
						else 
						{
							UPDATE_PRICE_CRT(wallets[wi].ptr, A.ptr);
						}

					}
					_reprintAll = true;
					break;
					case 1:
						s.str("");
						if (!_reg) {
							s << "-> PLS REGISTER [F5] <-";
							GetSDLInput(s.str(), NULL, 0);

						}
						else
						{
							int wi = GetWalletIndexfromPuk(myPUK);
							if (!DoesWalletContainsItem(wi, A.ptr)) {
								s << "NOT POSSESSED";
								GetSDLInput(s.str(), NULL, 0);
							}
							else
							{
								if (A._marketmode == 1) {
									UNLOCK_CRT(wallets[wi].ptr, A.ptr);
								}
								else {
									LOCK_CRT(wallets[wi].ptr, A.ptr);
								}
							}

						}
						_reprintAll = true;
					break;
					case 2: 
						// display wallet 
						break;
					case 3:

						DisplayWalletInfo(wallets[cpossessor]);
						_reprintAll = true;
						break;
					case 4:
						// create a buy 
						s.str("");
						if (!_reg) {
							s << "-> PLS REGISTER [F5] <-";
							GetSDLInput(s.str(), NULL, 0);

						}
						else {
							
							int wi = GetWalletIndexfromPuk(myPUK);
							if (DoesWalletContainsItem(wi, A.ptr)) {
								s << "ALREADY POSSESSED";
								GetSDLInput(s.str(), NULL, 0);
							}
							else
							{
								// check if art is savage or not 
								if (A._marketmode == 1) 
								{
									s.str("");
									s << "LOCKED BUY NOT AVAILABLE";
									GetSDLInput(s.str(), NULL, 0);
								}
								else 
								{

								}
							}
							
							
						}
					
						_reprintAll = true;
						break;

					case 5:
						s.str("");
						s << A.ptr;
						remove(s.str().c_str());
						FILE* f = fopen(s.str().c_str(), "ab");
						unsigned char* fdata = (unsigned char*)malloc(A.dataSize);
						GetArtData(A, fdata);
						fwrite(fdata, 1, A.dataSize, f);
						fclose(f);
						free(fdata);
						s.str("");
						s << "FILE EXTRACTED!";
						GetSDLInput(s.str(), NULL, 0);
						break;
						
				}
			
					
			}
			if (state[SDL_SCANCODE_F5]) {
				RegisterMe();
				_reprintAll = true;
			}
			if (IsBitSet(0, kState[5]) && IsBitSet(1, kState[5]))
			{
				SetBit(1, false, &kState[5]);
				Sleep(50);
				if (A.type == 1) {
					s.str("");
					s << "close " << A.ptr << ".mp3";

					std::wstring stemp = s2ws(s.str());
					LPCWSTR b = stemp.c_str();
					mciSendString(b, NULL, 0, 0);
				}
				
				std::cout << "close called" << std::endl;
				return;
			}
			if (IsBitSet(0, kState[0]) && IsBitSet(1, kState[0]))
			{
				SetBit(1, false, &kState[0]);
				selpos--;
			}
			
			if (IsBitSet(0, kState[2]) && IsBitSet(1, kState[2]))
			{
				SetBit(1, false, &kState[2]);
				selpos++;
			}
			if (selpos == 6) {
				selpos = 0;
			}
			if (selpos == -1) {
				selpos = 5;
			}
			
		}
	
		
		Sleep(50);
	}
}

int GetWalletPossessingArt(int iptr)
{
	for (int i = 0; i < wallets.size(); i++) {
		for (int n = 0; n < 100; n++)
		{
			
				if (wallets[i].items[n] == iptr) {
					return i;
				}
			
			
		}
	}
	return -1;
}
int GetWalletIndexfromPuk(unsigned char * PUK) 
{
	for (int i = 0; i < wallets.size(); i++) {
		if (memcmp(PUK, wallets[i].puKey, 64) == 0) {
			return i;
		}
	}
	return -1;
}
int GetWalletIndexFromPtr(int ptr)
{
	for (int i = 0; i < wallets.size(); i++) {
		if (ptr == wallets[i].ptr) {
			return i;
		}
	}
	return -1;
}
int GetArtIndexFromPtr(int ptr)
{
	for (int i = 0; i < arts.size(); i++) {
		if (ptr == arts[i].ptr) {
			return i;
		}
	}
	return -1;
}
bool DoesWalletContainsItem(int wallIndex, int itemptr) 
{
	if (wallIndex == -1)
		return false;

	for (int i = 0; i < 100; i++)
	{
		if (wallets[wallIndex].items[i] == itemptr) {
			return true;
		}
	}

}

bool GetSDLInput(std::string msg, char * result, unsigned char entryMode, bool _acceptdrop) 
{
	Sleep(100);
	std::ostringstream r;
	r.str("");
	SDL_Color c = { 255, 255, 255 };
	int sel = 0;
	Sleep(50);
	while (1) {
		FillRectangle(130, 200, 480, 200, 0, 0, 150);
		PrintTXT(msg, 164, 220, 16 * sizeof(msg), 20, c);
		// entry mode : 0 (only OK), 1(YES OR NO) , 2 (OK with input)
		if (entryMode == 0) {
			FillRectangle(164, 240, 32 * 4, 20, 0, 0, 255);
			PrintTXT("[OK]", 164, 240, 32 * 4, 20, c);
		}
		else if (entryMode == 1) {
			if (sel == 0) {
				FillRectangle(164, 240, 32 * 3, 20, 0, 0, 255);
			}
			else {
				FillRectangle(164 + 32 * 3 + 20, 240, 32 * 3, 20, 0, 0, 255);
			}
			PrintTXT("[Y]", 164, 240, 32 * 3, 20, c);
			PrintTXT("[N]", 164 + 32 * 3 + 20, 240, 32 * 3, 20, c);
		}
		else if (entryMode == 2 ) 
		{
			if (sel == 0) {
				
				FillRectangle(164, 240, 406, 20, 0, 0, 255);
			}
			if (r.tellp() > 0) {
				PrintTXT(r.str(), 164, 240, 16 * r.tellp(), 20, c);
			}
			if (sel == 1) {
				FillRectangle(164, 260, 32 * 4, 20, 0, 0, 255);
			}
			
			PrintTXT("[OK]", 164, 260, 32 * 4, 20, c);
		}
	

		SDL_PollEvent(&e);
		
		const Uint8* state = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));
		if (e.type == SDL_DROPFILE) {
			char * dropped_filedir = e.drop.file;
			r.str("");
			r << e.drop.file;
			std::cout << e.drop.file;
			SDL_free(dropped_filedir);
		}
		if (state[SDL_SCANCODE_RETURN]) {
			//SetBit(1, false, &kState[4]);
			if (entryMode == 0) {
				return 1;
			}
			else if (entryMode == 1) {
				if (sel == 0) {
					return 1; 

				}
				else {
					return 0;
				}
			}
			else if (entryMode == 2 && sel==1) 
			{
				strcpy(result, r.str().c_str());
				return 1;
			}
			
		}
		if (sel == 0 && entryMode == 2) {
			if (e.type == SDL_TEXTINPUT) {
				r << e.text.text;
			}
			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_BACKSPACE && r.tellp())
				{
					std::string s = r.str().substr(0, r.str().size() - 1);
					std::cout << s;
					r.str("");
					r << s;

				}
			}
		}
		if (state[SDL_SCANCODE_DOWN]) {
			sel--;
		}
		if (state[SDL_SCANCODE_UP]) {
			sel++;
		}
		if (sel == 2) {
			sel = 0;
		}
		if (sel == -1) {
			sel = 1;
		}
		
		Sleep(45);
	}
	

}

void RegisterMe() {
	std::ostringstream s;
	if (_reg) {
		s << "Reg from another account?";
		if (!GetSDLInput(s.str(), NULL, 1)) {
			return;
		}
	}
	_reg = false;
		char answ[255];
		s << "Type Blockchain Wallet ID";
		GetSDLInput(s.str(), answ, 2);
		myUTXO = atoi(answ);

		s.str("");
		s << "Type PUK path";
		GetSDLInput(s.str(), answ, 2);
		FILE* f = fopen(answ, "rb");
		if (f == NULL) { 
			s.str("");
			s << "File not found";
			GetSDLInput(s.str(), answ, 0);
			return; 
		}
		fread(myPUK, 1, 64, f);
		fclose(f);

		s.str("");
		s << "Type PRK path";
		GetSDLInput(s.str(), answ, 2);
		f = fopen(answ, "rb");
		if (f == NULL) {
			s.str("");
			s << "File not found";
			GetSDLInput(s.str(), answ, 0);
			return;
		}
		fread(myPRK, 1, 64, f);
		fclose(f);

		if (GetWalletIndexfromPuk(myPUK) == - 1 ) {
			s.str("");
			s << "No Wallet. Register in BitMarket?";
			if (GetSDLInput(s.str(), NULL, 1)) {
				REG_ME_CRT(myUTXO);
			}
		}

		s.str("");
		s << "REG SUCCESSED";
		GetSDLInput(s.str(), answ, 0);
		_reg = true;
	
	
}

void GetPathInfo() {
	return;
	FILE* f = fopen("path.ini", "rb");
	if (f == NULL) {
		return;
	}
	// reading line by line, max 256 bytes
	char buffer[256];
	fgets((char*)storagePath, 256, f);
	fgets((char*)outputDir, 256, f);
	fclose(f);
}


void ShowMenu() 
{

	int selpos = 0;
	int selline = 0;
	SDL_Color c = { 255, 255, 255 };

	int inpctr = 0;
	std::ostringstream s;
	bool _reprintAll = true;

	while (1) {
		
	
		if (_reprintAll) 
		{
			FillRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 0);
			s.str("");
			s << "-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_";
			PrintTXT(s.str(), 364 - (16 * s.tellp() / 2), 30, 16 * s.tellp(), 20, c);
			s.str("");
			s << "-_-_-_-_ BIMMARKET 1.0-_-_-_-_";
			PrintTXT(s.str(), 364 - (16 * s.tellp() / 2), 50, 16 * s.tellp(), 20, c);
			s.str("");
			s << "-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_";
			PrintTXT(s.str(), 364 - (16 * s.tellp() / 2), 70, 16 * s.tellp(), 20, c);
			s.str("");
			s << "Number of Registration : " << wallets.size();
			PrintTXT(s.str(), 364 - (16 * s.tellp() / 2), 90, 16 * s.tellp(), 20, c);
			
			s.str("");
			s << "Number of arts : " << arts.size();
			PrintTXT(s.str(), 364 - (16 * s.tellp() / 2), 110, 16 * s.tellp(), 20, c);
			s.str("");

			s.str("");
			if (!_reg) {
				s << "REGISTER";
			}
			else {
				char kString[139]; 
				char cutStr[6];
				GetKeyString(myPUK, kString);
				memcpy(cutStr, kString, 6);
				s << "REG AS " << cutStr << "...";
			}
			
			if (selpos == 0) {
				FillRectangle(364 - (16 * s.tellp() / 2), 130, 16 * s.tellp(), 20,  0, 0, 255);
			}
			PrintTXT(s.str(), 364 - (16 * s.tellp() / 2), 130, 16 * s.tellp(), 20, c);
			s.str("");

			s.str("");
			s << "SEARCH WALLET";
			if (selpos == 1) {
				FillRectangle(364 - (16 * s.tellp() / 2), 150, 16 * s.tellp(), 20, 0, 0, 255);
			}
			PrintTXT(s.str(), 364 - (16 * s.tellp() / 2), 150, 16 * s.tellp(), 20, c);
			s.str("");

			s.str("");
			s << "SEARCH ART";
			if (selpos == 2) {
				FillRectangle(364 - (16 * s.tellp() / 2), 170, 16 * s.tellp(), 20, 0, 0, 255);
			}
			PrintTXT(s.str(), 364 - (16 * s.tellp() / 2), 170, 16 * s.tellp(), 20, c);
			s.str("");

			s.str("");
			s << "ADD ART";
			if (selpos == 3) {
				FillRectangle(364 - (16 * s.tellp() / 2), 190, 16 * s.tellp(), 20, 0, 0, 255);
			}
			PrintTXT(s.str(), 364 - (16 * s.tellp() / 2), 190, 16 * s.tellp(), 20, c);
			s.str("");
			s << "RELOAD STORAGE";
			if (selpos == 4) {
				FillRectangle(364 - (16 * s.tellp() / 2), 210, 16 * s.tellp(), 20, 0, 0, 255);
			}
			PrintTXT(s.str(), 364 - (16 * s.tellp() / 2), 210, 16 * s.tellp(), 20, c);
			s.str("");
			
			ShowItems(0, false, false, selpos, selline, -1, false);
			_reprintAll = false;
		}
		SDL_PollEvent(&e); const Uint8* state = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));
		GetInputStates();

		if (state[SDL_SCANCODE_F5]) {
			RegisterMe();
			_reprintAll = true;
		}
		if (state[SDL_SCANCODE_F4]) {
			Special_BIM();
			_reprintAll = true;
		}
		if (selpos < 5) {
			if (IsBitSet(0, kState[0]) && IsBitSet(1, kState[0]))
			{
				SetBit(1, false, &kState[0]);
				selpos--;
				_reprintAll = true;
			}
			if (IsBitSet(0, kState[2]) && IsBitSet(1, kState[2]))
			{
				SetBit(1, false, &kState[2]);
				selpos++;
				_reprintAll = true;
			}
		}
		else {
			if (IsBitSet(0, kState[0]) && IsBitSet(1, kState[0]))
			{
				SetBit(1, false, &kState[0]);
				if (selpos - 7 < 5) {
					if (selline > 0) {
						selline--;
					}
					else {
						selpos = 4;
					}
				}
				else {
					selpos -= 7;
				}
				
				_reprintAll = true;
			}
			if (IsBitSet(0, kState[2]) && IsBitSet(1, kState[2]))
			{
				SetBit(1, false, &kState[2]);
				
				if (selpos + 7  > 28) {
					selline++;
				}
				else {
					selpos += 7;
				}
				_reprintAll = true;
			}
			if (IsBitSet(0, kState[1]) && IsBitSet(1, kState[1]))
			{
				SetBit(1, false, &kState[1]);
				
				selpos++;
				_reprintAll = true;
			}
			if (IsBitSet(0, kState[3]) && IsBitSet(1, kState[3]))
			{
				SetBit(1, false, &kState[3]);
				selpos--;
				_reprintAll = true;
			}
		}
		if (IsBitSet(0, kState[4]) && IsBitSet(1, kState[4]) && selpos > 4)
		{
			SetBit(1, false, &kState[4]);
			ShowItems(0, false, false, selpos, selline, -1, true);
			_reprintAll = true;
		}
		if (selpos < 0) {
			selpos = 0;
		}
		
		if (selpos < 5) {
			if (IsBitSet(0, kState[4]) && IsBitSet(1, kState[4]))
			{
				SetBit(1, false, &kState[4]);
				switch (selpos) {
					case 0 : 
					RegisterMe();
				    _reprintAll = true;
					break;
					case 1:
						s.str("");
						s << "Type Wallet ID";
						char result[255];
						GetSDLInput(s.str(), result, 2);
						int iptr;
						int wi;
						 iptr = atoi(result);

						 wi = GetWalletIndexFromPtr(iptr); 
						if (wi == -1) {
							s.str("");
							s << "Wallet not exists";
							GetSDLInput(s.str(), NULL, 0);
						}
						else {
							DisplayWalletInfo(wallets[wi]);
						}
						
						_reprintAll = true;
					break;
					case 2:
						s.str("");
						s << "Type Art ID";
						GetSDLInput(s.str(), result, 2);
						 iptr = atoi(result);

						wi = GetArtIndexFromPtr(iptr);
						if (wi == -1) {
							s.str("");
							s << "Art not exists";
							GetSDLInput(s.str(), NULL, 0);
						}
						else {
							DisplayArtInfo(arts[wi]);
						}

						_reprintAll = true;
						break;
					case 3: 
						s.str("");
						if (!_reg) {
							s << "-> PLS REGISTER [F5] <-";
							GetSDLInput(s.str(), NULL, 0);

						}
						else {
							s << "Drop or type file path";
							GetSDLInput(s.str(), result, 2, true);
							int wi = GetWalletIndexfromPuk(myPUK);
							REG_ART_CRT(result, wallets[wi].ptr);
						}
						_reprintAll = true;
						break;
					case 4 : 
						ReadStorage();
						_reprintAll = true;
						break;
				}

			}
		}

	}

}

void ShowItems(unsigned char sortMode, bool _ascending, bool onlySavage, int selpos, int selline, int onlyType, bool _enterPressed)
{
	// add 1 to sortMode if descending or ascending
	if (!_ascending) {
		sortMode++;
	}
	
	switch (sortMode)
	{
	case 0: 
		std::sort(arts.begin(), arts.end(), compareArtByPtr_A);
		break;
	case 1: 
		std::sort(arts.begin(), arts.end(), compareArtByPtr_D);
		break;
	case 2: 
		std::sort(arts.begin(), arts.end(), compareArtByPrice_A);
		break;
	case 3: 
		std::sort(arts.begin(), arts.end(), compareArtByPrice_D);
		break;
	}
	
	int x = 48;
	int y = 242;
	int xstart = 48;

	if (selpos > 4 + arts.size()) {
		selpos = 4 + arts.size();
	}
	int actr = 0;

	/*
	int showNumber = arts.size(); 
	if (showNumber > 28) {
		showNumber = 28;
	}*/
	int startIndex = selline * 7;
	int endIndex = 28 + startIndex;

	if (arts.size() < endIndex) {
		endIndex = arts.size()- startIndex;
	}
	std::cout << startIndex << std::endl;
	std::cout << endIndex << std::endl;
	for (int i = startIndex; i < endIndex; i++) {

		if (onlySavage && arts[i]._marketmode == 1) 
			continue;
		if (onlyType !=  -1 && arts[i].type != onlyType)
			continue;

		if (selpos == 5 + actr) {
			FillRectangle(x - 10, y - 10, SQUARE_TEX_SIZE + 20, SQUARE_TEX_SIZE + 20, 0, 0, 255);
		}
		if (arts[i].type == 0) {
			// get art data
			unsigned char* data = (unsigned char*)malloc(arts[i].dataSize);
			GetArtData(arts[i], data);
			PrintImage(data, arts[i].dataSize, x, y, SQUARE_TEX_SIZE, SQUARE_TEX_SIZE);
			free(data);
		}
		else {
			PrintImage(placeHolder_BIN, arts[i].dataSize, x, y, SQUARE_TEX_SIZE, SQUARE_TEX_SIZE);
		}
		std::ostringstream s;
		s << "# " << arts[i].ptr;
		SDL_Color c = { 255, 255, 255 };
		PrintTXT(s.str(), x, y + SQUARE_TEX_SIZE, 40, 20, c);
		s.str("");
		s  << arts[i].price << " gold";
		PrintTXT(s.str(), x, y + SQUARE_TEX_SIZE + 20, 80, 20, c);
		x += SQUARE_TEX_SIZE + 20;
		if (x > WINDOW_WIDTH - SQUARE_TEX_SIZE -  20) {
			x = xstart;
			y+= SQUARE_TEX_SIZE + 40;
		}
	
		if (_enterPressed && selpos == 5 + actr)
		{
			SetBit(1, false, &kState[4]);
			DisplayArtInfo(arts[i]);

		}
		actr++;
		
	}
	
}

bool compareArtByPrice_A(const ART& a, const ART& b)
{
	return a.price < b.price;
}
bool compareArtByPrice_D(const ART& a, const ART& b)
{
	return a.price > b.price;
}
bool compareArtByTimeStamp_A(const ART& a, const ART& b)
{
	return a.unixTimeStamp < b.unixTimeStamp;
}
bool compareArtByTimeStamp_D(const ART& a, const ART& b)
{
	return a.unixTimeStamp > b.unixTimeStamp;
}
bool compareArtByPtr_A(const ART& a, const ART& b)
{
	return a.ptr < b.ptr;
}
bool compareArtByPtr_D(const ART& a, const ART& b)
{
	return a.ptr > b.ptr;
}

void GetTypeString(int type, char* buffer) 
{
	switch (type) {
	case 0 : 
		strcpy(buffer, "JPEG");
		return;
	case 1:
		strcpy(buffer, "MP3");
		return;
	case 254 : 
		strcpy(buffer, "RAW WAV");
		return;
	}
	strcpy(buffer, "UNKNOW");
	return;
}

int GetFileType(char* fpath) 
{
	if (strstr(fpath, ".jpg")) {
		return 0;
	}
	else if (strstr(fpath, ".jpeg")) {
		return 0;
	}
	else if (strstr(fpath, ".png")) {
		return 0;
	}
	else if (strstr(fpath, ".bmp")) {
		return 0;
	}
	else if (strstr(fpath, ".tiff")) {
		return 0;
	}
	else if (strstr(fpath, ".mp3")) {
		return 1;
	}
	else if (strstr(fpath, ".wav")) {
		return 1;
	}
	else if (strstr(fpath, ".mpeg")) {
		return 1;
	}
	else if (strstr(fpath, ".txt")) {
		return 2;
	}
	return -1; 
}

void REG_ART_CRT(char * fpath, int walletptr) 
{
	std::cout << "w ptr " << walletptr;

	std::ostringstream s;
	s.str("");
	s << "Type Price";
	char result[20];
	GetSDLInput(s.str(), result, 2);
	int price = atoi(result);
	
	int type = 255;
	s.str("");
	bool tOK = false;
	int tf = GetFileType(fpath);
	if (tf > -1) {
		s << "Extension auto-found. Continue?";
		if (GetSDLInput(s.str(), result, 1)) {
			tOK = true;
			type = tf;
		}
		
	}
	if (!tOK) {
		s.str("");
		s << "Set type of your file: ";
		GetSDLInput(s.str(), result, 2);
		type = atoi(result);
	}
	

	FILE* f = fopen(fpath, "rb");
	if (f == NULL) { return; } // throw error if cannot read
	fseek(f, 0, SEEK_END);
	uint32_t lSize = ftell(f);
	rewind(f);
	uint32_t flrSize = nearestmultiple(lSize, 4, false);
	unsigned char* fdata = (unsigned char*)malloc(flrSize);
	memset(fdata, 0, flrSize);
	fread(fdata, 1, lSize, f);
	fclose(f);
	if (lSize>900000) {
		s.str("");
		s << "File too big ";
		GetSDLInput(s.str(), result, 0);
		return;
	}
	Sha256.init();
	Sha256.write((char*)fdata, flrSize);
	unsigned char hash[32];
	memcpy(hash, Sha256.result(), 32);
	printHash(Sha256.result());
	std::cout << "HASH IS : ";
	PrintRawBytes(hash, 32);
	
	
	uint8_t sign[64];
	uECC_sign(myPRK, hash, 32, sign, uECC_secp256k1());
	std::cout << "file successfully signed..." << std::endl;
	std::cout << "SIGN" << std::endl;

	
	std::cout << "SIGN IS : "; 
	PrintRawBytes(sign, 64);
	
	std::cout << flrSize << std::endl;
	;
	uint32_t  pushnum = (flrSize / 4) + 20;
	std::cout << pushnum << std::endl;
	

	unsigned char* CRT = (unsigned char*)malloc(85 + 16 + (pushnum * 4));
	UintToBytes(1, CRT); // set UTXO ID which is 1
	memset(CRT + 4, 0, 77);  // set blank header TX
	UintToBytes(16 + (pushnum * 4), CRT + 81); // ok its smartcontract ptr (8) + all push op (4bytes foreach) + (4o) entry jump +  gas limit (4o) 
	// set Smart Contract pointer to 0 
	memset(CRT + 85, 0, 8);
	uint32_t bOff = 93; // 85 + 8 (bi ti) 
	// copy bitfile

	for (int i = flrSize - 4; i >= 0; i -= 4)
	{
		memcpy(CRT + bOff, fdata + i, 4);
		bOff += 4;
	}


	// copy bitfile size 
	UintToBytes(flrSize, CRT + bOff);  bOff += 4;
	// copy bitfile type 
	UintToBytes(type, CRT + bOff);  bOff += 4;
	// copy bitfile sign
	
	for (int i = 60; i >= 0; i -= 4) {
		memcpy(CRT + bOff, sign + i, 4);
		bOff += 4;
	}
	// copy  initprice 
	UintToBytes(price, CRT + bOff);  bOff += 4;
	// copy  walletptr 
	UintToBytes(walletptr, CRT + bOff);  bOff += 4;

	// copy  entry which is index 1 
	UintToBytes(1, CRT + bOff);  bOff += 4;
	// put gas limit to 0 
	memset(CRT + bOff, 0, 4);

	SET_CRT_HEADER(CRT, 85 + 16 + (pushnum * 4), 16 + (pushnum * 4));

	s.str("");
	char rdname[65];
	GetRandomHashString(rdname);
	s << outputDir << rdname;
	f = fopen(s.str().c_str(), "ab");
	if (f == NULL) return;
	fwrite(CRT, 1, 85 + 16 + (pushnum * 4), f);
	fclose(f);
	s.str("");
	s << "CRT SUCCESSED ";
	GetSDLInput(s.str(), result, 0);
	PrintRawBytes(CRT + 85 + 16 + (pushnum * 4) - 200, 200);

	if (uECC_verify(myPUK, hash, 32, sign, uECC_secp256k1())) {
		std::cout << "signature is valid! " << std::endl;
	}
	
	free(fdata);
	PrintRawBytes(CRT, 100);
	free(CRT);

}

void REG_ME_CRT(int utxop) 
{

	unsigned char CRT[68 + 85 + 16];

	UintToBytes(1, CRT); // set UTXO ID which is 1
	memset(CRT + 4, 0, 77);  // set blank header TX
	UintToBytes(16 + 68, CRT + 81); // ok its smartcontract ptr (8) + all push op (4bytes foreach) + (4o) entry jump +  gas limit (4o) 
	// set Smart Contract pointer to 0 
	memset(CRT + 85, 0, 8);
	uint32_t bOff = 93; // 85 + 8 (bi ti) 
	// copy pukey
	for (int i = 60; i >= 0; i -= 4) {
		memcpy(CRT + bOff, myPUK + i, 4);
		bOff += 4;
	}
	// copy utxop 
	UintToBytes(utxop, CRT + bOff);  bOff += 4;
	
	// put entry and gas to 0 
	memset(CRT + bOff, 0, 8);
	SET_CRT_HEADER(CRT, 68 + 85 + 16, 68 + 16 );

	std::ostringstream s;
	s.str("");
	char rdname[65];
	GetRandomHashString(rdname);
	s << outputDir << rdname;
	FILE* f = fopen(s.str().c_str(), "ab");
	if (f == NULL) return;
	fwrite(CRT, 1, 68 + 85 + 16, f);
	fclose(f);
	
	s.str("");
	s << "CRT SUCCESSED";
	GetSDLInput(s.str(), NULL, 0);
}

void GetRandomHashString( char* buffer) 
{
	uint32_t ts = GetTimeStamp();
	Sha256.init();
	Sha256.write((char*)&ts, 4); 
	GetHashString(Sha256.result(), buffer);

}
void SET_CRT_HEADER(unsigned char* CRTDATA, int TXSIZE, int CRTSIZE )
{

	std::ostringstream s;
	s.str("");
	s << "Type NONCE";
	char result[20];
	GetSDLInput(s.str(), result, 2);
	int nonce = atoi(result);

	s.str("");
	s << "Type FEE";
	GetSDLInput(s.str(), result, 2);
	int fee = atoi(result);

	s.str("");
	s << "Type GAS LIMIT";
	GetSDLInput(s.str(), result, 2);
	int _guserLimit = atoi(result);


	UintToBytes(myUTXO, CRTDATA); // UTXOP
	UintToBytes(nonce, CRTDATA + 68); // TOU AT +68 
	uint32_t ptime = GetTimeStamp() + 5000000;
	UintToBytes(ptime, CRTDATA + 72); // ts AT +72 
	UintToBytes(fee, CRTDATA + 76); // fee AT +76 
	UintToBytes(2, CRTDATA + 80); // TX TYPE  IS 2 FOR CRT AT +77 
	std::cout << "size " << TXSIZE - 85 << std::endl;
	UintToBytes(TXSIZE - 85, CRTDATA + 81); // datasize  length  AT +78 

	// update also bloc index and tx index is 0 so dont upload and upload max gas 
	UintToBytes(30, CRTDATA + 85); // bloc 30 
	UintToBytes(_guserLimit, CRTDATA + TXSIZE - 4); //guser limit 

	int ds = CRTSIZE;
	unsigned char* data = (unsigned char*)malloc(21 + ds);
	memcpy(data, CRTDATA, 4);
	std::cout << 68 + 17 + ds << std::endl;
	memcpy(data + 4, CRTDATA + 68, 17 + ds);
	Sha256.init();
	Sha256.write((char*)data, 21 + ds); // hash transaction 
	unsigned char hash[32];
	memcpy(hash, Sha256.result(), 32);
	free(data); // release heap alloc
	unsigned char sign[64];
	
	if (uECC_sign(myPRK, hash, 32, sign, uECC_secp256k1())) {
		std::cout << "DATA transaction successfully signed" << std::endl;
	}
	// copy sign at +4 
	memcpy(CRTDATA + 4, sign, 64);

}

uint32_t BytesToUint(unsigned char* arr)
{
	uint32_t foo;
	// big endian
	foo = (uint32_t)arr[3] << 24;
	foo |= (uint32_t)arr[2] << 16;
	foo |= (uint32_t)arr[1] << 8;
	foo |= (uint32_t)arr[0];

	return foo;
}

void UintToBytes(uint32_t v, unsigned char* a)
{
	a[3] = v >> 24;
	a[2] = v >> 16;
	a[1] = v >> 8;
	a[0] = v;
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

void PrintRawBytes(unsigned char* ptr, int length)
{
	std::cout << std::endl;
	for (int i = 0; i < length; i++) {
		std::cout << (int)ptr[i] << " ";
	}
	std::cout << std::endl;
}

void PrintKey(unsigned char* buff)
{
	unsigned char data[64];
	memcpy(data, buff, 64);

	for (int i = 0; i < 64; i++) {
		std::cout << ("0123456789abcdef"[data[i] >> 4]);
		std::cout << ("0123456789abcdef"[data[i] & 0xf]);
	}
	std::cout << std::endl;

}
void GetKeyString(unsigned char* buff, char* stringbuffer)
{
	unsigned char data[64];
	memcpy(data, buff, 64);
	std::ostringstream s;
	for (int i = 0; i < 64; i++) {
		s << ("0123456789abcdef"[data[i] >> 4]);
		s << ("0123456789abcdef"[data[i] & 0xf]);
	}
	strcpy(stringbuffer, s.str().c_str());
	
}
void SetBit(int p, bool b, unsigned char* byte)// not working here cause only for int 
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


//------------- SP
bool _autoRCD = false;
void AutoREC(int stNonce);

void AutoREC(int stNonce) 
{
	int wi = GetWalletIndexfromPuk(myPUK);
	while (_autoRCD)
	{
		if (std::experimental::filesystem::v1::is_empty(outputDir))
		{
			// wait if a directory have file 
			short* data;
			data = RecordMic(5);
			AUTOREG_ART_CRT((unsigned char*)data, (44100 * 5 * 2), wallets[wi].ptr, stNonce);
			stNonce++;
		}
		else {
			Sleep(1000);
		}

		
	}
}
void Special_BIM()
{
	int selpos = 0;
	SDL_Color c = { 255, 255, 255 };
	std::ostringstream s;

	if (!_reg) {
		s.str("");
		s << "-> PLS REGISTER [F5] <-";
		GetSDLInput(s.str(), NULL, 0);
		return;
	}
	

	while (1) {
		FillRectangle(144, 10, 440, 620, 0, 0, 0);
		s.str("");
		s << "**SPECIAL BIM EVENT **";
		PrintTXT(s.str(), 364 - (16 * s.tellp() / 2), 170, 16 * s.tellp(), 20, c);
		s.str("");
		s << "MANUAL RECORD";
		if (selpos == 0) {
			FillRectangle(164, 190, 32 * s.tellp(), 20, 0, 0, 255);
		}
		PrintTXT(s.str(), 164, 190, 16 * s.tellp(), 20, c);
		s.str("");
		if (_autoRCD) {
			s << "STOP AUTO RECORD";
		}
		else {
			s << "START AUTO RECORD";
		}
		
		if (selpos == 1) {
			FillRectangle(164, 210, 32 * s.tellp(), 20, 0, 0, 255);
		}
		PrintTXT(s.str(), 164, 210, 16 * s.tellp(), 20, c);
		s.str("");


		SDL_PollEvent(&e);
		// select stuff
		const Uint8* state = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));
		GetInputStates();

		if (IsBitSet(0, kState[4]) && IsBitSet(1, kState[4]))
		{
			SetBit(1, false, &kState[4]);
			switch (selpos) {
			case 0:
				if (_autoRCD) {
					s.str("");
					s << "PLS STOP AUTO REC";
					GetSDLInput(s.str(), NULL, 0);
					break;
				}
				char answ[255];
				s.str("");
				s << "Type time to rec (s)";
				GetSDLInput(s.str(), answ, 2);
				int seconds;
				seconds = atoi(answ);
				s.str("");
				s << "PRESS OK 2 RECORD";
				GetSDLInput(s.str(), answ, 0);
				short* data; 
				data = RecordMic(seconds);

				s.str("");
				s << "Type NONCE";
				GetSDLInput(s.str(), answ, 2);
				int nonce;
				nonce = atoi(answ);
				int wi;
				wi = GetWalletIndexfromPuk(myPUK);
				AUTOREG_ART_CRT((unsigned char*)data, (44100 * seconds * 2), wallets[wi].ptr, nonce);
				break;
			case 1 : 
				if (_autoRCD) {
					_autoRCD = false;
					s.str("");
					s << "AUTOREC STOPPED";
					GetSDLInput(s.str(), NULL, 0);
				}
				else {

					s.str("");
					s << "Type NONCE";
					GetSDLInput(s.str(), answ, 2);
					nonce = atoi(answ);
					std::thread autoRCD(AutoREC, nonce);
					autoRCD.detach();
					_autoRCD = true;
				}
				break;
				
			}
			
		}
		if (IsBitSet(0, kState[5]) && IsBitSet(1, kState[5]))
		{
			SetBit(1, false, &kState[5]);
			Sleep(50);

			std::cout << "close called" << std::endl;
			return;
		}

		if (IsBitSet(0, kState[0]) && IsBitSet(1, kState[0]))
		{
			SetBit(1, false, &kState[0]);
			selpos--;
		}

		if (IsBitSet(0, kState[2]) && IsBitSet(1, kState[2]))
		{
			SetBit(1, false, &kState[2]);
			selpos++;
		}
		if (selpos == 2) {
			selpos = 0;
		}
		if (selpos == -1) {
			selpos = 1;
		}
		Sleep(50);
	}
}


void AUTOREG_ART_CRT(unsigned char* rdata, int size, int walletptr, int nonce)
{
	
	int price = 1;
	int type = 254;
	
	uint32_t flrSize = nearestmultiple(size, 4, false);
	unsigned char* fdata = (unsigned char*)malloc(flrSize);
	memset(fdata, 0, flrSize);
	memcpy(fdata, rdata, size );
	
	Sha256.init();
	Sha256.write((char*)fdata, flrSize);
	unsigned char hash[32];
	memcpy(hash, Sha256.result(), 32);
	printHash(Sha256.result());
	std::cout << "HASH IS : ";
	PrintRawBytes(hash, 32);


	uint8_t sign[64];
	uECC_sign(myPRK, hash, 32, sign, uECC_secp256k1());
	std::cout << "file successfully signed..." << std::endl;
	std::cout << "SIGN" << std::endl;


	std::cout << "SIGN IS : ";
	PrintRawBytes(sign, 64);

	std::cout << flrSize << std::endl;
	;
	uint32_t  pushnum = (flrSize / 4) + 20;
	std::cout << pushnum << std::endl;


	unsigned char* CRT = (unsigned char*)malloc(85 + 16 + (pushnum * 4));
	UintToBytes(1, CRT); // set UTXO ID which is 1
	memset(CRT + 4, 0, 77);  // set blank header TX
	UintToBytes(16 + (pushnum * 4), CRT + 81); // ok its smartcontract ptr (8) + all push op (4bytes foreach) + (4o) entry jump +  gas limit (4o) 
	// set Smart Contract pointer to 0 
	memset(CRT + 85, 0, 8);
	uint32_t bOff = 93; // 85 + 8 (bi ti) 
	// copy bitfile

	for (int i = flrSize - 4; i >= 0; i -= 4)
	{
		memcpy(CRT + bOff, fdata + i, 4);
		bOff += 4;
	}


	// copy bitfile size 
	UintToBytes(flrSize, CRT + bOff);  bOff += 4;
	// copy bitfile type 
	UintToBytes(type, CRT + bOff);  bOff += 4;
	// copy bitfile sign

	for (int i = 60; i >= 0; i -= 4) {
		memcpy(CRT + bOff, sign + i, 4);
		bOff += 4;
	}
	// copy  initprice 
	UintToBytes(price, CRT + bOff);  bOff += 4;
	// copy  walletptr 
	UintToBytes(walletptr, CRT + bOff);  bOff += 4;

	// copy  entry which is index 1 
	UintToBytes(1, CRT + bOff);  bOff += 4;
	// put gas limit to 0 
	memset(CRT + bOff, 0, 4);

	// auto set the head mg ---------------------------------------------------->
	//SET_CRT_HEADER(CRT, 85 + 16 + (pushnum * 4), 16 + (pushnum * 4));
	UintToBytes(myUTXO, CRT); // UTXOP
	UintToBytes(nonce, CRT + 68); // TOU AT +68 
	uint32_t ptime = GetTimeStamp() + 50000;
	UintToBytes(ptime, CRT + 72); // ts AT +72 
	UintToBytes(0, CRT + 76); // fee AT +76 
	UintToBytes(2, CRT + 80); // TX TYPE  IS 2 FOR CRT AT +77 
	std::cout << "size " << 85 + 16 + (pushnum * 4) - 85 << std::endl;
	UintToBytes(85 + 16 + (pushnum * 4) - 85, CRT + 81); // datasize  length  AT +78 

	// update also bloc index and tx index is 0 so dont upload and upload max gas 
	UintToBytes(30, CRT + 85); // bloc 30 
	UintToBytes(99999999, CRT + 85 + 16 + (pushnum * 4) - 4); //guser limit 

	int ds = 16 + (pushnum * 4);
	unsigned char* data = (unsigned char*)malloc(21 + ds);
	memcpy(data, CRT, 4);
	std::cout << 68 + 17 + ds << std::endl;
	memcpy(data + 4, CRT + 68, 17 + ds);
	Sha256.init();
	Sha256.write((char*)data, 21 + ds); // hash transaction 
	memcpy(hash, Sha256.result(), 32);
	free(data); // release heap alloc

	if (uECC_sign(myPRK, hash, 32, sign, uECC_secp256k1())) {
		std::cout << "DATA transaction successfully signed" << std::endl;
	}
	// copy sign at +4 
	memcpy(CRT + 4, sign, 64);

	//----------------------------------------------

	std::ostringstream s;
	s.str("");
	char rdname[65];
	GetRandomHashString(rdname);
	s << outputDir << rdname;
	FILE * f = fopen(s.str().c_str(), "ab");
	if (f == NULL) return;
	fwrite(CRT, 1, 85 + 16 + (pushnum * 4), f);
	fclose(f);
	

	free(fdata);
	PrintRawBytes(CRT, 100);
	free(CRT);

}