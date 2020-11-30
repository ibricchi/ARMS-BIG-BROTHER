#!/bin/bash
set -e

./build_assembler.sh

cd tests
tests=( $(ls | grep '.asm$' | sed 's/.asm$//') )
cd ../
echo $tests;

echo "Running tests."
echo ""

pass="PASS"
for test in $tests; do
    echo "Test: ${test}:"
    ../bin/assembler < "./tests/$test.asm" > "./tests/$test.bin.out"
    if diff "./tests/$test.asm" "./tests/$test.bin.out" &> /dev/null
    then
        echo "PASS"
    else
        pass="FAIL"
        echo "FAIL"
    fi 
    echo ""
done

echo "Overal: $pass"
