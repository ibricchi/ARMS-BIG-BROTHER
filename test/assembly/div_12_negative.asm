ADDIU $a0, $a0, 10  # set a0 to 10
SUBU $a1, $a1, $a0  # set a1 to -10
DIV $a1, $a0        # a1/a0 = -10/10 ---- HI = 0 LO = -1
MFLO $v0            # v0 = -1
MFHI $a2            # a2 = 0
ADDU $v0, $v0, $a2  # v0 = -1
JR $ra
