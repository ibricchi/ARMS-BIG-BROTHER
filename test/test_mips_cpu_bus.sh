#!/bin/bash

SOURCE_DIRECTORY="$1"
INSTRUCTION="${2:-all}" # Run all testcases if no instruction is specified
CPU_VARIANT="bus"

TESTCASES="./assembly/*.asm"
C_TEST_FILES="./test_program/*.c"

# Require SOURCE_DIRECTORY argument
if [[ -z "${SOURCE_DIRECTORY}" ]] ; then
   echo "FAIL: Missing SOURCE_DIRECTORY argument"
   exit
fi

for i in ${TESTCASES} ; do
    TESTNAME=$(basename ${i} .asm)
    TEST_INSTRUCTION="$(grep -oE ^${INSTRUCTION} <<< ${TESTNAME})"
    
    if [[ "$INSTRUCTION" == "$TEST_INSTRUCTION" ]] || [[ "$INSTRUCTION" == "all" ]] ; then
        ./run_one_testcase.sh ${SOURCE_DIRECTORY} ${CPU_VARIANT} ${TESTNAME} ${INSTRUCTION}
    fi
done

for i in ${C_TEST_FILES} ; do
    TESTNAME=$(basename ${i} .c)
    
    if [[ "$INSTRUCTION" == "all" ]] || [[ "$INSTRUCTION" == "c" ]] ; then
        ./run_one_testcase.sh ${SOURCE_DIRECTORY} ${CPU_VARIANT} ${TESTNAME} ${INSTRUCTION} "yes"
    fi
done
