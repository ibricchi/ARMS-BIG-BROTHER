ADDIU $a0, $a0, 10  # a0 = 0xa
SUBU $a1, $a1, $a0  # a1 = 0xFFFFFFF6
MULTU $a1, $a0      # hi:lo = 0x00000009:FFFFFF9C
MFHI $v0            # v0 = 0x9
JR $ra
