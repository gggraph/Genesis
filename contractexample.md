# Getting deeper inside DAPP writing

## Overview

To make more understandable the proccess of writing a smartcontract and the functionnalities and limitations of the Genesis VM, 
we will comment step by step a light smart contract which acts as an art market. 

The contract will be called BIMMARKET. Here the promises:

- Everyone can get registered inside the art market, **every account can hold up to 100 arts.** If they want more, they can have multi-account.
  
- Every files can be written inside the blockchain with no size limit ( it can be either a bmp, pdf or a binary it doesnt matter!). A file registered
  inside the contract storage will be called art. **If the file size is more than 900kb it should be split to multiple chunks** because of the vm allocated 
  memory limitation which is roughly 1MB. If you want more memory for the VM, change MEM_SIZE constant inside vm.cpp
  
- Every art inside storage has a **price, a signature from the artist, a creation timestamp,** a market mode flag and so on... 

- Every art holder can update price of its art every week, and cannot go more than 4x its current price. This is like a basic market regulation. 

- To obtain property of an art data, there will be actually 3 ways. It will depend of market mode flag of the art:  
  The **private market** mod will make **arts buyable only with permissions of current holder**. Theirs SECP256K1 signatures is needed during CRT creation proccess. 
  
  The **savage market** mod will make **arts buyable at its current price without any permission**. Every registered account can obtain the art at any time if they   have a suffisant sold. 
  
  The **bid market** mod will let accounts compete against each other to obtain the art property during a laps of time decided by the holder. When the bid end, 
  the art will be obtainable by the highest bidder. The holder can also set a minimum price. 
  
 - Price of art is in native cryptocurrency. This is just a simple 1:1 ratio. An art priced as 100 will cost to the buyer 100 native crypto-currency.

**Last but not least, the contract will be 100% written in Genesis contract assembly langage with the use of GenesisExplorer.exe. The block validation protocol of Genesis software will not be changed. It will just be a series of bytecodes inside a transaction somewhere in the blockchain!**

## Account registration

## Iterate through account data

## Art registration

## Contract reversibility

Because this is never a good idea to prevent validation of a downloaded block series where block height starts lower than node's current highest official block, **a blockchain software should always have a way to downgrade its actuality to a previous state.**  

Where default BTC transaction cancelling is straightforward, lowering the amount of the receiver sold and adding back the amount to sender, so basically just negating the amount value, reversing machine code instructions could be a lot more difficult! For this, Genesis virtual machine has actually two mods , once enabled, machine instructions will act differently. Those mods can only be SET or UNSET during a downgrading proccess with OPCODES **REVN** and **REVD** . 

**REVN** will SET/UNSET **REV NORMAL MOD**. If this mod is enabled, every write instruction to contract storage will be negated. So incrementing bytes at address 100 of the storage will decrement instead. Appending to storage will truncating instead; of course, for this case, truncating happen only during a blockchain upgrade. 

**REVD** will SET/UNSET **REV DEEP MOD**. This mod is a lot more complex. It will reverse a lot of instructions. ADD<>SUB, MUL<>DIV, ROL<>ROR, INC<>DEC ..etc.. If you want to use this mod, be aware of your code pattern and potential loss of informations. Here is a loop example: 

```
  PATTERN:
  revd
  xor ecx, ecx
  mov eax, ecx
  inc eax
  add ecx, 10
  .loop:
  jecxz .end
  dec ecx
  mul 2
  jmp .loop
  .end:
  hlt
  
```
The 32-bit value of A register will be 1024.  
This current code depends of registers' value at vm initialisation. In REVD mod, it will result of register A equal 0. Starting EAX at -1, dividing 10 times...
**Only ECX value is correct, starting loop at -10, incrementing by 1 until 0.** 

Here is another one which depend of values kept from previous contract request . It can be interesting to use this mod in specific case, but general registers values should be stored and loaded at initialisation when needed.  

