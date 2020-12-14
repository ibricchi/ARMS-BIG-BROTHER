ADDIU $a0, $a0, 0
BGTZ $a0, GreaterZero
ADDIU $v0, $v0, 50      # Branch delay slot
ADDIU $v0, $v0, 100
GreaterZero:
ADDIU $v0, $v0, 3
JR $ra
