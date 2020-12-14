ADDIU $a0, $a0, 0
BLEZ $a0, LessEqualZero
ADDIU $v0, $v0, 50      # Branch delay slot
ADDIU $v0, $v0, 100
LessEqualZero:
ADDIU $v0, $v0, 3
JR $ra
