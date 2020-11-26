#!bin/bash

set -eou pipefail

SOURCE_DIRECTORY="$1"
CPU_VARIANT="$2"
TESTCASE="$3"

ASSEMBLER_PATH="./" # SPECIFY THIS!
REFERENCE_SIMULATOR_PATH="./" # SPECIFY THIS!

>&2 echo "Test MIPS ${CPU_VARIANT} using test-case ${TESTCASE}"

>&2 echo "  1 - Assembling test case"
ASSEMBLER_PATH < ./assembly/${TESTCASE}.asm > ./binary/${TESTCASE}.hex

>&2 echo "  2 - Compiling test-bench"
iverilog -g 2012 \
   SOURCE_DIRECTORY/*.v \ # maybe need to add RAM file here (depending on implementation)
   -s mips_cpu_${VARIANT}_tb \ # we are including all .v files in SOURCE_DIRECTORY and hence need to be exact about which VARIANT to test here
   -o ./simulator/mips_cpu_${VARIANT}_tb_${TESTCASE}

>&2 echo "  3 - Running test-bench"
set +e
./simulator/mips_cpu_${VARIANT}_tb_${TESTCASE} > ./output/mips_cpu_${VARIANT}_tb_${TESTCASE}.out
RESULT=$?
set -e

if [[ "${RESULT}" -ne 0 ]] ; then
   echo "   MIPS_${VARIANT}, ${TESTCASE}, FAIL"
   exit
fi

>&2 echo "  4 - Running reference simulator"
set +e
# might need to modify the input to REFERENCE_SIMULATOR_PATH, depending on how REFERENCE_SIMULATOR is implemented
REFERENCE_SIMULATOR_PATH < ./binary/${TESTCASE}.hex > ./reference/${TESTCASE}.out
set -e

>&2 echo "  5 - Comparing output"
set +e
diff -w ./reference/${TESTCASE}.out ./output/mips_cpu_${VARIANT}_tb_${TESTCASE}.out
RESULT=$?
set -e

if [[ "${RESULT}" -ne 0 ]] ; then
   echo "   MIPS_${VARIANT}, ${TESTCASE}, FAIL"
else
   echo "   MIPS_${VARIANT}, ${TESTCASE}, PASS"
fi
