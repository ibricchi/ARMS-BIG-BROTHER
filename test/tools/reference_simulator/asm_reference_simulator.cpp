#include "asm_reference_simulator.hpp"

#include <array>
#include <tuple>
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

        // Remove leading 0x if exists
        if (line.substr(0, 2) == "0x")
        {
            line = line.substr(2);
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

        currentMemAddress += 4; // byte indexing
    }

    return memory;
}

uint32_t simulateMIPS(vector<uint32_t> &memory, const uint32_t memInstructionStartIdx)
{
    const uint32_t maxUint32t = 0xFFFFFFFF;

    uint32_t pc = memInstructionStartIdx;
    array<uint32_t, 32> regs{};

    while (pc != 0) // attempting to execute address 0 causes the CPU to halt
    {
        assert(pc <= maxUint32t);

        uint32_t instruction = memory[pc];
        uint32_t opcode = instruction >> 26; // opcode is 6 bits
        assert(opcode <= 0b111111);

        switch (opcode)
        {
        case 0b000000: // ARITHLOG
        {
            uint32_t func = instruction & 0b111111;
            assert(func <= 0b111111);
            switch (func)
            {
            case 0b100001: // ADD
            {
                break;
            }
            case 0b100100: // AND
            {
                break;
            }
            case 0b100101: // OR
            {
                break;
            }
            case 0b101010: // SLT
            {
                break;
            }
            case 0b101011: // SLTU
            {
                break;
            }
            case 0b100011: // SUBU
            {
                break;
            }
            case 0b100110: // XOR
            {
                break;
            }
            case 0b011010: // DIV
            {
                break;
            }
            case 0b011011: // DIVU
            {
                break;
            }
            case 0b011000: // MULT
            {
                break;
            }
            case 0b011001: // MULTU
            {
                break;
            }
            case 0b000000: // SLL
            {
                break;
            }
            case 0b000011: // SRA
            {
                break;
            }
            case 0b000010: // SWL
            {
                break;
            }
            case 0b000100: // SLLV
            {
                break;
            }
            case 0b000111: // SRAV
            {
                break;
            }
            case 0b000110: // SRLV
            {
                break;
            }
            case 0b001000: // JR
            {
                uint32_t sReg;
                tie(ignore, sReg, ignore, ignore) = decodeArithmeticType(instruction);
                pc = regs[sReg];
                break;
            }
            case 0b010001: // MTHI
            {
                break;
            }
            case 0b010011: // MTLO
            {
                break;
            }
            default:
            {
                cerr << "Invalid function code for ARITHLOG instruction: " << func << endl;
                assert(true);
                break;
            }
            }
            break;
        }
        case 0b001001: // ADDIU
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            regs[tReg] = regs[sReg] + immediate;
            pc += 4;
            break;
        }
        case 0b001100: // ANDI
        {
            break;
        }
        case 0b001101: // ORI
        {
            break;
        }
        case 0b001010: // SLTI
        {
            break;
        }
        case 0b001011: // SLTIU
        {
            break;
        }
        case 0b001110: // XORI
        {
            break;
        }
        case 0b000100: // BEQ
        {
            break;
        }
        case 0b000101: // BNE
        {
            break;
        }
        case 0b000111: // BGTZ
        {
            break;
        }
        case 0b000110: // BLEZ
        {
            break;
        }
        case 0b000001: // BLTZ
        {
            break;
        }
        // case 0b000001: // OTHER BRANCHZ
        // {
        //     break;
        // }
        case 0b100000: // LB
        {
            break;
        }
        case 0b100100: // LBU
        {
            break;
        }
        case 0b100001: // LH
        {
            break;
        }
        case 0b100101: // LHU
        {
            break;
        }
        case 0b100010: // LWL
        {
            break;
        }
        case 0b100110: // LWR
        {
            break;
        }
        case 0b100011: // LW
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            regs[tReg] = memory[regs[sReg] + immediate];
            pc += 4;
            break;
        }
        case 0b101000: // SB
        {
            break;
        }
        case 0b101001: // SH
        {
            break;
        }
        case 0b101011: // SW
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            memory[regs[sReg] + immediate] = regs[tReg];
            pc += 4;
            break;
        }
        case 0b000010: // J
        {
            break;
        }
        case 0b000011: // JAR
        {
            break;
        }
        default:
        {
            cerr << "Invalid opcode: " << opcode << endl;
            assert(true);
            break;
        }
        }
    }

    return regs[2]; // $v0 final value (register_v0 MIPS output)
}

tuple<uint32_t, uint32_t, uint32_t, uint32_t> decodeArithmeticType(uint32_t instruction)
{
    uint32_t constant = (instruction >> 6) & 0b11111;
    uint32_t dReg = (instruction >> 11) & 0b11111;
    uint32_t tReg = (instruction >> 16) & 0b11111;
    uint32_t sReg = (instruction >> 21) & 0b11111;

    return {dReg, sReg, tReg, constant};
}

tuple<uint32_t, uint32_t, uint32_t> decodeImmediateType(uint32_t instruction)
{
    uint32_t tReg = (instruction >> 16) & 0b11111;
    uint32_t sReg = (instruction >> 21) & 0b11111;
    uint32_t immediate = instruction & 0xFFFF;

    return {tReg, sReg, immediate};
}
