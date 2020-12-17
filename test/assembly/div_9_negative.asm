ADDIU $a0, $a0, 10
SUBU $a1, $a1, $a0
SUBU $a2, $a2, $a0
DIV $a1, $a2
MFLO $v0
JR $ra
