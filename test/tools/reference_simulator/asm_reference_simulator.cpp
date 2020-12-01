#include "asm_reference_simulator.hpp"

#include <cassert>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

int main()
{
    const uint32_t memInstructionStartIdx = 0xBFC00000;

    vector<uint32_t> memory = readMemoryBinary(cin, memInstructionStartIdx);

    uint32_t register_v0 = simulateMIPS(memory, memInstructionStartIdx);

    // cout final value stored in register $v0
    // this should be used as the reference output
    cout << register_v0 << endl;
}

vector<uint32_t> readMemoryBinary(istream &src, const uint32_t memInstructionStartIdx)
{
    const uint32_t maxUint32t = 0xFFFFFFFF;
    vector<uint32_t> memory(maxUint32t, 0);

    uint32_t currentMemAddress = memInstructionStartIdx;
    string line{};
    while (getline(src, line))
    {
        // Trim leading space
        while (line.size() > 0 && isspace(line.front()))
        {
            line = line.substr(1);
        }

        // Trim trailing space
        while (line.size() > 0 && isspace(line.back()))
        {
            line.pop_back();
        }

        // A line should be exactly 4 bytes (8 hex symbols)
        if (line.size() != 8)
        {
            cerr << "Line number " << currentMemAddress - memInstructionStartIdx << " : expected exactly eight chars, got '" << line << '"' << endl;
            exit(1);
        }

        for (int i = 0; i < line.size(); i++)
        {
            if (!isxdigit(line[i]))
            {
                cerr << "Line number " << currentMemAddress - memInstructionStartIdx << " : expected only hexadecimal digits, got '" << line[i] << '"' << endl;
                exit(1);
            }
        }

        // Convert base 16 string to unsigned integer
        unsigned memLineData = stoul(line, nullptr, 16);
        assert(memLineData < 0xFFFFFFFF);

        assert(currentMemAddress <= maxUint32t);
        memory[currentMemAddress] = memLineData;

        currentMemAddress++;
    }

    return memory;
}

uint32_t simulateMIPS(vector<uint32_t> &memory, const uint32_t memInstructionStartIdx)
{
    // Implementation in progress
    return 0;
}
