#!/bin/bash

set -eo pipefail

SOURCE_DIRECTORY="$1"
INSTRUCTION="${2:-all}" # Run all testcases if no instruction is specified
CPU_VARIANT="bus"

TESTCASES="./test/assembly/*.asm"
C_TEST_FILES="./test/test_program/*.c"

# Require SOURCE_DIRECTORY argument
if [[ -z "${SOURCE_DIRECTORY}" ]] ; then
   echo "FAIL: Missing SOURCE_DIRECTORY argument"
   exit
fi

# Build all necessary tools
./test/tools/build_tools.sh

for i in ${TESTCASES} ; do
    TESTNAME=$(basename ${i} .asm)
    TEST_INSTRUCTION="$(grep -oE ^$INSTRUCTION <<< $TESTNAME || true)" # '|| true prevents grep from exiting on no match'

    if [[ "$INSTRUCTION" == "$TEST_INSTRUCTION" ]] || [[ "$INSTRUCTION" == "all" ]] ; then
        # Extract real instruction name for printing to cout
        if [[ "$INSTRUCTION" == "all" ]] ; then
            TEST_INSTRUCTION="$(grep -oE ^[a-z]+ <<< ${TESTNAME} || true)"
        fi

        ./test/run_one_testcase.sh ${SOURCE_DIRECTORY} ${CPU_VARIANT} ${TESTNAME} ${TEST_INSTRUCTION}
    fi
done

for i in ${C_TEST_FILES} ; do
    TESTNAME=$(basename ${i} .c)
    
    if [[ "$INSTRUCTION" == "all" ]] || [[ "$INSTRUCTION" == "c" ]] ; then
        ./test/run_one_testcase.sh ${SOURCE_DIRECTORY} ${CPU_VARIANT} ${TESTNAME} ${INSTRUCTION} "yes"
    fi
done
