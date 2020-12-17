ADDIU $a0, $a0, 65535
ADDIU $a1, $a1, 43218
MULTU $a0, $a1
MFLO $v0
MFHI $a2
ADDU $v0, $v0, $a2
JR $ra
