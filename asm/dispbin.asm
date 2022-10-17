       MOV   RBX, 0x600000
       MOV   RCX,16            ;set up loop counter
NEXT:  SHL   AX,1            ;move bit into Carry flag
       PUSH  AX              ;save number
       JC    ITIS1           ;was the LSB a 1?
       MOV   RDX,'0'          ;load '0' character
       JMP   SAY01           ;go display it
ITIS1: MOV   RDX,'1'         ;load '1' character
SAY01: MOV   [RBX],RDX            ;display character function
       INC   RBX             ;DOS call
       POP   AX              ;get number back
       LOOP  NEXT            ;and repeat