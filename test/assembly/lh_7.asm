LUI $a0, 53419
ADDIU $a0, $a0, 202
ADDIU $a1, $a1, 300
SH $a0, 0($a1)
SH $a0, 2($a2)
LH $v0, 0($a1)
LH $t0, 2($a2)
ADDU $v0, $v0, $t0
JR $ra
