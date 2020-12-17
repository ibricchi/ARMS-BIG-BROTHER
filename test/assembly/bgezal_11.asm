ADDIU $a0, $a0, 15
SUBU $t0, $t0, $a0
loop:
BGEZAL $t0, exit
NOP
ADDIU $t0, $t0, 1
ADDIU $v0, $v0, 3
J loop
NOP
exit:
JR $zero
