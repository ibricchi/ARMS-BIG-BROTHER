LUI $a0, 0xffff         # a0 = -65536
SLTI $v0, $a0, 0xffff   # v0 = -65536 <= -1 = 0
JR $ra
