ADDIU $a0, $a0, 15
loop:
BEQ $t0, $a0, exit
NOP
ADDIU $t0, $t0, 1
ADDIU $v0, $v0, 3
J loop
NOP
exit:
JR $zero
