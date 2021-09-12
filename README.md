

          _
        ,/_\,
      ,/_/ \_\,
     /_/ ___ \_\
    /_/ |(V)| \_\
      |  .-.  |                                    _____________
      | / / \ |                                 //              \\
      | \ \ / |                                 |  G E N E S I S ||
      |  '-'  |                                 |                ||
      '--,-,--'                                 |    C H A I N   ||
         | |                                    \\______________//
         | |
         | |
         /\|
         \/|
          /\
          \/



# Genesis Blockchain

## Overview

Genesis Blockchain is a _proof-of-work blockchain_ for local network. 

It can run _smartcontracts_ through a virtual machine similar to x86 microprocessor ( instruction set is reduced for blockchain
purposes). 
A custom assembler is provided, langage syntax is similar to NASM.

Genesis Blockchain parameters is easily tweakable and has been originally made to experiment time complexity of hashcash 
protocol.
An _untitled cryptocurrency_ is also implemented, following same dynamics as Bitcoin software(coin halving...etc.)

All is written in C-style for low-end hardware compatibility. 

## Run it

Genesis was made for Windows OS. You can compile it importing all .cpp and .h files located in /src directory using the IDE of your choice.
A pre-built executable is located in /src/build for console app.  
A MONO version which can be used on Linux system can be found at . 
An atmega-328 port is currently in developement. 

## First step and network configuration

Once your executable is built. Place it in an empty folder, many files will be generated at first start. 
Make sure _net.ini_ file is located at the root directory of the executable. It contains important information about
all your peers IP and port. Genesis Blockchain only run on local environment even if its design was purposed
to meet larger and public network. You can manually set IP:port for both starting a server and connecting to other
machine, simply by editing the file. 

## Write Smart Contract

Now, we will going to technical material. 
The Genesis Blockchain treat actually with three types of transaction. 
First one is DFT (Default Transaction), which is a basic exchange of coin between two wallets. 
Second is called CST (Contract Submission Transaction), this transaction contains a series of instructions for the Genesis Virtual Machine. 
A CST has a generic header transaction data (public key pointer, secp256k1 signature, a t.o.u. and a purishmment time) but his specific data is a number of entries pointing to memory address and a long list of bytecode. When a CST is validated inside a block, its whole list of instructions can be called by the following type of transaction during the whole blockchain lifetime by any user:
A CRT ( Contract Request Transaction ) is a transaction that run a smartcontract code at specific offset. Technically, it will load the compiled smart contract, push some data to the virtual machine stack if necessary, then perform a jump at a given memory address. Just thinking a CRT is like calling a function in a piece of code somewhere in the blockchain.  

To both write CRT and CST for the Genesis Blockchain, run GenesisExplorer.exe (which can be downloaded at /src/build). It looks like this:  
![alt text](https://raw.githubusercontent.com/gggraph/genesis/main/TRANSACTION%20VIEWER%20B.png?token=AO6VU22AXZLF4AVNTDRAQ3TBHZQ2G)
