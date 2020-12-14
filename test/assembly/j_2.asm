ADDIU $v0, $v0, 10
J End
ADDIU $v0, $v0, 10      # Branch delay slot
ADDIU $v0, $v0, 5       # Gets skipped
ADDIU $v0, $v0, 1000    # Gets skipped
End:
ADDIU $v0, $v0, 3
JR $ra
