#!/bin/bash

# This file will be called once from inside 'test_mips_cpu_{bus/harvard}.sh'
# It is responsible for all tools that might be necessary for 'test_mips_cpu_{bus/harvard}.sh' to run properly.

# assembler
./tools/assembler/build_assembler.sh

# reference simulator
./tools/reference_simulator/build_reference_simulator.sh
