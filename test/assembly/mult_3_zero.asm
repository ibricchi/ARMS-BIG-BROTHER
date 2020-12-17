ADDIU $a0, $a0, 10
MULT $a0, $zero
MFLO $v0
MFHI $a2
ADDU $v0, $v0, $a2
JR $ra
