ADDIU $a0, $a0, 1       # a0 = 1
SLTI $v0, $a0, 65535    # v0 = 1 <= -1 = 0
JR $ra