```
  PATTERN:
  call LOAD_REGISTERS
  revd
  xor ecx, ecx
  add ecx, 10
  .loop:
  jecxz .end
  add eax, 25
  jmp .loop
  .end:
  revd
  call SAVE_REGISTERS
  hlt
  
  ; note here that because of stapp instruction, general registers can only be saved from the stack. 
  SAVE_REGISTERS:
  push ebp
  mov ebp, esp
  pusha
  mov eax, ebp
  sub eax, 0x20
  xor edx, edx
  mov ecx, 0x20
  stapp 
  popa
  pop ebp
  ret
  
  LOAD_REGISTERS:
  xor eax, eax
  xor edx, edx
  mov ecx, 0x20
  strdb
  ret
```
Whatever is EAX after LOAD_REGISTERS call, the virtual machine will correctly negate the loop inside PATTERN, getting back EAX to its older state. 

**One final instruction to make your code reversible is the conditionnal jump instruction JMR.** 

This instruction will proccess a memory jump only during a downgrading proccess. This is the way-to-go to easily set up mirrored instructions in your contract!
Not the most elegant however, increasing also the cost of the contract submission because of a heavier contract. 

```
  PATTERN:
  xor ecx, ecx
  jmr MIRROR
  mov eax, ecx
  .loop:
  jecxz .end
  add eax, 10
  dec ecx
  jmp .loop
  .end:
  hlt
  
  MIRROR:
  mov eax, 110
  .loop:
  jecxz .end
  sub eax, 10
  dec ecx
  jmp .loop
  .end:
  hlt
  
```

**Last words, reverting instructions code depends to contract writer. Because machines usually overwrite their memory, loss of informations is part of the nature of 
computers. Writing piece of codes in blockchain environment is fighting against time and the mutability of elements. Overwriting contract storage is not allowed, here is why there is no simple write instructions to disk space. Writing smartcontract is writing symettric code. Welcome to the world of palindrome langages.**

## Final view of the whole asm code

