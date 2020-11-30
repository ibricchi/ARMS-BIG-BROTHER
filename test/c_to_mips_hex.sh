#!/bin/bash
set -eou pipefail

CFILE="$1" # should only include the basename (e.g. 'test' instead of 'test.c')

mipsel-linux-gnu-gcc -g -c -mips1 -mfp32 ./test_program/${CFILE}.c -o ./temp/${CFILE}

mipsel-linux-gnu-objdump -d -S ./temp/${CFILE} > ./temp/${CFILE}.disassembly

LINE_REGEX="^ *[0-9a-f]+:"
grep -E "${LINE_REGEX}" ./temp/${CFILE}.disassembly > ./temp/${CFILE}.disassembly-lines

EXCLUDE_NOP_REGEX="^((?!nop).)*$"
grep -P "${EXCLUDE_NOP_REGEX}" ./temp/${CFILE}.disassembly-lines > ./temp/${CFILE}.disassembly-lines-no-nop

HEX_REGEX="[0-9a-f]{8}"
grep -oE "${HEX_REGEX}" ./temp/${CFILE}.disassembly-lines-no-nop > ./binary/${CFILE}.hex
