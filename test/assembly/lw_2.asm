ADDIU $a0, $a0, 78
ADDIU $a1, $a1, 300
SW $a0, 12($a1)
LW $v0, 12($a1)
JR $ra
