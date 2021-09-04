#pragma once
#include "Utils.h"
// -_-_-_-_-_-_-_-_-_-_-_-_-_-_- CONSENSUS PARAMETERS -_-_-_-_-_-_-_-_-_-_-_-_- \\

#define RUN_DISTANCE			6
#define TARGET_CLOCK			42 //2106
#define TIMESTAMP_TARGET		11
#define TARGET_TIME				128//512 // HAS TO BE A POWER OF 2 FOR BITSHIFTING. IS 8MINUTE AND HALF APPROX. UNUSED - just for readable purpose
#define POW_TARGET_TIME         7//9   // THE TARGET TIME AS POWER OF 2. HERE IS 2^9. THIS IS WHAT IS ACTUALLY USED. 
#define TARGET_FACTOR			4
#define FIRST_UNIX_TIMESTAMP    1598981949
#define NATIVE_REWARD		    50
#define HALVING_CLOCK		    210000
#define MAX_TIME_UP		        7200

#define MAX_GAS_SIZE            12500000
#define MAX_CONTRACT_SIZE       16000

