ADDIU $a0, $a0, 10  # set a0 to 10
SUBU $a1, $a1, $a0  # set a1 to -10
DIV $a1, $a1        # a1/a1 = -10/-10
MFLO $v0
MFHI $a1
ADDU $v0, $v0, $a1
JR $ra
