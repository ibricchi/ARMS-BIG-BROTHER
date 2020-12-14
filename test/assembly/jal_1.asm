ADDIU $v0, $v0, 10
JAL End
NOP
ADDIU $v0, $v0, 1000
End:
ADDU $v0, $v0, $ra
JR $zero
