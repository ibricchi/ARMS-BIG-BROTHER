ADDIU $a0, $a0, 11
SUBU $a1, $a1, $a0
BGTZ $a1, GreaterZero
NOP                       # Branch delay slot
ADDIU $v0, $v0, 100
GreaterZero:
ADDIU $v0, $v0, 3
JR $ra
