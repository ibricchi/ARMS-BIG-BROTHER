#!/bin/bash
set -e

if [ $# -eq 0 ]
then
./build_assembler.sh
fi

cd tests
tests=($(ls | grep '.asm$' | sed 's/.asm$//'))
cd ../

echo "Running tests:"
echo ${tests[@]};
echo ""

pass="PASS"
for test in ${tests[@]}; do
    ../bin/assembler < "./tests/$test.asm" > "./tests/$test.bin.out"
    if cmp -s "./tests/$test.bin" "./tests/$test.bin.out"
    then
        echo "Test ${test}: PASS"
    else
        pass="FAIL"
        echo "Test ${test}: FAIL"
    fi 
done
echo ""

echo "Overal: $pass"
