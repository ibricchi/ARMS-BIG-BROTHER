ADDIU $a0, $a0, 10
ADDIU $a1, $a1, 1
Loop:
ADDIU $v0, $v0, 137
SUBU $a0, $a0, $a1
BNE $zero, $a0, Loop
SLL $zero, $zero, 0
JR $ra
