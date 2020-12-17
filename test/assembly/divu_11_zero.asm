ADDIU $a0, $a0, 13
DIVU $a1, $a0        # 0 / 13 = 0
MFLO $v0
MFHI $a2
ADDU $v0, $v0, $a2
JR $ra
