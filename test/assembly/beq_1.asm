ADDIU $a0, $a0, 11
ADDIU $a1, $a1, 11
BEQ $a0, $a1, Equal
ADDIU $v0, $v0, 50
ADDIU $v0, $v0, 100
Equal:
JR $ra
