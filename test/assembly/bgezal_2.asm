ADDIU $a0, $a0, 0
BGEZAL $a0, GreaterEqualZero
ADDIU $v0, $v0, 50      # Branch delay slot
ADDIU $v0, $v0, 100
GreaterEqualZero:
ADDIU $v0, $v0, 3
JR $zero
