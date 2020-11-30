#!/bin/bash
set -e

if [ $# -eq 0]
then
./build_assembler.sh
fi

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
    if cmp -s "./tests/$test.bin" "./tests/$test.bin.out"
    then
        echo "PASS"
    else
        pass="FAIL"
        echo "FAIL"
    fi 
    echo ""
done

echo "Overal: $pass"
