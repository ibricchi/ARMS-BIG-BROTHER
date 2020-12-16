LUI $a0, 53419          # a0 = 0xD0AB0000
ADDIU $a0, $a0, 202     # a0 = a0 + 202 = 0xD0AB00CA 
ADDIU $a1, $a1, 300     # a1 = 300
SH $a0, 0($a1)          # mem(300) = 0x00CA 
LH $v0, 0($a1)          # v0 = mem(300) = 0x00CA = 202
JR $ra
