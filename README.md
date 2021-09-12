

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



# The Genesis Blockchain

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
A pre-built executable is located in /build for console app.  
A MONO version which can be used on Linux system can be found at . 
An atmega-328 port is currently in developement. 
