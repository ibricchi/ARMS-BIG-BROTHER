set -eou pipefail

SOURCE_DIRECTORY="$1"
CPU_VARIANT="$2"
TEST_PROGRAM="$3"

REFERENCE_SIMULATOR_PATH="./" # SPECIFY THIS!

>&2 echo "Test MIPS ${CPU_VARIANT} using test-program ${TEST_PROGRAM}"

>&2 echo "  1 - Assembling test case"
./c_to_mips_hex.sh ${TEST_PROGRAM}

>&2 echo "  2 - Compiling test-bench"
iverilog -g 2012 \
   SOURCE_DIRECTORY/*.v \ # maybe need to add RAM file here (depending on implementation)
   -s mips_cpu_${VARIANT}_tb \ # we are including all .v files in SOURCE_DIRECTORY and hence need to be exact about which VARIANT to test here
   -o ./simulator/mips_cpu_${VARIANT}_tb_${TEST_PROGRAM}

>&2 echo "  3 - Running test-bench"
set +e
./simulator/mips_cpu_${VARIANT}_tb_${TEST_PROGRAM} > ./output/mips_cpu_${VARIANT}_tb_${TEST_PROGRAM}.out
RESULT=$?
set -e

if [[ "${RESULT}" -ne 0 ]] ; then
   echo "   MIPS_${VARIANT}, ${TEST_PROGRAM}, FAIL"
   exit
fi

>&2 echo "  4 - Running reference simulator"
set +e
# might need to modify the input to REFERENCE_SIMULATOR_PATH, depending on how REFERENCE_SIMULATOR is implemented
REFERENCE_SIMULATOR_PATH < ./binary/${TEST_PROGRAM}.hex > ./reference/${TEST_PROGRAM}.out
set -e

>&2 echo "  5 - Comparing output"
set +e
diff -w ./reference/${TEST_PROGRAM}.out ./output/mips_cpu_${VARIANT}_tb_${TEST_PROGRAM}.out
RESULT=$?
set -e

if [[ "${RESULT}" -ne 0 ]] ; then
   echo "   MIPS_${VARIANT}, ${TEST_PROGRAM}, FAIL"
else
   echo "   MIPS_${VARIANT}, ${TEST_PROGRAM}, PASS"
fi
