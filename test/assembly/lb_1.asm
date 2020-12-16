ADDIU $a0, $a0, 65520   # a0 = 0xFFF0
ADDIU $a0, $a0, 65520   # a0 = 0x1FFE0
ADDIU $a0, $a0, 65520   # a0 = 0x2FFD0
ADDIU $a0, $a0, 65520   # a0 = 0x3FFC0
ADDIU $a0, $a0, 65520   # a0 = 0x4FFB0
ADDIU $a1, $a1, 300     # a1 = 300
SW $a0, 0($a1)          # mem(300) = 0x0004FFB0
LB $v0, 1($a1)          # v0 = mem(301) = 0x04
JR $ra
