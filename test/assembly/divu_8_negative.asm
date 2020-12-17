ADDIU $a0, $a0, 10
SUBU $a1, $a1, $a0
SUBU $a2, $a2, $a0
DIVU $a1, $a2
MFLO $v0
MFHI $a1
ADDU $v0, $v0, $a1
JR $ra
