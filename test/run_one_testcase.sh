#!/bin/bash

set -eou pipefail

SOURCE_DIRECTORY="$1"
CPU_VARIANT="$2"
TESTCASE="$3"
INSTRUCTION="$4" # only used for printing to stdout
IS_C_PROGRAM="${5:-no}" # 'yes' or 'no' (default is 'no')

if [[ "$IS_C_PROGRAM" == "yes" ]] ; then
   >&2 echo "Test MIPS ${CPU_VARIANT} using test-program ${TESTCASE}"

   >&2 echo "  1 - Assembling test case"
   ./c_to_mips_hex.sh ${TESTCASE}
elif [[ "$IS_C_PROGRAM" == "no" ]] ; then
   >&2 echo "Test MIPS ${CPU_VARIANT} using test-case ${TESTCASE}"

   >&2 echo "  1 - Assembling test case"
   ./tools/bin/assembler < ./assembly/${TESTCASE}.asm > ./binary/${TESTCASE}.hex
else
   >&2 echo "FAIL: Invalid 'IS_C_PROGRSM' argument: $IS_C_PROGRAM"
   exit
fi

>&2 echo "  2 - Compiling test-bench"
iverilog -g 2012 \
   $SOURCE_DIRECTORY/*.v \ # maybe need to add RAM file here (depending on implementation)
   -s mips_cpu_${VARIANT}_tb \ # we are including all .v files in SOURCE_DIRECTORY and hence need to be exact about which VARIANT to test here
   -Pmips_cpu_${VaRIANT}_tb.RAM_INIT_FILE="./binary/${TESTCASE}.hex"
   -o ./simulator/mips_cpu_${VARIANT}_tb_${TESTCASE}

>&2 echo "  3 - Running test-bench"
set +e
./simulator/mips_cpu_${VARIANT}_tb_${TESTCASE} > ./output/mips_cpu_${VARIANT}_tb_${TESTCASE}.stdout
RESULT=$?
set -e

>&2 echo "     Extracting final register_v0 value"
PATTERN="TB: INFO: register_v0 = " ./output/mips_cpu_${VARIANT}_tb_${TESTCASE}.stdout > ./output/mips_cpu_${VARIANT}_tb_${TESTCASE}.out-v0-lines
set +e
grep "${PATTERN}" 
set -e
NOTHING=""
sed -e "s/${PATTERN}/${NOTHING}/" ./output/mips_cpu_${VARIANT}_tb_${TESTCASE}.out-v0-lines > ./output/mips_cpu_${VARIANT}_tb_${TESTCASE}.out-v0

if [[ "${RESULT}" -ne 0 ]] ; then
   echo "${TESTCASE} ${INSTRUCTION} Fail"
   exit
fi

>&2 echo "  4 - Running reference simulator"
set +e
if [[ "$IS_C_PROGRAM" == "yes" ]] ; then
   ./tools/reference_simulator/c_program_reference_simulator.sh ${TESTCASE}
else
   ./tools/bin/asm_reference_simulator < ./binary/${TESTCASE}.hex > ./reference/${TESTCASE}.out-v0
fi
set -e

>&2 echo "  5 - Comparing output"
set +e
diff -w ./reference/${TESTCASE}.out-v0 ./output/mips_cpu_${VARIANT}_tb_${TESTCASE}.out-v0
RESULT=$?
set -e

if [[ "${RESULT}" -ne 0 ]] ; then
   echo "${TESTCASE} ${INSTRUCTION} Fail"
else
   echo "${TESTCASE} ${INSTRUCTION} Pass"
fi
