ADDIU $a0, $a0, 1   # a0 = 1
ADDIU $a1, $a1, 300 # a1 = 300
SB $a0, 0($a1)      # mem(300) = 1
SB $a0, 1($a1)      # mem(301) = 1
SB $a0, 2($a1)      # mem(302) = 1
SB $a0, 3($a1)      # mem(303) = 1
LW $v0, 0($a1)      # v0 = mem(300) = 0x01010101 = 16843009
JR $ra
