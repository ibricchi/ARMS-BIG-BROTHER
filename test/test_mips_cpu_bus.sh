#!bin/bash

SOURCE_DIRECTORY="$1"
INSTRUCTION="${2:-ALL}" # Run all testcases if no instruction is specified
CPU_VARIANT="Bus"

TESTCASES="./assembly/*.asm"

for i in ${TESTCASES} ; do
    TESTNAME=$(basename ${i} .asm)
    TEST_INSTRUCTION="$(grep -oE ^${INSTRUCTION} <<< ${TESTNAME})"
    
    if [[ "$INSTRUCTION" == "$TEST_INSTRUCTION" ]] || [[ "$INSTRUCTION" == "ALL" ]] ; then
        ./run_one_testcase.sh ${CPU_VARIANT} ${TESTNAME}
    fi
done
