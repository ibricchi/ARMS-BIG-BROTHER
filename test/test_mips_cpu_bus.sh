#!bin/bash

SOURCE_DIRECTORY="$1"
INSTRUCTION="${2:-ALL}" # Run all testcases if no instruction is specified
CPU_VARIANT="bus"

TESTCASES="./assembly/*.asm"
C_TEST_FILES="./test_program/*.c"

for i in ${TESTCASES} ; do
    TESTNAME=$(basename ${i} .asm)
    TEST_INSTRUCTION="$(grep -oE ^${INSTRUCTION} <<< ${TESTNAME})"
    
    if [[ "$INSTRUCTION" == "$TEST_INSTRUCTION" ]] || [[ "$INSTRUCTION" == "ALL" ]] ; then
        ./run_one_testcase.sh ${SOURCE_DIRECTORY} ${CPU_VARIANT} ${TESTNAME}
    fi
done

for i in ${C_TEST_FILES} ; do
    TESTNAME=$(basename ${i} .c)
    
    if [[ "$INSTRUCTION" == "ALL" ]] || [[ "$INSTRUCTION" == "c" ]] ; then
        ./run_one_testprogram.sh ${SOURCE_DIRECTORY} ${CPU_VARIANT} ${TESTNAME}
    fi
done
