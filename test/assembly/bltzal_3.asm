ADDIU $a0, $a0, 1
SUBU $a1, $a1, $a0
BLTZAL $a1, SmallerZero
ADDIU $v0, $v0, 50      # Branch delay slot
ADDIU $v0, $v0, 100
SmallerZero:
ADDIU $v0, $v0, 3
JR $zero
