

          _
        ,/_\,       +                                                 +
      ,/_/ \_\,                         +                                       +
    /_/ ___ \_\
    /_/ |(V)| \_\             +                                                 +
      |  .-.  |                                    _____________
      | / / \ |                +                //              \\           +
      | \ \ / |                                 |  G E N E S I S ||
      |  '-'  |    +                      +     |                ||                    
      '--,-,--'                                 |    C H A I N   ||
         | |                                    \\______________//
         | |              +                                                     +
         | |
         /\|                                                +                              
         \/|
          /\                  +                +                                 +
          \/



# Genesis Blockchain

## Overview

Genesis Blockchain is a **proof-of-work blockchain** for local network. 

It can run **smartcontracts** through a virtual machine similar to x86 microprocessor ( instruction set is reduced for blockchain
purposes). 
A custom assembler is provided, langage syntax is similar to NASM.

Genesis Blockchain parameters are easily tweakable and software have been originally made to experiment mining process.

An _untitled cryptocurrency_ is also implemented, following same dynamics as Bitcoin software (coin halving, mining reward...etc.)

All is written in C-style for low-end hardware compatibility. 

## Build and Run

Genesis was made for Windows OS. You can compile it importing all .cpp and .h files located in /src directory using the IDE of your choice.
A pre-built executable is located in /src/build for windows console app.  
<p align="center">
https://github.com/gggraph/genesis/blob/main/source%20cpp/build/Genesis.exe
</p>
A MONO version which can be used on Linux system can be found at https://github.com/lionelbroye/BIM/blob/master/Genesis%20Chain/build/firstchain.exe. 
It is the first version of Genesis software written in c# .NET frameworks 

An atmega-328 port is currently in development. 

## First step and network configuration

Once your executable is built. Place it in an empty folder, many files will be generated at first start. 
Make sure _net.ini_ file is located at the root directory of the executable. It contains important information about
all your peers IP. Genesis Blockchain only run on local environment even if its design was purposed
to meet larger and public network. You can manually set IP:port for both starting a server and connecting to other
machines, simply by editing the file. It is by default configured to local address 127.0.0.1 for testing purpose.