```
; -------------------------------------------------------------------
; ---------------------------- BIMMARKET 1.1------------------------
; -------------------------------------------------------------------
; WALLET struct 468 o
;     pukey     64  o
;     utxop     4   o
;     ITEMS 	400 o (100*4o) 

; ITEM   struct
;	LOCKBYTE      	1B [market mode / 0: savage market, 1: private market, 2: bid ]  +0
;	CURRPRICE	4B [CURRENT PRICE OF THE ART] 					 +1
;	CWALLET		4B [ARTIST WALLET PTR] 						 +5
;	SIGN		64B[SECP256K1 SIGNAUTR OF ARTIST]				 +9
;	TS		4B [TIMESTAMP AT CREATION]					 +73
;	LASTUPD		4B [TIMESTAMP OF LAST PRICE UPDATE]				 +77
;	TYPE		4B [TYPE OF DATA] 						 +81
;	PPTR		4B [PREVIOUS DATABLOCK POINTER, 0 IF START]			 +85 
;	NPTR		4B [NEXT DATABLOCK POINTER, 0 IF END] 				 +89
;	BIDEND		4B [TIME WHEN BID STOP]						 +93
;	MINBIDPRICE	4B [MIN BID PRICE / FIRST BID PRICE] 				 +97
;	HIGHESTBID	4B [CURRENT HIGHEST BID]  					 +101
;	BIDWPTR		4B [CURRENT HIGHEST BIDDER PTR] 				 +105	
;	DATASIZE	4B [SIZE OF DATA] 						 +109
;	DATA		DATASIZE [DATA] 						 +113
;
; STORAGE struct
; 
; *brutally append WALLET struct or item struct 
; *in no particular order
; *leave a trailing byte to know if it is a wallet struct or
; *item struct

jmp atload

hashbuff  db 0 times 32
pukeybuff db 0 times 64
uintbuff  db 0 times 4

atload:
hlt

reg_art:
; arg :
; wallet ptr (+4)  
; init price (+8) 
; sign (+12)
; type (+76) 
; PPTR (+80)
; size (+84)
; data (+88) 
push ebp
mov  ebp, esp 
; [0] verify wallet integrity 
mov edx, pukeybuff
txkey  ; move current transaction key to addr stored in edx
push pukeybuff
push dword [ebp+4]
call verifyintegritywallet
; [1.1] get data size
mov ecx, [ebp+84]
; [1.2] hash data 
mov eax, ebp
add eax, 88
mov edx, hashbuff
hash 
; [2] verify signature of data.
mov edx, ebp
add edx, 12
mov eax, hashbuff
mov ebx, pukeybuff 
sign
jz badend
; [3] get new item ptr
stlen        ; keep trace of art pointer
mov ebx, eax
; [3] add item to the wallet 
inc ebx ; art ptr is at stlen + the trailing bytes 
push dword[ebp+4]
push ebx
call additemtowallet 
; [4] append the art to storage
; [4.1] append trailing bytes ; weird stuff here 
mov eax, 1
mov dword[uintbuff], eax ; 1 is for item 
mov eax, uintbuff
mov ecx, 1 
stapp
stapp ; append also lockbyte
; [4.2] append init price
mov eax, ebp
add eax, 8
mov ecx, 4
stapp
; [4.3] append creator wallet ptr
mov eax, ebp
add eax, 4
stapp
; [4.4] append signature
mov edx, pukeybuff
txkey 
mov eax, ebp
add eax, 12
mov ecx, 64 
stapp
; [4.5] append timestamp
clock
mov dword[hashbuff], eax
mov eax, hashbuff
mov ecx, 4
stapp ; append it twice for TS and U TS
stapp
; [4.6] append type 
mov eax, ebp
add eax, 76
stapp 
; [4.7] 
mov eax, [ebp+80]
cmp eax, 0
je .zeropptr
push ebx
push eax
call addnptrtoart 
jmp .pptrnext
.zeropptr:
mov ecx, 4
xor ebx, ebx
mov eax, [ebx]
stapp    ; append 0 PPTR
jmp .pptrnext
.pptrnext:
mov ecx, 4
xor eax, eax
mov eax, [eax]
stapp    ; append 0 NPTR
; [4.8] zeroing and append bid data (12b)
stapp ; --------- 4 times 
stapp 
stapp
stapp 
; [4.9] append the data
mov ecx, [ebp+84]
add ecx, 4 ; append also datasize so add 4 lenght
mov eax, ebp
add eax, 84
stapp

hlt

badend:
err

addnptrtoart:
; args : item ptr +8 ; next ptr +12
push ebp
mov ebp, esp
mov eax, [ebp+12]
mov edx, [ebp+8]
add edx, 89 ; ptr to nptr
mov ecx, 1
stad4
pop ebp
retp 8

; check if wallet is correct
verifyintegritywallet: 
; args : wallet ptr +8 ; pukey ptr +12
push ebp
mov ebp, esp
; verify if wallet ptr can be proccessed
stlen
mov ebx, [ebp+8]
add ebx, 468
cmp eax, ebx
jb badend

; verify if wallet pukey is same as provided
mov eax, [ebp+8] 
mov ebx, [ebp+12]

; eax, edx and ecx used for strdb . left is ebx 
mov esi, 16
mov ecx, 4
; read wallet ptr at offset 
.cmploop:
cmp  esi, 0
je .end ; label is confond with end label in assembler c#
mov edx, uintbuff
strdb ; it works ok 
mov edi, dword[uintbuff]
cmp edi, dword[ebx]
jne badend
add ebx, 4
add eax, 4
dec esi
jmp .cmploop
.end:
pop ebp
retp 8 

verifyintegrityitem:
; args : item ptr +8 ; pukey ptr +12
push ebp
mov ebp, esp

pop ebp
retp 8


additemtowallet: ; ITEM PTR +8 , WALLET PTR +12
push ebp
mov ebp, esp
mov eax, [ebp+12]
mov ebx, eax
add ebx, 468
add eax, 68; adjust to item ptrs offset
mov ecx, 4
searchloop: 
cmp eax, ebx
jae badend
mov edx, uintbuff
strdb
mov edx, dword[uintbuff]
cmp edx, 0
je endsearch
add eax, 4
jmp searchloop
endsearch: 
mov edx, eax
mov eax, ebp
add eax, 8
mov ecx, 1
stad4   ; it is a pointer
pop ebp
retp 8


remitemfromwallet: ; ITEM PTR +8 , WALLET PTR +12
push ebp
mov ebp, esp
mov eax, [ebp+12]
mov ebx, eax
add ebx, 468
add eax, 68; adjust to item ptrs offset
mov ecx, 4
searchloopB: 
cmp eax, ebx
jae badend
mov edx, uintbuff
strdb
mov edx, dword[uintbuff]
cmp edx, dword[ebp+8]
je endsearchB
add eax, 4
jmp searchloopB
endsearchB: 
mov edx, eax
mov eax, [ebp+8]
neg eax
push eax
mov eax, ebp
sub eax, 4
mov ecx, 1
stad4   ; it is a pointer
add esp, 4
pop ebp
retp 8

wallettmp db 0 times 400

reg_me: 
; pukey 
; utxop 
push ebp
mov ebp, esp 
xor eax, eax
mov dword[uintbuff], eax
mov eax, uintbuff
mov ecx, 1
stapp ; add trailing bytes 
; add sign
mov ecx, 64
mov eax, ebp
add eax, 8
stapp
; add utxop 
mov ecx, 4
mov eax, ebp
add eax, 4
stapp
mov ecx, 400
mov eax, wallettmp
stapp
pop ebp
hlt

verifyintegrityitem:
; item ptr +8 
push ebp
mov ebp, esp
pop ebp
retp 4

isartpossessed:
; ITEM PTR +8 , WALLET PTR +12
push ebp
mov ebp, esp
mov esi, [ebp+8]
mov eax, [ebp+12]
mov ebx, eax
add ebx, 468
add eax, 68; adjust to item ptrs offset
mov ecx, 4
mov edx, uintbuff
.loop: 
cmp eax, ebx
jae badend
strdb
cmp dword[uintbuff], esi
je .end
add eax, 4
jmp .loop
.end: 
pop ebp
retp 8

unlock_art:
; wallet ptr +4
; art    ptr +8
push ebp
mov ebp, esp
; [0] verify wallet integrity 
mov edx, pukeybuff
txkey; move current transaction key to addr stored in edx
push pukeybuff
push dword [ebp+4]
call verifyintegritywallet
; verify if art is possessed
push dword [ebp+4]
push dword [ebp+8] ; repush because retp 8
call isartpossessed
; !!!  only if art is in private
xor eax, eax
mov dword[uintbuff], eax 
mov eax, [ebp+8] 
mov ecx, 1
mov edx, uintbuff
strdb 
mov edx, [uintbuff]
cmp edx, 1
jne badend
; change the bool in art 
mov eax, -1
mov dword[uintbuff], eax
mov eax, uintbuff 
mov edx, [ebp+8]
mov ecx, 1
stadd
hlt


lock_art:
; wallet ptr +4
; art    ptr +8
push ebp
mov ebp, esp
; [0] verify wallet integrity 
mov edx, pukeybuff
txkey; move current transaction key to addr stored in edx
push pukeybuff
push dword [ebp+4]
call verifyintegritywallet
; verify if art is possessed
push dword [ebp+4]
push dword [ebp+8] ; repush because retp 8
call isartpossessed
; verify art in good mode 
xor eax, eax
mov dword[uintbuff], eax 
mov eax, [ebp+8] 
mov ecx, 1
mov edx, uintbuff
strdb 
mov edx, [uintbuff]
cmp edx, 1
je badend
; change the bool in art 
mov eax, 1
mov dword[uintbuff], eax
mov eax, uintbuff 
mov edx, [ebp+8]
mov ecx, 1
stadd
hlt

update_price:
; new price +4
; estimated time offset  +8 
; wallet ptr +12 
; art ptr +16 
push ebp
mov ebp, esp
; [0] verify wallet integrity 
mov edx, pukeybuff
txkey; move current transaction key to addr stored in edx
push pukeybuff
push dword [ebp+12]
call verifyintegritywallet
; verify if art is possessed
push dword [ebp+16]
push dword [ebp+12] ; repush because retp 8
call isartpossessed
; [2] verify time offset
;-- TODO
; ----
; [2] verify if new price is not above 4x current price
mov edx, uintbuff
mov eax, [ebp+16]
inc eax ; offset for current price 
mov ecx, 4
strdb 
; check if currentprice can be mult by 4 
mov eax, 4294967295
shr eax, 2
cmp eax, dword[uintbuff] 
jb .next ; currentprice cannot be mult by 4. 
mov eax, dword[uintbuff]
add eax, [ebp+4]
mov ebx, dword[uintbuff]
shl ebx, 2
cmp ebx, eax
jb badend
jmp .next
.next:
; [3] update the price 
mov eax,  ebp
add eax, 4
mov edx, [ebp+16] 
inc edx
mov ecx, 1
stad4
hlt

savage_buy:
; buy art as its current price
; args :
; buyer wallet ptr  +4
; holder wallet     +8
; item  ptr         +12
; new price         +16
; buy art as its current price 
push ebp
mov ebp, esp 
; check wallets and art integrity 
mov edx, pukeybuff
txkey  ; move current transaction key to addr stored in edx
push pukeybuff
push dword [ebp+4]
call verifyintegritywallet
push dword [ebp+8]
push dword [ebp+12]
call isartpossessed
; check if art in savage mod
xor eax, eax
mov dword[uintbuff], eax 
mov eax, [ebp+12] 
mov ecx, 1
mov edx, uintbuff
strdb 
mov edx, [uintbuff]
cmp edx, 1
je badend
; [2] verify if new price is not above 4x current price
mov edx, uintbuff
mov eax, [ebp+12]
inc eax ; offset for current price 
mov ecx, 4
strdb 
; check if currentprice can be mult by 4 
mov eax, 4294967295
shr eax, 2
cmp eax, dword[uintbuff] 
jb .next ; currentprice cannot be mult by 4. 
mov eax, dword[uintbuff]
add eax, [ebp+16]
mov ebx, dword[uintbuff]
shl ebx, 2
cmp ebx, eax
jb badend
.next:
; [3] apply the Xchange 
mov eax, [ebp+4] 
add eax, 64 
mov ecx, 4
mov edx, uintbuff
strdb
mov esi, [uintbuff]
mov eax, [ebp+8] 
add eax, 64
strdb
mov edi, [uintbuff]
mov eax, [ebp+12] 
inc eax ; ptr to price
strdb
mov ecx, [uintbuff] 
mov eax, esi
mov edx, edi 
exch
jz badend
; [4] update items ptr of wallets
; ITEM PTR +8 , WALLET PTR +12
push dword[ebp+4]
push dword[ebp+12]
call additemtowallet
push dword[ebp+8]
push dword[ebp+12]
call remitemfromwallet
; [5] update the price
mov eax,  ebp
add eax, 16
mov edx, [ebp+12] 
inc edx
mov ecx, 1
stad4
hlt

buy:
; buyer wallet ptr  +4
; holder wallet     +8
; item  ptr         +12
; new price         +16
; sign              +20
push ebp
mov ebp, esp 
; [1] check wallets and art integrity 
mov edx, pukeybuff
txkey  ; move current transaction key to addr stored in edx
push pukeybuff
push dword [ebp+4]
call verifyintegritywallet
push dword [ebp+8]
push dword [ebp+12]
call isartpossessed
; [1b] verify art in not in bid mode
xor eax, eax
mov dword[uintbuff], eax 
mov eax, [ebp+12] 
mov ecx, 1
mov edx, uintbuff
strdb 
mov edx, [uintbuff]
cmp edx, 1
ja badend

; [2] verify if new price is not above 4x current price
mov edx, uintbuff
mov eax, [ebp+12]
inc eax ; offset for current price 
mov ecx, 4
strdb 
; check if currentprice can be mult by 4 
mov eax, 4294967295
shr eax, 2
cmp eax, dword[uintbuff] 
jb .next ; currentprice cannot be mult by 4. 
mov eax, dword[uintbuff]
add eax, [ebp+16]
mov ebx, dword[uintbuff]
shr ebx, 2
cmp ebx, eax
jb badend
.next:
; [3] verify signature 
mov eax, ebp
add eax, 4
mov ecx, 16
mov edx, hashbuff
hash
; get the pukey of holder
mov eax, [ebp+8]
add eax, 64     ; ptr to utxo pointer
mov edx, uintbuff
mov ecx, 4
strdb
mov eax, [uintbuff]
mov edx, wallettmp
utxo 
mov ebx, wallettmp
mov eax, hashbuff
mov edx, ebp
add edx, 20
sign
jz badend
; [4] apply the Xchange 
mov eax, [ebp+4] 
add eax, 64 
mov ecx, 4
mov edx, uintbuff
strdb
mov esi, [uintbuff]
mov eax, [ebp+8] 
add eax, 64
strdb
mov edi, [uintbuff]
mov eax, [ebp+12] 
inc eax ; ptr to price
strdb
mov ecx, [uintbuff] 
mov eax, esi
mov edx, edi 
exch
jz badend
; [5] update items ptr of wallets
; ITEM PTR +8 , WALLET PTR +12
push dword[ebp+4]
push dword[ebp+12]
call additemtowallet
push dword[ebp+8]
push dword[ebp+12]
call remitemfromwallet
; [5] update the price
mov eax,  ebp
add eax, 16
mov edx, [ebp+12] 
inc edx
mov ecx, 1
stad4
hlt

bid: 
mov ebp, esp 
hlt

startbid: 
mov ebp, esp 
hlt

closebid:
; art ptr +4 ; bid high price +8 ; bid wptr +12 ; holder wptr +16; curr lock time + 20
mov ebp, esp
; [0] verify art in bid mod
xor eax, eax
mov dword[uintbuff], eax 
mov eax, [ebp+4] 
mov ecx, 1
mov edx, uintbuff
strdb 
mov edx, [uintbuff]
cmp edx, 2
jb badend
; [1] verify time is not over
mov eax, [ebp+4] 
add eax,  93
mov ecx, 4
strdb
mov ebx, [uintbuff]
clock
cmp ebx, eax
jb .end
; [2] verify if high price is correct. verify also if not 0 
mov eax, [ebp+4] 
add eax, 101
strdb
mov ebx, [uintbuff]
mov eax, [ebp+8]
cmp ebx, eax
jne .end
cmp ebx, 0
je .resetmarketmode 
; [3.a] verify if bidwptr is  correct
mov eax, [ebp+4] 
add eax, 105
strdb
mov ebx, [uintbuff]
mov eax, [ebp+12]
cmp ebx, eax
jne .end
; [3.b] verify if lock time is  correct
mov eax, [ebp+4] 
add eax, 93
strdb
mov ebx, [uintbuff]
mov eax, [ebp+20]
cmp ebx, eax
jne .end
; [3.c] verify if holder possess the art. 
; ITEM PTR +8 , WALLET PTR +12
push dword[ebp+16]
push dword[ebp+4]
call isartpossessed
mov ecx, 4
mov edx, uintbuff
; [4] apply the xchange
; load & store bidder ptr as donator
mov eax, [ebp+12] 
add eax, 64 ; offset to utxo
strdb
mov esi, [uintbuff]
; load & store holder ptr as receiver
mov eax, [ebp+16] 
add eax, 64 ; offset to utxo
strdb
mov edx, [uintbuff]
; get bid price
mov ecx, [ebp+8] 
mov eax, esi
exch
jz badend
; [5] reset the biddata
; undo bidend and highestbid. minbidprice will be automaticcaly init at startbid crt
mov edx, [ebp+4] 
add edx, 93 
mov eax, [ebp+20]
neg eax
mov ecx, 1
stad4
add edx, 8 
mov eax, [ebp+8]
neg eax
stad4
add edx, 4
mov eax, [ebp+12]
neg eax
stad4
.resetmarketmode: 
mov eax, -1 ; set -1 to make it to private mode
mov dword[uintbuff], eax
mov eax, uintbuff 
mov edx, [ebp+8]
mov ecx, 1
stadd
.end:
hlt

#reg_me
#reg_art
#unlock_art
#lock_art
#update_price
#savage_buy
#buy
#startbid
#bid
#closebid
```
