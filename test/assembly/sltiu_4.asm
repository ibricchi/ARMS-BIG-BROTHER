LUI $a0, 0xffff         # a0 = 0xffff0000
SLTIU $v0, $a0, 0xffff   # v0 = 0xffff0000 <= 0xffffffff = 1
JR $ra
