#include "asm_reference_simulator.hpp"

#include <array>
#include <tuple>
#include <cassert>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

int main()
{
    const uint32_t memInstructionStartIdx = 0xBFC00000;

    unordered_map<uint32_t, uint32_t> memory = readMemoryBinary(cin, memInstructionStartIdx);

    uint32_t register_v0 = simulateMIPS(memory, memInstructionStartIdx);

    // cout final value stored in register $v0
    // this should be used as the reference output
    cout << register_v0 << endl;
}

unordered_map<uint32_t, uint32_t> readMemoryBinary(istream &src, const uint32_t memInstructionStartIdx)
{
    const uint32_t maxUint32t = 0xFFFFFFFF;
    unordered_map<uint32_t, uint32_t> memory;

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

uint32_t simulateMIPS(unordered_map<uint32_t, uint32_t> &memory, const uint32_t memInstructionStartIdx)
{
    const uint32_t maxUint32t = 0xFFFFFFFF;

    uint32_t pc = memInstructionStartIdx;
    array<uint32_t, 32> regs{};
    uint32_t lo{};
    uint32_t hi{};

    while (pc != 0) // attempting to execute address 0 causes the CPU to halt
    {
        assert(pc <= maxUint32t && pc > 0);

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
            case 0b100001: // ADDU
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[sReg] + regs[tReg];
                pc += 4;
                break;
            }
            case 0b100100: // AND
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[sReg] & regs[tReg];
                pc += 4;
                break;
            }
            case 0b100101: // OR
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[sReg] | regs[tReg];
                pc += 4;
                break;
            }
            case 0b101010: // SLT
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                // Using signed values
                regs[dReg] = (static_cast<int32_t>(regs[sReg]) < static_cast<int32_t>(regs[tReg])) ? 1 : 0;
                pc += 4;
                break;
            }
            case 0b101011: // SLTU
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = (regs[sReg] < regs[tReg]) ? 1 : 0;
                pc += 4;
                break;
            }
            case 0b100011: // SUBU
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[sReg] - regs[tReg];
                pc += 4;
                break;
            }
            case 0b100110: // XOR
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[sReg] ^ regs[tReg];
                pc += 4;
                break;
            }
            case 0b011010: // DIV
            {
                uint32_t sReg, tReg;
                tie(ignore, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                // signed division
                lo = static_cast<int32_t>(regs[sReg]) / static_cast<int32_t>(regs[tReg]); // quotient
                hi = static_cast<int32_t>(regs[sReg]) % static_cast<int32_t>(regs[tReg]); // remainder
                pc += 4;
                break;
            }
            case 0b011011: // DIVU
            {
                uint32_t sReg, tReg;
                tie(ignore, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                lo = regs[sReg] / regs[tReg]; // quotient
                hi = regs[sReg] % regs[tReg]; // remainder
                pc += 4;
                break;
            }
            case 0b011000: // MULT
            {
                uint32_t sReg, tReg;
                tie(ignore, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                // signed multiplication
                lo = static_cast<int32_t>(regs[sReg]) * static_cast<int32_t>(regs[tReg]);
                pc += 4;
                break;
            }
            case 0b011001: // MULTU
            {
                uint32_t sReg, tReg;
                tie(ignore, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                lo = regs[sReg] * regs[tReg];
                pc += 4;
                break;
            }
            case 0b000000: // SLL
            {
                uint32_t dReg, tReg, constant;
                tie(dReg, ignore, tReg, constant) = decodeArithmeticType(instruction);
                regs[dReg] = regs[tReg] << constant;
                pc += 4;
                break;
            }
            case 0b000011: // SRA
            {
                uint32_t dReg, tReg, constant;
                tie(dReg, ignore, tReg, constant) = decodeArithmeticType(instruction);
                // >> always shifts in zero for unsigned types
                regs[dReg] = static_cast<int32_t>(regs[tReg]) >> constant;
                pc += 4;
                break;
            }
            case 0b000010: // SRL
            {
                uint32_t dReg, tReg, constant;
                tie(dReg, ignore, tReg, constant) = decodeArithmeticType(instruction);
                regs[dReg] = regs[tReg] >> constant;
                pc += 4;
                break;
            }
            case 0b000100: // SLLV
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[tReg] << regs[sReg];
                pc += 4;
                break;
            }
            case 0b000111: // SRAV
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                // >> always shifts in zero for unsigned types
                regs[dReg] = static_cast<int32_t>(regs[tReg]) >> regs[sReg];
                pc += 4;
                break;
            }
            case 0b000110: // SRLV
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[tReg] >> regs[sReg];
                pc += 4;
                break;
            }
            case 0b001000: // JR
            {
                uint32_t sReg;
                tie(ignore, sReg, ignore, ignore) = decodeArithmeticType(instruction);
                pc = regs[sReg];
                break;
            }
            case 0b010000: // MFHI
            {
                uint32_t dReg;
                tie(dReg, ignore, ignore, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = hi;
                pc += 4;
                break;
            }
            case 0b010010: // MFLO
            {
                uint32_t dReg;
                tie(dReg, ignore, ignore, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = lo;
                pc += 4;
                break;
            }
            case 0b010001: // MTHI
            {
                uint32_t sReg;
                tie(ignore, sReg, ignore, ignore) = decodeArithmeticType(instruction);
                hi = regs[sReg];
                pc += 4;
                break;
            }
            case 0b010011: // MTLO
            {
                uint32_t sReg;
                tie(ignore, sReg, ignore, ignore) = decodeArithmeticType(instruction);
                lo = regs[sReg];
                pc += 4;
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
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            regs[tReg] = regs[sReg] & immediate;
            pc += 4;
            break;
        }
        case 0b001101: // ORI
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            regs[tReg] = regs[sReg] | immediate;
            pc += 4;
            break;
        }
        case 0b001010: // SLTI
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // Using signed values
            regs[tReg] = (static_cast<int32_t>(regs[sReg]) < static_cast<int32_t>(immediate)) ? 1 : 0;
            pc += 4;
            break;
        }
        case 0b001011: // SLTIU
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            regs[tReg] = (regs[sReg] < immediate) ? 1 : 0;
            pc += 4;
            break;
        }
        case 0b001110: // XORI
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            regs[tReg] = regs[sReg] ^ immediate;
            pc += 4;
            break;
        }
        case 0b000100: // BEQ
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            pc += 4;
            if (regs[sReg] == regs[tReg])
            {
                // Use signed immediates as relative branches could be negative
                pc += static_cast<int32_t>(immediate) << 2;
            }
            break;
        }
        case 0b000101: // BNE
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            pc += 4;
            if (regs[sReg] != regs[tReg])
            {
                // Use signed immediates as relative branches could be negative
                pc += static_cast<int32_t>(immediate) << 2;
            }
            break;
        }
        case 0b000111: // BGTZ
        {
            uint32_t sReg, immediate;
            tie(ignore, sReg, immediate) = decodeImmediateType(instruction);
            pc += 4;
            if (regs[sReg] > 0)
            {
                // Use signed immediates as relative branches could be negative
                pc += static_cast<int32_t>(immediate) << 2;
            }
            break;
        }
        case 0b000110: // BLEZ
        {
            uint32_t sReg, immediate;
            tie(ignore, sReg, immediate) = decodeImmediateType(instruction);
            pc += 4;
            if (regs[sReg] <= 0)
            {
                // Use signed immediates as relative branches could be negative
                pc += static_cast<int32_t>(immediate) << 2;
            }
            break;
        }
        case 0b000001: // OTHER BRANCHZ
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);

            switch (tReg)
            {
            case 0b00000: // BLTZ
            {
                pc += 4;
                if (regs[sReg] < 0)
                {
                    // Use signed immediates as relative branches could be negative
                    pc += static_cast<int32_t>(immediate) << 2;
                }
                break;
            }
            case 0b00001: // BGEZ
            {
                pc += 4;
                if (regs[sReg] >= 0)
                {
                    // Use signed immediates as relative branches could be negative
                    pc += static_cast<int32_t>(immediate) << 2;
                }
                break;
            }
            case 0b10001: // BGEZAL
            {
                pc += 4;
                regs[31] = pc;
                if (regs[sReg] >= 0)
                {
                    // Use signed immediates as relative branches could be negative
                    pc += static_cast<int32_t>(immediate) << 2;
                }
                break;
            }
            case 0b10000: // BLTZAL
            {
                pc += 4;
                regs[31] = pc;
                if (regs[sReg] < 0)
                {
                    // Use signed immediates as relative branches could be negative
                    pc += static_cast<int32_t>(immediate) << 2;
                }
                break;
            }
            default:
            {
                cerr << "Invalid tReg for OTHER BRANCHZ instruction: " << tReg << endl;
                assert(true);
                break;
            }
            }
            break;
        }
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
            uint32_t immediate;
            tie(ignore, ignore, immediate) = decodeImmediateType(instruction);
            pc = immediate << 2;
            break;
        }
        case 0b000011: // JAR
        {
            regs[31] = pc + 4;
            uint32_t immediate;
            tie(ignore, ignore, immediate) = decodeImmediateType(instruction);
            pc = immediate << 2;
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
