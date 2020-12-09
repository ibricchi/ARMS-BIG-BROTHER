ADDIU $a0, $a0, 11
ADDIU $a1, $a1, 11
ADDIU $v0, $v0, 50
BNE $a0, $a1, NotEqual
ADDIU $v0, $v0, 100
NotEqual:
JR $ra
