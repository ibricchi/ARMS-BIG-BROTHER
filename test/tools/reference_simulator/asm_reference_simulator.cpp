#include "asm_reference_simulator.hpp"

#include <vector>
#include <iostream>

using namespace std;

int main()
{
    vector<uint32_t> memory = readMemoryBinary(cin);

    uint32_t register_v0 = simulateMIPS(memory);

    // cout final value stored in register $v0
    // this should be used as the reference output
    cout << register_v0 << endl;
}

vector<uint32_t> readMemoryBinary(istream &src)
{
}

uint32_t simulateMIPS(vector<uint32_t> &memory)
{
    const uint32_t memInstructionStartIdx = 0xBFC00000;
}
