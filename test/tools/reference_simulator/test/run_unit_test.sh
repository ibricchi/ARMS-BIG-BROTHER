#!/bin/bash

set -e

g++ -std=c++11 test/tools/reference_simulator/test/unit_test.cpp test/tools/reference_simulator/asm_reference_simulator.cpp -o test/tools/bin/reference_simulator_unit_test

./test/tools/bin/reference_simulator_unit_test
