ADDIU $v0, $v0, 10
J End
ADDIU $v0, $v0, 10      # Branch delay slot
ADDIU $v0, $v0, 1000    # Gets skipped
End:
JR $ra
