ADDIU $v0, $0, 10   # set v0 to 10
JAL End             # jump to end
ADDIU $v0, $0, 10   # this get's skipped
End:                
ADDIU $ra, $ra, 8   # increases ra by 16 (increases by 4 instructions)
JR $31              # jump to address in register ra
ADDU $v0, $0, $ra   # add register ra to v0
JR $0               # exit program and return v0