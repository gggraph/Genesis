
#include "Utils.h"

#define MEM_SIZE 32000 

unsigned char MEM[MEM_SIZE];
/*
		*
		   MEM STRUCTURE :

		   0x00           EAX
		   0x04           EBX
		   0x08           ECX
		   0x0C           EDX
		   0x10           ESP
		   0x14           EBP
		   0x18           ESI
		   0x1C           EDI
		   0x20           EIP
		   0x24           EFLAGS
		   0x25           SC CODE
				   ...
		   0x?            STACK

	   */
const char REGmat[] =
{
	0x00, 0x02, 0x00, // A
	0x08, 0x0A, 0x08, // C
	0x0C, 0x0E, 0x0C, // D
	0x04, 0x06, 0x04, // B
	0x03, 0x12, 0x10, // ah, SP
	0x0B, 0x16, 0x14, // ch, BP
	0x0F, 0x1A, 0x18, // dh , SI
	0x07, 0x1E, 0x1C  // bh, DI

};


const char SIBID []=
{
	0x00, //EAX
	0x08, //ECX
	0x0C, //EDX
	0x04, //EBX
	0xFF, //illegal
	0x14, //EBP
	0x18, //ESI
	0x1C  //EDI
};

/* NOT COMPLETE */
const char OPMAP[] =
{
	1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

};

bool InitVM(unsigned char * cs, uint32_t length)
{
	// Init VM with CS Code
	// EIP (0X21) is initialize at 0x25
	*(MEM + 0X20) = 0x25;
	// ESP is initialize at 0x25 + code length
	UintToBytes(0x25 + length, MEM + 0x10);
	// Copy code
	memcmp(MEM + 0x25, cs, length);
	return true;
}

