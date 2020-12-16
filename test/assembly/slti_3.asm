ADDIU $a0, $a0, 2   # a0 = 2
SUBU $v0, $a1, $a0  # v0 = -2
SLTI $v0, $v0, 2    # v0 = -2 <= 2 = 1
JR $ra
