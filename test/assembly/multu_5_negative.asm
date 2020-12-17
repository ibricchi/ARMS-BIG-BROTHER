ADDIU $a0, $a0, 13
SUBU $a1, $a1, $a0
MULTU $a1, $a1
MFLO $v0
MFHI $a2
ADDU $v0, $v0, $a2
JR $ra
