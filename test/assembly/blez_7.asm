ADDIU $t0, $t0, 15
ADDIU $a0, $a0, 1
loop:
BLEZ $t0, exit
NOP
SUBU $t0, $t0, $a0
ADDIU $v0, $v0, 3
J loop
NOP
exit:
JR $zero
