ADDIU $a0, $a0, 202
ADDIU $a1, $a1, 300
SB $a0, 1($a1)
LBU $v0, 1($a1)
JR $ra
