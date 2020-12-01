#ifndef ASM_REFERENCE_SIMULATOR_HPP
#define ASM_REFERENCE_SIMULATOR_HPP

#include <vector>
#include <iostream

using namespace std;

// Read a binary from the given stream
// This always returns the full 4096 values, even if there are fewer in the input binary
vector<uint32_t> readMemoryBinary(istream &src);

// Simulates the execution of a program on a MIPS processors.
// Input: Receives a vector that represents the initial memory.
// Output: Returns the final value inside register $v0 (register_v0).
uint32_t simulateMIPS(vector<uint32_t> &memory);

#endif