![alt text](https://github.com/gggraph/genesis/blob/main/git-content/start%20command.png)

Run executable, you can interact typing command. List of all available command are printed to console at start. 
You can first try to run demo program entering **demo** . Create a wallet entering **createwallet** and start mine some blocks with 
newly created public key using **stepmining** 

## About file structure 

The Genesis Blockchain software will create directory and files at its root. Let's see them one by one : 

##### The blockchain files

* /blockchain is a directory containing all _official_ blocks. There are blocks that has been validated and has won the distance rule : 
          they are fully integrated to the blockchain. 
* All blocks are stored and named in an ascending way. /blockchain/0 is a file containing genesis block and all future blocks up to 
  4 294 967 295  bytes ( originally designed to be written on FAT32 system). /blockchain/1 containing all next official blocks ... etc. 
* /blockchain/blocksptr is a file containing data pointing to block files and byte offsets of all official blocks. It is used to easily
  retrieving block data without reading the whole blockchain.  

##### The competitive blocks files 

* /fork is a directory contaning all competitive blocks. Genesis Blockchain _upgrade_ process happened every time the last index of 
  of competitive blocks reached or going above **last official block index + 6** ( 6 is an arbitrary number which can be changed inside
          params.h ).
* During mining process, the software will start craft next block from longest competitive blocks file. Mining strategy is up to you. 
* Competitive blocks are named with the hexadecimal representation of latest block hash in its file. 
          
##### The UTXO Set files 

* /utxos is a directory containing all _wallets_ at the current state of the official blockchain. In the same way for block files, they are named
  in an ascending order. /utxos/0 containing first wallets which has been registered during blocks validation. 
  utxos files are overwritten when upgrading the official blockchain. 
* UTXO stand for Unspend Transaction Output. An Utxo is a data structure
  containing public key of 64b, a sold ( or amount of coin hold by the key) and a token called Token of Uniqueness ( or TOU ) which is burnt
  consciously by the wallet holder at every new transaction to avoid double spending problem. 
          
##### The Temporary storage

* /tmp folder contains all files which have been downloaded server-side. 
* DLL files are both pending transaction data or blocks data waiting to be validated. 
* Files are systematically deleted after their process. So consider them as _cache_ files. 
          
##### The Pending Transactions File

* located at /ptx. This file contains all transaction which has been received by peers or built with command console.
* ECDSA verification is always done to transactions before being appended to this file. 
* When Genesis Software prepare mining, it fills the new block with transaction inside /ptx file. 
* /ptx file is refreshed at every blockchain upgrade, deleting transactions who have old TOU or those who have purished. 

##### The Smart Contract Storage Files

* /sc directory contains all contract storage files. A contract storage file is data which can be written by a smart contract code. Its name is 
  hexadecimal hash representation of the block index and transaction index which contain the contract code.
* A blank contract storage is created **everytime a new CST is validated inside a block** and only when the block is not competitive anymore. 
* As deleting utxos files, deleting contract storage file make blockchain corrupted. utxos directory and sc directory are part of the blockchain 
  current state. 
  
          
## Data structure

Even if the Genesis blockchain solution doesn't treat with object-oriented programming, its blocks, transactions and tokens can be represented as following table : 

##### Block structure

| data name     | byte size     | offset        |
| ------------- | ------------- | ------------- |
| index         | 4             | 0             |
| header hash   | 32            | 4             |
| previous hash | 32            | 36            |
| unix timestamp| 4             | 68            |
| hash target   | 32            | 72            |
| nonce         | 4             | 104           |
| miner token   | 5 or 65       | 108           |
| tx count      | 2             | 113 or 173    |
| transactions  | variable      | 115 or 175    |


##### Miner Token structure

If flag byte is set to 0, data is an utxo identifier. 
If flag byte is set to 1, data is a SECP256K1 public key.

| data name     | byte size     | offset        |
| ------------- | ------------- | ------------- |
| flag byte     | 1             | 0             |
| data          | 4 or 64       | 1             |

##### UTXO structure 

| data name     | byte size     | offset        |
| ------------- | ------------- | ------------- |
| public key    | 32            | 0             |
| T O U         | 4             | 32            |
| Sold          | 4             | 36            |

##### TX Header structure 

| data name     | byte size     | offset        |
| ------------- | ------------- | ------------- |
| utxo id       | 4             | 0             |
| SECP256K1 sign| 64            | 4             |
| Burnt Token   | 4             | 68            |
| Purishment    | 4             | 72            |
| Fee           | 4             | 76            |
| Flag          | 1             | 77            |
| Data size     | 4             | 81            |

##### DFT data structure 

| data name     | byte size     | offset        |
| ------------- | ------------- | ------------- |
| rcv flag      | 1             | 0             |
| amount        | 4             | 1             |
| rcv key       | 4 or 64       | 5             |

##### CST data structure 

| data name     | byte size     | offset        |
| ------------- | ------------- | ------------- |
| entries count | 4             | 0             |
| entries       | 4xentry count | 4             |
| contract code | relative      | relative      |


##### CRT data structure 

| data name     | byte size     | offset        |
| ------------- | ------------- | ------------- |
| block ptr     | 4             | 0             |
| tx ptr        | 4             | 4             |
| push ops.     | relative      | 8             |
| entry jump    | 4             | relative      |
| gas limit     | 4      | relative      |

## Write Decentralized Applications

Now, we will going to technical materials. 
The Genesis Blockchain treat actually with three types of transaction. 
First one is **DFT** (Default Transaction), which is a basic exchange of valuable data between two wallets. 
Second is called **CST** (Contract Submission Transaction), this transaction contains a series of instructions for the Genesis Virtual Machine. 
A CST has a generic header transaction data (see TX header structure table) but its specific data is a number of entries pointing to **bytecodes** in vm memory. When a CST is validated inside a block, its whole list of instructions (the smartcontract) can be called by the following type of transaction during the whole blockchain lifetime and by any users:
A **CRT** ( Contract Request Transaction ). CRT is a transaction that run a smartcontract code at specific offset. Technically, it will load the compiled smart contract from a given block, push some data to the virtual machine stack if necessary, then perform a jump at a given memory address. Just thinking, CRT fit x86 calling conventions. 

To both write CRT and CST for the Genesis Blockchain, run GenesisExplorer.exe (which can be downloaded at /src/build). It looks like this:  

![alt text](https://github.com/gggraph/genesis/blob/main/git-content/contracteditor.png)

### Write a simple CST code

You can write your contract assembly code inside left textbox element (the big one). 
Just to make things clear, the _genesis vm_ can work with 8086 original instruction set. It has all general registers and a flag register which is not fully implemented yet (only ZF is used) . 
Smart Contract is loaded at **0x5D memory offset of the virtual machine**. VM memory allocation size is limited to **32kb**. Stack going downward. Registers depth is 32-bit.
Genesis vm acts as a CISC microprocessor. You will meet again MOD-REG-RM, SIB mode, direct and indirect addressing, 8-bit or 32-bit displacement... etc.
The syntax to follow to convert your code to binary is similar to nasm x86 code. 
**Last but not least, the instruction set is limited to blockchain usage. VM can only read and write data within its memory or its contract storage file. 
Pseudo-randomness, listening ports or anything from the outside cannot be achieved.** 

So let's start with a basic example code : 

```
          var1 DD 20
          var2 DD 0
          
          start:
          MOV EAX, [var1]
          MOV EBX, 8
          ADD EAX, EBX
          MOV dword[var1+4], EAX
          CMP dword[var2], 28
          JE  END
          MOV ECX, 1
          JMP start
          
          end:
          HLT

```
This asm code will :
* define two 32-bit unsigned integer at its start. One called var1, the other var2.
* copy 4 bytes from var1 memory address to register A
* copy constant 8 to register B
* adding register B value to register A
* copy  register A to memory address of var1 + 4 wich is actually the memory address of var2 ! 
* compare var2 value to 28
* then jump to label end if equal
* halt the vm 

Going further with a basic call and stack mechanism : 

          start:
          MOV  EAX, 16
          SHL  EAX, 1
          PUSH EAX
          CALL func
          XOR ECX, ECX
          INC ECX
          HLT
          
          func:
          PUSH EBP
          MOV EBP,ESP
          MOV EDX, [EBP+8]
          POP EBP
          RETP 4
          
This asm code will :
* set register A value to 16
* left shift register A data, so multiplying itself by 2
* push 4 bytes of register A content to the stack
* jump to func label address, pushing EIP value 
* moving pshed eax content in the stack to register D value
* return and increment stack pointer by 4 
* set register C value to 0
* increment C value by 1
* halt the vm 

There is specific instructions related to blockchain contract storage. Here is an example of writing to the storage file using OPCODE **STF**

          func:
          PUSH EBP
          MOV  EBP, ESP
          XOR  EAX, EAX
          MOV  ECX, 1
          MOV  EDX, EBP
          ADD  EDX, 4
          STAD4
          HLT
          
this code snippet, once being called by a **CRT**, will write 4 bytes contained at the top of the stack, at the begginning of the storage file.
For further understanding, read the long switch statement inside _vm.cpp_

### Defining entries

To make your contract interactable during the blockchain lifetime, you will have to define some entries to contract bytecodes. 
Put a # hastage and the label name at the end of your contract to make this label callable by user in future block transactions. 
Here is an example : 

          label1:
          HLT
          
          label2:
          PUSH EBP
          MOV EBP, ESP
          MOV EAX, [EBP+4]
          HLT
          
          #label1
          #label2

the _label1_ label will be callable but will lead to halting opcode. the _label2_ label will be also callable moving the last pushed data to register A. 
The generated **CST** will propose two entries.

You can memory dump your compiled asm code by clicking on **Dump** button to see raw byte code and their bits sequence. Also check for errors. 
**Output the CST file by clicking on Create CST button.** 

### Write a simple CRT code

Once you have a ready-to-use CST. **You can write a contract request transaction inside the bottom-right text box.**

Writing CRT is straight forward:

* Define the block index wich contains the contract using ### three hashtags. 
* Define the transaction index wich contains the contract using ## two hashtags.
* Define the entry index using # one hashtag.
* Everything else that can be read as number by the software will become values to push to the stack.   

          ### 10
          ##  0
          #   1
          
          18
          
If block number 10 of the blockchain contains CST of the last paragraph at its first transaction. Those instructions will push 18 to the vm stack then 
call _label2_ label, so put 18 to reg A. 

**Output the CST file by clicking on Create CST button.** 

### Testing a contract

Once you have both a CST file and a related CRT file, you can run contract using _testcontract_ command in genesis.exe. 
It will output registers value to the console. 
You can also mine your CST and CRTs, but it is currently work in progress. To experiment it, call AddTransactionToPTXFile(), **BUT YOU 
WILL HAVE TO CRAFT BY YOURSELFT THE TRANSACTION HEADER DATA** because Genesisexplorer.exe cannot do it ! 
List of OPCODES can be read at vmopcode1.ini. Don't modify this file or CST binary conversion will not work properly! 

**[READ MORE ABOUT CONTRACT ASSEMBLY](https://github.com/gggraph/Genesis/blob/main/contract-example)**



# Further sight

## Source files

| Bloc.h        | 
| ------------- |

Header file to access block data, either from official or from fork files. Most of the methods output raw unsigned char pointer to block data such as its hash or 
one of its transaction... etc. 

| Mine.h        | 
| ------------- |

Header file to access mining function. It's there where the software will prepare and hash new blocks. 

| UI.h          | 
| ------------- |

Console commands main thread and basic graphic interface. 

| Utils.h       | 
| ------------- |

Miscellaneous methods to convert byte array to integer. writing, reading, deleting files. Get/set bit in a byte... etc.

| arith256.h    | 
| ------------- |

Header file to manipulate 256-bit integer. It contains only compare, right shift, add and multiply function according to proof-of-work necessity.  

| consensus.h   | 
| ------------- |

Main block validation method. ProcessBlocks() method will start validating blocks file from qfiles (see NetfileManager.cpp) 
Contains also hash target computation, mining reward computation, timestamp validation. It is the core of the consensus algorithm. 

| params.h      | 
| ------------- |

Where all proof-of-work related constants are stored.

| tx.h          | 
| ------------- |

Header file to get transaction data inside a block, verify signature, craft new keys and all transaction related code such as verify contract returning
gas used... 
It also provide pending transactions file manipulation methods. 

| utxo.h        | 
| ------------- |

Header file containing wallet related function. You can get utxo data from there, virtualize wallet at a specific instant of 
the blockchain time. Mainly use when verifying block transaction. 

| vm.h          | 
| ------------- |

Genesis virtual machine load contract and run methods. 

| NetfileManager.h | 
| ---------------  |

Header file that contains list of file data waiting to be proccessed during the main loop. 

| NetServer.h      | 
| ---------------  |

Start server method and download handler. 

| NetClient.h      | 
| ---------------  |

Connecting to peers and all broadcasting methods. 

| Genesis.h        | 
| ---------------  |

Program entry. Main thread method. Genesis block creation and blockchain initialization methods. 

**about sha256.h and uECC.h** 

Those files are cryptography algorithm used by the Genesis blockchain. **This is not my work!** : 
elliptic curve signature : 
https://github.com/kmackay/micro-ecc
sha256 : 
https://code.google.com/p/cryptosuite/

## Calling method for blockchain monitoring

### Get Block data

To read a block of the blockchain. You will first need to load blocks pointer in memory, using  
```cpp 
          LoadBlockPointers()
```
Because blocks size are not constant, you will have to do some heap allocation to store the block. 
Prepare a byte pointer and call GetOfficialBlock() method to load the block. Don't forget to deallocate it after the needs. 
```cpp          
          // Get Genesis block (block #0)
          unsigned char * block = GetOfficialBlock(0); 
          /* do what you want here */
          free(block); 
```
Once you have a block pointer, you can use it to get its specific data. See bloc.h or use bytes table declared above. 

```cpp          
          // Get Genesis block (block #0)
          unsigned char * block = GetOfficialBlock(0); 
          // get nonce of the block
          uint32_t nonce = GetBlockNonce(block);
          // Get its hash. 2 ways. 
          unsigned char hash[32]; 
          memcpy(hash, GetBlockHash(block), 32); 
          // or 
          memcpy(hash, block + 4, 32); 
          // print hex representation
          printHash(hash); 
          // fast way to print previous block hash
          printHash(block+36);
          free(block); 
```
You can also retrieve transaction data from block pointer. Make sure block has a transaction : 
```cpp          
          // Get latest block
          uint32_t index = GetLatestBlockIndex(true); 
          unsigned char * block = GetOfficialBlock(index); 
          // make sure block has a transaction
          if (  GetTransactionNumber(block) > 0 ) {
                // Get first Transaction
                unsigned char * TX = GetBlockTransaction(block, 0); 
                // fast method to print transaction data. 
                PrintTransaction( TX ) ; 
                // Because transaction is a pointer to block memory. There is not extra 
                // memory allocation. 
          }
          free(block); 
```
### Tweaking Proof of work constants

All _proof of work_ constant value are inside params.h
Let's have a look at some of them : 

```cpp 
          #define TARGET_CLOCK			42 
          #define TARGET_TIME			128
          #define POW_TARGET_TIME               7
          #define TARGET_FACTOR			4
```
TARGET_CLOCK is a tick of the hash target update. If you change it to 2016, every 2016 blocks, hash target will change. 

TARGET_TIME is a human way to see number of seconds spent between two blocks creation. Mine a block will always lasts something closed to 128 seconds. 
Yet this constant has no effect, the real value which will impact block creation time is POW_TARGET_TIME which **has to be a power of 2**. 
2 ^ 7 is 128. 

For improved time complexity of hash target computation, logicial right shift is used for dividing hash target with wanted target_time. 

TARGET_FACTOR is boundaries to new hash target value. If set to 8, new hash target will never be more than 8 times higher or less than 8 times lower the preceding target.

Changing thoses variables and the GENESIS_TARGET constant values (located at genesis.cpp ) will let you experiment hashcash mechanism. 
