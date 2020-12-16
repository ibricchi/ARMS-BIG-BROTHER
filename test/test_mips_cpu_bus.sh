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

# Give permission to child test script
chmod +x \
    ./test/tools/build_tools.sh \
    ./test/c_to_mips_hex.sh \
    ./test/run_one_testcase.sh \
    ./test/tools/reference_simulator/c_program_reference_simulator.sh

# Build all necessary tools
./test/tools/build_tools.sh

for i in ${TESTCASES} ; do
    TESTNAME=$(basename ${i} .asm)
    TEST_INSTRUCTION="$(grep -oE ^$INSTRUCTION\_ <<< $TESTNAME || true)" # '|| true prevents grep from exiting on no match'
    TEST_INSTRUCTION="$(sed -E s/_// <<< ${TEST_INSTRUCTION} || true)"

    if [[ "$INSTRUCTION" == "$TEST_INSTRUCTION" ]] || [[ "$INSTRUCTION" == "all" ]] ; then
        # Extract real instruction name for printing to cout
        if [[ "$INSTRUCTION" == "all" ]] ; then
            TEST_INSTRUCTION="$(grep -oE ^[a-z]+_ <<< ${TESTNAME} || true)"
            TEST_INSTRUCTION="$(sed -E s/_// <<< ${TEST_INSTRUCTION} || true)"
        fi

        ./test/run_one_testcase.sh ${SOURCE_DIRECTORY} ${CPU_VARIANT} ${TESTNAME} ${TEST_INSTRUCTION}
    fi
done

# Only test c-programs if specified. Not tested when no instruction is specified.
if [[ "$INSTRUCTION" == "c" ]] ; then
    for i in ${C_TEST_FILES} ; do
        TESTNAME=$(basename ${i} .c)
        ./test/run_one_testcase.sh ${SOURCE_DIRECTORY} ${CPU_VARIANT} ${TESTNAME} ${INSTRUCTION} "yes"
    done
fi
