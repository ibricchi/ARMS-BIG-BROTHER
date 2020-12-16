#!/bin/bash

# This file will be called once from inside 'test_mips_cpu_{bus/harvard}.sh'
# It is responsible for all tools that might be necessary for 'test_mips_cpu_{bus/harvard}.sh' to run properly.

# assembler
chmod +x ./test/tools/assembler/build_assembler.sh
./test/tools/assembler/build_assembler.sh

# reference simulator
chmod +x ./test/tools/reference_simulator/build_reference_simulator.sh
./test/tools/reference_simulator/build_reference_simulator.sh
