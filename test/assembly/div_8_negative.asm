ADDIU $a0, $a0, 10  # set a0 to 10
ADDIU $a1, $a1, 5   # set a1 to 5
SUBU $a1, $0, $a1   # set a1 to -5
DIV $a0, $a1        # a0/a1 = 10/-5
MFHI $v0
JR $ra