bool Run()
{
	while (1)
	{
		
		uint32_t bl;
		bl = 2; // instruction size. add 1 if constant or sib or disp. add 2 if sib+disp
		// [0] first check prefix offset
		// [1] Get OPCODE
		unsigned char  OPCODE  = *(MEM + *(MEM + 0x20));
		// [1b] 
		// depending of opcode jump
		// [2] GET OPCODE info
		bool imm     = (IsBitSet(0, OPCODE));
		bool d       = (IsBitSet(6, OPCODE));
		bool s       = (IsBitSet(7, OPCODE));
		// [1] GET MODREGXM
		unsigned char MODREGXM = *(MEM + *(MEM + 0x20) + 1 );

		unsigned char REG    = 0;
		unsigned char MOD    = 0;
		unsigned char RM     = 0;
		// copying bits
		SetBit(7, IsBitSet(1, MODREGXM), &MOD);
		SetBit(6, IsBitSet(0, MODREGXM), &MOD);

		SetBit(7, IsBitSet(4, MODREGXM), &REG);
		SetBit(6, IsBitSet(3, MODREGXM), &REG);
		SetBit(5, IsBitSet(2, MODREGXM), &REG);

		SetBit(7, IsBitSet(7, MODREGXM), &RM);
		SetBit(6, IsBitSet(6, MODREGXM), &RM);
		SetBit(5, IsBitSet(5, MODREGXM), &RM);

		unsigned char * REGADDR, *  RMADDR;
		// 16bit mode always false if not 0x66 prefix
		REGADDR =  GetREGaddr(REG, s, false);  
		RMADDR  =  GetREGaddr(RM , s, false);
		
	    // CHECK SIB MODE OR DISPONLY MOD
		unsigned char  mod = 0;
		// copying bits
		SetBit(7, IsBitSet(1, MODREGXM), (unsigned char *)&mod);
		SetBit(6, IsBitSet(0, MODREGXM), (unsigned char *)&mod);

		if ( mod == 0 )
		{
			
			// SIB MODE ( not called CISC for nothing ...) 
			if (RM == 5) 
			{
				bl++;
				// we update R/M with SIB MODE
				unsigned char SIB = *(MEM + *(MEM + 0x20) + 2);
				unsigned char scale = 0;
				unsigned char index = 0; 
				unsigned char base  = 0;

				SetBit(7, IsBitSet(1, SIB), &scale);
				SetBit(6, IsBitSet(0, SIB), &scale);

				if (scale == 3)
					scale = 4;
				if (scale == 4)
					scale = 8;

				unsigned char * BASEADDR, *INDEXADDR;

				SetBit(7, IsBitSet(4, SIB), &index);
				SetBit(6, IsBitSet(3, SIB), &index);
				SetBit(5, IsBitSet(2, SIB), &index);

				if (index == 5) // illegal...
					return;

				
				INDEXADDR = MEM + SIBID[index];

				SetBit(7, IsBitSet(7, SIB), &base);
				SetBit(6, IsBitSet(6, SIB), &base);
				SetBit(5, IsBitSet(5, SIB), &base);


				if (base == 5) // disp only [ SIB MODE ]
				{
					int disp32 = *(MEM + *(MEM + 0x20) + 2);
					BASEADDR = MEM + disp32;
					bl += 4;
				}
				else
				{
					BASEADDR = MEM + SIBID[index];
				}
				
				RMADDR = (unsigned char * ) *(BASEADDR)+ (*(INDEXADDR) * scale);

			}
			// 32bit DISP ONLY
			else if (RM == 6)
			{
			//	RMADDR = (unsigned char *) 
				int disp32 = *(MEM + *(MEM + 0x20) + 2);
				bl += 4;
				RMADDR = MEM + disp32;
			}
			// INDIRECT
			else 
			{
				RMADDR = MEM + *(RMADDR); // MEM + LA VALEUR CONTENUE A RM ADDR
			}
			// SIB MOD

			
		}
		if (mod == 1)
		{
			// indirect + disp 8 
			char disp8 = *(MEM + *(MEM + 0x20) + 2);
			bl++;
			RMADDR = (unsigned char * ) *(RMADDR) + disp8;
		}
		if (mod == 2)
		{
			// indirect + disp 32 
			int disp32 = *(MEM + *(MEM + 0x20) + 2);
			bl += 4;
			RMADDR = (unsigned char *) *(RMADDR)+disp32;
		}
		unsigned char * addr1, *addr2;
		

		if (d) 
		{
			// dest : REG, source : R/M
			addr1 = REGADDR;
			addr2 = RMADDR ;
		}
		else
		{
			// dest : R/M, source : REG
			addr2 = REGADDR;
			addr1 = RMADDR;
		}

		// imm mode 
		int immVal; // usefull if immediate mode
		if ( imm )
		{

			if ( (s && d == 0)  || !s) 
			{
				char  imm8 = *(MEM + *(MEM + 0x20) + 2);
				immVal = (int)imm8;
			}
			else 
			{
				int   imm32 = *(MEM + *(MEM + 0x20) + 2);
				// short imm16 = *(MEM + *(MEM + 0x20) + 2); no 16bit mode
				immVal = imm32;
			}
			
		}
		// OPERAND TYPE IS :
		// /0
		// MEM MEM
		// MEM IMM
		// MEM
		// IMM
		

		// RUN INSTRUCTION ( BY OPCODE MAP (16*16 mat) )( and get INSTRUCTION IDENTIFIER ) 
		char OPID = OPMAP[OPCODE];

		switch (OPID)
		{
			case 1: // ADD
				if (imm)
					*(addr1) += immVal;
				else
					*(addr1) += *(addr2);
			break; 
			case 2: // MOV
				if (imm)
					*(addr1) = immVal;
				else
					*(addr1) = *(addr2);
			break;
		}
		// update EIP
		*(MEM + 0x20) += bl;
	}
	return true;
}

unsigned char *  GetREGaddr(unsigned char val, bool s, bool b16) 
{

	int x,y;
	x = val * 3;
	if (!s)
		y = 0;
	else {
		if (b16)
			y = 1;
		else
			y = 2;
	}
	return MEM + REGmat[x + y];

}

void PrintReg()
{

	std::cout << "EAX : " << *(MEM + 0x00) << std::endl;
	std::cout << "EBX : " << *(MEM + 0x04) << std::endl;
	std::cout << "ECX : " << *(MEM + 0x08) << std::endl;
	std::cout << "EDX : " << *(MEM + 0x0C) << std::endl;

}
