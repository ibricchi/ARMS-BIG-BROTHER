ADDIU $v0, $0, 10   # set v0 to 10
JAL End             # jump to end and link (address of ADDIU, line 4 should now be in $ra)
ADDIU $v0, $v0, 1   # delay slot
ADDIU $v0, $v0, 10  # this get's skipped
End:                
ADDIU $ra, $ra, 20  # increases ra by 20 (increases by 5 instructions, address of ADDU should now be in $ra)
JR $ra              # jump to current address in register ra
ADDIU $v0, $v0, 3   # delay slot
ADDIU $v0, $v0, 1   # should be skipped
ADDU $v0, $v0, $ra  # add register ra to v0
JR $0               # exit program and return v0
