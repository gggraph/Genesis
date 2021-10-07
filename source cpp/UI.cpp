
#include "UI.h"

void SayHello() 
{
	std::cout << "          _												" << std::endl;
	std::cout << "        ,/_\\,											" << std::endl;
	std::cout << "      ,/_/ \\_\\,											" << std::endl;
	std::cout << "     /_/ ___ \\_\\										" << std::endl;
	std::cout << "    /_/ |(V)| \\_\\										" << std::endl;
	std::cout << "      |  .-.  |					   _____________					" << std::endl;
	std::cout << "      | / / \\ |					//              \\\\					" << std::endl;
	std::cout << "      | \\ \\ / |					|  W E L C O M E || 					" << std::endl;
	std::cout << "      |  '-'  |					|  B A C K   T O ||			" << std::endl;
	std::cout << "      '--,-,--'					|  G E N E S I S ||				" << std::endl;
	std::cout << "         | |					\\\\______________//						" << std::endl;
	std::cout << "         | |												" << std::endl;
	std::cout << "         | |												" << std::endl;
	std::cout << "         /\\|												" << std::endl;
	std::cout << "         \\/|												" << std::endl;
	std::cout << "          /\\												" << std::endl;
	std::cout << "          \\/												" << std::endl;
}
void GetCommand() 
{

	while (true) 
	{
		char cmd[255];
		
		std::cout << "[Type a command]" << std::endl;
		std::cin.getline(cmd, 256);

		if (strstr(cmd, "help") != NULL) {
			
			PrintCommandList();
			continue;
		}
		if (strstr(cmd, "demo") != NULL) {
			if (YerOrNo("[WARNING] This will erase entire blockchain files. Continue?")) {
				Demo();
			}
			
			continue;
		}
		if (strstr(cmd, "testcontract") != NULL) {
			char fpath[255];
			std::cout << "Type path of a contract submission transaction file :" << std::endl;
			std::cin.getline(fpath, 256);

			
			FILE* f = fopen(fpath, "rb");
			if (f == NULL) { continue; } // throw error if cannot read
			fseek(f, 0, SEEK_END);
			uint32_t lSize = ftell(f);
			rewind(f);
			unsigned char* tContract = (unsigned char*)malloc(lSize);
			uint32_t cSize = lSize;
			fread(tContract, 1, lSize, f);
			fclose(f);

			

			std::cout << "Type path of a contract request transaction file :" << std::endl;
			std::cin.getline(fpath, 256);

			f = fopen(fpath, "rb");
			if (f == NULL) { continue; } // throw error if cannot read
			fseek(f, 0, SEEK_END);
			lSize = ftell(f);
			rewind(f);
			unsigned char* tRequest = (unsigned char*)malloc(lSize);
			uint32_t rSize = lSize;
			fread(tRequest, 1, lSize, f);
			fclose(f);

			TestContract(tContract, cSize, tRequest, rSize);

			free(tContract);
			free(tRequest);
			continue;
		}
		if (strstr(cmd, "stepmining") != NULL) {
			char argstr[20];
			uint32_t utxop = 0;
			unsigned char pukey[64];

			if (YerOrNo("Do you have a UTXO identifier? ")) {
				std::cin.getline(argstr, 21);
				utxop = atoi(argstr);
			}
			else {
				std::cout << "Type path of your public Key :" << std::endl;
				char fpath[255];
				std::cin.getline(fpath, 256);
				ReadFile(fpath, 0, 64, pukey); // fill prkey buffer 
			}
			std::cout << "Type number of blocks to mine :" << std::endl;
			std::cin.getline(argstr, 21);
			int n = atoi(argstr);
			LoadBlockPointers();
			char wblockpath[255];
			
			for (int i = 0; i < n; i++) {
				remove("ptx");
				Mine(pukey, 5000, utxop, wblockpath);
			//	getchar();
				ProccessBlocksFile(wblockpath);
			//	getchar();
			}
			
			
			continue;
		}
		if (strstr(cmd, "initchain") != NULL) {
			if (YerOrNo("[WARNING] This will erase entire blockchain files. Continue?")) {
				InitChain();
				VerifyFiles();
				LoadBlockPointers();
			}
			
			continue;
		}
		if (strstr(cmd, "disconnect") != NULL) {
			StopServer();
			DisconnectAll();
			continue;
		}

		if (strstr(cmd, "connect") != NULL) {
			StartServer("net.ini");
			ConnectToPeerList("net.ini");
			continue;
		}
		
		if (strstr(cmd, "createwallet") != NULL) {
			MakeSECP256K1PairKeys();
			continue;
		}
		if (strstr(cmd, "getutxoptr") != NULL) {
			std::cout << "Type path of your public Key :" << std::endl;
			char fpath[255];
			std::cin.getline(fpath, 256);
			unsigned char pukey[64]; //  pukey
			ReadFile(fpath, 0, 64, pukey); // fill pukey buffer 
			uint32_t utxop = GetUtxoPointer(pukey);
			if (utxop > 0) {
				std::cout << "Your UTXO identifier is " << utxop  << std::endl;
			}
			else {
				std::cout << "UTXO was not found." << std::endl;
			}
			continue;
		}
		if (strstr(cmd, "printwallet") != NULL) {
			if (YerOrNo("Do you have the UTXO identifier?")) {
				
				std::cout << "Type UTXO identifier :" << std::endl;
				char utxopstr[20];
				std::cin.getline(utxopstr, 21);
				int utxop = atoi(utxopstr);
				if (utxop != 0) {
					unsigned char UTXO[72];
					GetUtxo(utxop, UTXO);
					PrintUTXO(UTXO);
				}
				else
				{
					std::cout << "UTXO was not found." << std::endl;
				}
			}
			else {
				std::cout << "Type path of your public Key :" << std::endl;
				char fpath[255];
				std::cin.getline(fpath, 256);
				unsigned char pukey[64]; //  pukey
				ReadFile(fpath, 0, 64, pukey); // fill pukey buffer 
				uint32_t utxop = GetUtxoPointer(pukey);
				if (utxop != 0) {
					unsigned char UTXO[72]; 
					GetUtxo(utxop, UTXO);
					PrintUTXO(UTXO);
				}
				else 
				{
					std::cout << "UTXO was not found." << std::endl;
				}
			}
			continue;
		}
		if (strstr(cmd, "getblock") != NULL) {
			LoadBlockPointers();
			std::cout << "Type block index : " << std::endl;
			char utxopstr[20];
			std::cin.getline(utxopstr, 21);
			uint32_t index = atoi(utxopstr);
			uint32_t lbi = GetLatestBlockIndex(true);
			
			if (index < lbi )
			{
				unsigned char* b = GetOfficialBlock(index);
				PrintBlockInfo(b);
				free(b);
			}
			else
			{
				unsigned char* b = GetOfficialBlock(lbi);
				PrintBlockInfo(b);
				free(b);
				std::cout << "Index is too high. Here is the latest block. Check blockchain general info with getbcinfo command." << std::endl;
			}
			continue;
		}
		if (strstr(cmd, "getbcinfo") != NULL) {
			LoadBlockPointers();
			std::cout << "-_-_-_-_-_-_-_-_- Blockchain Info_-_-_-_-_-_-_-" << std::endl;;
			std::cout << std::endl;
			std::cout << "BLOCK CREATION CLOCK			" << TARGET_TIME << std::endl;
			std::cout << "HASHTARGET CLOCK			" << TARGET_CLOCK << std::endl;
			std::cout << "HASHTARGET FACTOR			" << TARGET_FACTOR << std::endl;
			std::cout << "TIMESTAMP MEDIANE			" << TIMESTAMP_TARGET << std::endl;
			std::cout << "GENESIS TIMESTAMP			" << FIRST_UNIX_TIMESTAMP << std::endl;
			std::cout << "NATIVE REWARD				" << NATIVE_REWARD << std::endl;
			std::cout << "HALVING CLOCK				" << HALVING_CLOCK << std::endl;
			std::cout << "MAX FORK DISTANCE			" << RUN_DISTANCE << std::endl;
			std::cout << "NETWORK TIME TOLERANCE			" << MAX_TIME_UP << std::endl;
			std::cout << "CONTRACT GAS PER BLOCK			" << MAX_GAS_SIZE << std::endl;
			std::cout << "MAX CONTRACT SIZE			" << MAX_CONTRACT_SIZE << std::endl;
			
			std::cout << std::endl;
			getchar();
			uint32_t lbi = GetLatestBlockIndex(true);
			std::cout <<  "BLOCKCHAIN CURRENT HEIGHT IS " << lbi << std::endl;
			std::cout << std::endl;
			getchar();
			unsigned char* b = GetOfficialBlock(0);
			PrintBlockInfo(b);
			free(b);
			b = GetOfficialBlock(lbi);
			PrintBlockInfo(b);
			free(b);
			std::cout << "-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-" << std::endl;
			continue;
		}

		if (strstr(cmd, "createtx") != NULL) {
			//CreateDefaultTransaction(prkey, utxop, GetUtxoTOU(utxobuff) + 1, 5000, 50, 8, 0, rdpukey);

			std::cout << "Type your UTXO identifier (use getutxoptr if you don't know ): " << std::endl;
			char argstr[20];
			std::cin.getline(argstr, 21);
			uint32_t utxop = atoi(argstr);

			unsigned char utxo[72]; 
			GetUtxo(utxop, utxo);

			std::cout << "Here is your last updated wallet information : " << std::endl;
			PrintUTXO(utxo);

			std::cout << "Type path of your private Key :" << std::endl;
			char fpath[255];
			std::cin.getline(fpath, 256);
			unsigned char prkey[32]; //  prkey
			ReadFile(fpath, 0, 32, prkey); // fill prkey buffer 
			uint32_t rutxop = 0; 

			unsigned char rcvpukey[64]; //  pukey
			if (YerOrNo("Has the receiver a UTXO identifier?  ")) {
				std::cout << "Type receiver key identifier: " << std::endl;
				char argstr[20];
				std::cin.getline(argstr, 21);
				rutxop = 0; atoi(argstr);
			}
			else {
				std::cout << "Type path of receiver Key :" << std::endl;
				std::cin.getline(fpath, 256);
				ReadFile(fpath, 0, 64, rcvpukey); // fill pukey buffer 
			}
			std::cout << "Type Token to burn: " << std::endl;
			std::cin.getline(argstr, 21);
			uint32_t nTOU = atoi(argstr);

			std::cout << "Type Amount: " << std::endl;
			std::cin.getline(argstr, 21);
			uint32_t amount = atoi(argstr);

			std::cout << "Type Fee: " << std::endl;
			std::cin.getline(argstr, 21);
			uint32_t fee = atoi(argstr);

			std::cout << "Type purishment time: " << std::endl;
			std::cin.getline(argstr, 21);
			uint32_t purishment = atoi(argstr);

			if (rutxop > 0) {
				CreateDefaultTransaction(prkey, utxop, nTOU, purishment, fee, amount, rutxop, NULL);
			}
			else {
				CreateDefaultTransaction(prkey, utxop, nTOU, purishment, fee, amount, 0, rcvpukey);
			}

			continue;
		}
	}
	


}

