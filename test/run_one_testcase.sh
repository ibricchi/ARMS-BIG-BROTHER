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
   ./test/c_to_mips_hex.sh ${TESTCASE}
elif [[ "$IS_C_PROGRAM" == "no" ]] ; then
   >&2 echo "Test MIPS ${CPU_VARIANT} using test-case ${TESTCASE}"

   >&2 echo "  1 - Assembling test case"
   ./test/tools/bin/assembler < ./test/assembly/${TESTCASE}.asm > ./test/binary/${TESTCASE}.hex
else
   >&2 echo "FAIL: Invalid 'IS_C_PROGRAM' argument: $IS_C_PROGRAM, test-case: ${TESTCASE}"
   exit
fi

>&2 echo "  2 - Compiling test-bench"
iverilog -g 2012 \
   $SOURCE_DIRECTORY/*.v \
   -s mips_cpu_${CPU_VARIANT}_tb \
   -P mips_cpu_${CPU_VARIANT}_tb.RAM_INIT_FILE=\"./test/binary/${TESTCASE}.hex\" \
   -o ./test/simulator/mips_cpu_${CPU_VARIANT}_tb_${TESTCASE}

>&2 echo "  3 - Running test-bench"
set +e
./test/simulator/mips_cpu_${CPU_VARIANT}_tb_${TESTCASE} > ./test/output/mips_cpu_${CPU_VARIANT}_tb_${TESTCASE}.stdout
RESULT=$?
set -e

if [[ "${RESULT}" -ne 0 ]] ; then
   echo "${TESTCASE} ${INSTRUCTION} Fail  # Verilog testbench returned error code"
   exit
fi

>&2 echo "     Extracting final register_v0 value"
PATTERN="TB: INFO: register_v0 = " 
set +e
grep "${PATTERN}" ./test/output/mips_cpu_${CPU_VARIANT}_tb_${TESTCASE}.stdout > ./test/output/mips_cpu_${CPU_VARIANT}_tb_${TESTCASE}.out-v0-lines
set -e
NOTHING=""
sed -e "s/${PATTERN}/${NOTHING}/" ./test/output/mips_cpu_${CPU_VARIANT}_tb_${TESTCASE}.out-v0-lines > ./test/output/mips_cpu_${CPU_VARIANT}_tb_${TESTCASE}.out-v0

>&2 echo "  4 - Running reference simulator"
set +e
if [[ "$IS_C_PROGRAM" == "yes" ]] ; then
   ./test/tools/reference_simulator/c_program_reference_simulator.sh ${TESTCASE}
else
   ./test/tools/bin/asm_reference_simulator < ./test/binary/${TESTCASE}.hex > ./test/reference/${TESTCASE}.out-v0
fi
RESULT=$?
set -e

if [[ "${RESULT}" -ne 0 ]] ; then
   echo "${TESTCASE} ${INSTRUCTION} Fail  # Reference simulator returned error code"
   exit
fi

>&2 echo "  5 - Comparing output"
set +e
diff -w ./test/reference/${TESTCASE}.out-v0 ./test/output/mips_cpu_${CPU_VARIANT}_tb_${TESTCASE}.out-v0 >&2
RESULT=$?
set -e

if [[ "${RESULT}" -ne 0 ]] ; then
   echo "${TESTCASE} ${INSTRUCTION} Fail  # Final register v0 value does not match"
else
   echo "${TESTCASE} ${INSTRUCTION} Pass"
fi
