ADDIU $a0, $a0, 10  # a0 = 10
SUBU $a1, $a1, $a0  # a1 = -10
MULT $a0, $a1       # hi:lo = 10*-10 = 0xFFFF:FF9c
MFLO $v0            # v0 = 0xFF9c
MFHI $a2            # a2 = 0xFFFF
ADDU $v0, $v0, $a2  # v0 = 0x1FF9B = 130971
JR $ra
