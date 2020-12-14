ADDIU $v0, $0, 10   # set v0 to 10
JAL End             # jump to end
NOP                 # delay slot nop
ADDIU $v0, $0, 10   # this get's skipped
End:                
ADDIU $ra, $ra, 8   # increases ra by 16 (increases by 4 instructions)
JALR $31            # jump to address in register ra
NOP                 # delay slot nop
ADDU $v0, $0, $ra   # add register ra to v0
JR $0               # exit program and return v0