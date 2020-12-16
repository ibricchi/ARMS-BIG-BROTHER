LUI $a0, 3452
ADDIU $a0, $a0, 241
ADDIU $a1, $a1, 300
SW $a0, 0($a1)
LWL $v0, 2($a1)
JR $ra
