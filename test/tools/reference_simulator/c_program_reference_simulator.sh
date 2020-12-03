#!/bin/bash

set -eo pipefail

CFILE="$1"

if [[ "$CFILE" == "" ]] ; then
    >&2 echo "  c_program_reference_simulator: Missing CFILE argument"
    exit 1
fi

# compile c program
g++ ./test_program/${CFILE}.c -o ./temp/${CFILE}

# run c program and capture output
set +e
./temp/${CFILE}
REFERENCE_OUT="$?"
set -e

echo "${REFERENCE_OUT}" > ./reference/${CFILE}.out-v0
