ADDIU $a0, $a0, 11
SUBU $a1, $a1, $a0
BLTZ $a1, SmallerZero
NOP                       # Branch delay slot
ADDIU $v0, $v0, 100
SmallerZero:
ADDIU $v0, $v0, 3
JR $ra