void PrintCommandList() 
{

	std::cout << "-_-_-_-_-_-_-_-_-_List Of Command available_-_-_-_-_-_-_-" << std::endl;
	std::cout << std::endl;
	std::cout << "help   						> Print this. " << std::endl;
	std::cout << "demo   						> Run demo program. " << std::endl;
	std::cout << "run   						> Run  core program. " << std::endl;
	std::cout << "stop   						> Stop core program. " << std::endl;

	std::cout << "testcontract					> Run Genesis Virtual Machine with a contract request. " << std::endl;
	std::cout << "stepmining				        > Mine blocks in step-by-step mode. " << std::endl;

	std::cout << "initchain					> Delete all files and initialize blockchain from genesis." << std::endl;
	std::cout << "connect						> Start server and connect to all peers inside local network. " << std::endl;
	std::cout << "disconnect					> Disconnect to all peers. Shut off server." << std::endl;
	std::cout << "createwallet					> Create SECP256K1 pair keys ." << std::endl;
	std::cout << "getutxoptr					> Get  wallet number from public key ." << std::endl;
	std::cout << "printwallet					> Print wallet information from public key or wallet number." << std::endl;
	std::cout << "getblock					> Print block information from index." << std::endl;
	std::cout << "getbcinfo				        > Print blockchain general data." << std::endl;
	std::cout << "createtx				        > Run TX Helper to create a new transaction." << std::endl;
	std::cout << std::endl;
	std::cout << "-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-" << std::endl;
}
bool YerOrNo(const char * question) 
{
	std::cout << question << "[y/n]" << std::endl;
	while (true) {
		if (GetKeyState('Y') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
		{
			return true;
		}
		if (GetKeyState('N') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
		{
			return false;
		}
	}
	return false;
}