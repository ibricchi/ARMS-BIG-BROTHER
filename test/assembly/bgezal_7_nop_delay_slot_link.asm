ADDIU $a0, $a0, 11              # set a0 = 11
SUBU $a1, $a1, $a0              # set a1 = -11
BGEZAL $a1, GreaterEqualZero    # jump if a1 >= 0 which means it should not jump but set L to 0xBFC00010
NOP                             # Branch delay slot
ADDIU $v0, $v0, 100             # skipped
GreaterEqualZero:               # jump here
ADDIU $v0, $v0, 3               # set v0 = 3
ADDU $v0, $v0, $ra              # set v0 = 3 + 0xBFC00010 = 3217031187
JR $zero
