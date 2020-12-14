#!/bin/bash
set -eou pipefail

CFILE="$1" # should only include the basename (e.g. 'test' instead of 'test.c')

mipsel-linux-gnu-gcc -g -c -mips1 -mfp32 ./test/test_program/${CFILE}.c -o ./test/temp/${CFILE}

mipsel-linux-gnu-objdump -d -S ./test/temp/${CFILE} > ./test/temp/${CFILE}.disassembly

LINE_REGEX="^ *[0-9a-f]+:"
grep -E "${LINE_REGEX}" ./test/temp/${CFILE}.disassembly > ./test/temp/${CFILE}.disassembly-lines

#EXCLUDE_NOP_REGEX="^((?!nop).)*$"
#grep -P "${EXCLUDE_NOP_REGEX}" ./test/temp/${CFILE}.disassembly-lines > ./test/temp/${CFILE}.disassembly-lines-no-nop

HEX_REGEX="[0-9a-f]{8}"
grep -oE "${HEX_REGEX}" ./test/temp/${CFILE}.disassembly-lines > ./test/binary/${CFILE}.hex
