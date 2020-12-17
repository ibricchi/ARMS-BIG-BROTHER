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
    const uint32_t memInstructionStartIdx = 0xBFC00000 / 4; // MIPS supports byte addressing and reference simulator supports word addressing

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
            cerr << "Reference simulator: Line number " << currentMemAddress - memInstructionStartIdx << " : expected exactly eight chars, got '" << line << '"' << endl;
            exit(1);
        }

        for (int i = 0; i < line.size(); i++)
        {
            if (!isxdigit(line[i]))
            {
                cerr << "Reference simulator: Line number " << currentMemAddress - memInstructionStartIdx << " : expected only hexadecimal digits, got '" << line[i] << '"' << endl;
                exit(1);
            }
        }

        // Convert base 16 string to unsigned integer
        unsigned memLineData = stoul(line, nullptr, 16);
        assert(memLineData < 0xFFFFFFFF);

        assert(currentMemAddress <= maxUint32t / 4); // MIPS supports byte addressing and reference simulator supports word addressing
        memory[currentMemAddress] = memLineData;

        currentMemAddress++;
    }

    return memory;
}

uint32_t simulateMIPS(unordered_map<uint32_t, uint32_t> &memory, const uint32_t memInstructionStartIdx)
{
    uint32_t pc = memInstructionStartIdx;
    array<uint32_t, 32> regs{};
    uint32_t lo{};
    uint32_t hi{};

    simulateMIPSHelper(memory, pc, regs, lo, hi, memInstructionStartIdx);

    return regs[2]; // $v0 final value (register_v0 MIPS output)
}

void simulateMIPSHelper(unordered_map<uint32_t, uint32_t> &memory, uint32_t pc, array<uint32_t, 32> &regs, uint32_t &lo, uint32_t &hi, const uint32_t memInstructionStartIdx, bool isDelaySlot)
{
    const uint32_t maxUint32t = 0xFFFFFFFF;

    while (pc != 0) // attempting to execute address 0 causes the CPU to halt
    {
        assert(pc <= maxUint32t / 4 && pc > 0); // MIPS supports byte addressing and reference simulator supports word addressing

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
                pc++;
                break;
            }
            case 0b100100: // AND
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[sReg] & regs[tReg];
                pc++;
                break;
            }
            case 0b100101: // OR
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[sReg] | regs[tReg];
                pc++;
                break;
            }
            case 0b101010: // SLT
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                // Using signed values
                regs[dReg] = (static_cast<int32_t>(regs[sReg]) < static_cast<int32_t>(regs[tReg])) ? 1 : 0;
                pc++;
                break;
            }
            case 0b101011: // SLTU
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = (regs[sReg] < regs[tReg]) ? 1 : 0;
                pc++;
                break;
            }
            case 0b100011: // SUBU
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[sReg] - regs[tReg];
                pc++;
                break;
            }
            case 0b100110: // XOR
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[sReg] ^ regs[tReg];
                pc++;
                break;
            }
            case 0b011010: // DIV
            {
                uint32_t sReg, tReg;
                tie(ignore, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                // signed division
                lo = static_cast<int32_t>(regs[sReg]) / static_cast<int32_t>(regs[tReg]); // quotient
                hi = static_cast<int32_t>(regs[sReg]) % static_cast<int32_t>(regs[tReg]); // remainder
                pc++;
                break;
            }
            case 0b011011: // DIVU
            {
                uint32_t sReg, tReg;
                tie(ignore, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                lo = regs[sReg] / regs[tReg]; // quotient
                hi = regs[sReg] % regs[tReg]; // remainder
                pc++;
                break;
            }
            case 0b011000: // MULT
            {
                uint32_t sReg, tReg;
                tie(ignore, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                uint64_t result;
                // if result negative
                if (((regs[sReg] >> 31) && !(regs[tReg] >> 31)) || (!(regs[sReg] >> 31) && (regs[tReg] >> 31)))
                {
                    // signed multiplication
                    result = (int64_t)static_cast<int32_t>(regs[sReg]) * (int64_t)static_cast<int32_t>(regs[tReg]);
                }
                else
                {
                    result = regs[sReg] * regs[tReg];
                }
                lo = (result << 32) >> 32; // lower 32 bits
                hi = result >> 32;         // higher 32 bits
                pc++;
                break;
            }
            case 0b011001: // MULTU
            {
                uint32_t sReg, tReg;
                tie(ignore, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                uint64_t result = (uint64_t)regs[sReg] * (uint64_t)regs[tReg];
                lo = (result << 32) >> 32; // lower 32 bits
                hi = result >> 32;         // higher 32 bits
                pc++;
                break;
            }
            case 0b000000: // SLL
            {
                uint32_t dReg, tReg, constant;
                tie(dReg, ignore, tReg, constant) = decodeArithmeticType(instruction);
                regs[dReg] = regs[tReg] << constant;
                pc++;
                break;
            }
            case 0b000011: // SRA
            {
                uint32_t dReg, tReg, constant;
                tie(dReg, ignore, tReg, constant) = decodeArithmeticType(instruction);
                // >> always shifts in zero for unsigned types
                regs[dReg] = static_cast<int32_t>(regs[tReg]) >> constant;
                pc++;
                break;
            }
            case 0b000010: // SRL
            {
                uint32_t dReg, tReg, constant;
                tie(dReg, ignore, tReg, constant) = decodeArithmeticType(instruction);
                regs[dReg] = regs[tReg] >> constant;
                pc++;
                break;
            }
            case 0b000100: // SLLV
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[tReg] << regs[sReg];
                pc++;
                break;
            }
            case 0b000111: // SRAV
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                // >> always shifts in zero for unsigned types
                regs[dReg] = static_cast<int32_t>(regs[tReg]) >> regs[sReg];
                pc++;
                break;
            }
            case 0b000110: // SRLV
            {
                uint32_t dReg, sReg, tReg;
                tie(dReg, sReg, tReg, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = regs[tReg] >> regs[sReg];
                pc++;
                break;
            }
            case 0b001000: // JR
            {
                uint32_t sReg;
                tie(ignore, sReg, ignore, ignore) = decodeArithmeticType(instruction);
                simulateMIPSHelper(memory, pc + 1, regs, lo, hi, memInstructionStartIdx, true); // execute branch delay slot instruction
                // MIPS supports byte addressing and reference simulator supports word addressing
                pc = regs[sReg] / 4;
                break;
            }
            case 0b001001: // JALR
            {
                uint32_t dReg, sReg;
                tie(dReg, sReg, ignore, ignore) = decodeArithmeticType(instruction);
                simulateMIPSHelper(memory, pc + 1, regs, lo, hi, memInstructionStartIdx, true); // execute branch delay slot instruction
                uint32_t prevPc = pc;
                // MIPS supports byte addressing and reference simulator supports word addressing
                pc = regs[sReg] / 4;
                // address after branch delay slot (MIPS byte address rather than reference simulator word address)
                regs[dReg] = (prevPc + 2) * 4;
                break;
            }
            case 0b010000: // MFHI
            {
                uint32_t dReg;
                tie(dReg, ignore, ignore, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = hi;
                pc++;
                break;
            }
            case 0b010010: // MFLO
            {
                uint32_t dReg;
                tie(dReg, ignore, ignore, ignore) = decodeArithmeticType(instruction);
                regs[dReg] = lo;
                pc++;
                break;
            }
            case 0b010001: // MTHI
            {
                uint32_t sReg;
                tie(ignore, sReg, ignore, ignore) = decodeArithmeticType(instruction);
                hi = regs[sReg];
                pc++;
                break;
            }
            case 0b010011: // MTLO
            {
                uint32_t sReg;
                tie(ignore, sReg, ignore, ignore) = decodeArithmeticType(instruction);
                lo = regs[sReg];
                pc++;
                break;
            }
            default:
            {
                cerr << "Reference simulator: Invalid function code for ARITHLOG instruction: " << func << endl;
                exit(1);
                break;
            }
            }
            break;
        }
        case 0b001001: // ADDIU
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            int32_t signed_imm = ((int16_t)immediate);
            regs[tReg] = regs[sReg] + ((uint32_t)signed_imm);
            pc++;
            break;
        }
        case 0b001100: // ANDI
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            regs[tReg] = regs[sReg] & immediate;
            pc++;
            break;
        }
        case 0b001101: // ORI
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            regs[tReg] = regs[sReg] | immediate;
            pc++;
            break;
        }
        case 0b001010: // SLTI
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // Using signed values
            regs[tReg] = (static_cast<int32_t>(regs[sReg]) < static_cast<int16_t>(immediate)) ? 1 : 0;
            pc++;
            break;
        }
        case 0b001011: // SLTIU
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            int32_t signed_imm = ((int16_t)immediate);
            regs[tReg] = (regs[sReg] < ((uint32_t)signed_imm)) ? 1 : 0;
            pc++;
            break;
        }
        case 0b001110: // XORI
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            regs[tReg] = regs[sReg] ^ immediate;
            pc++;
            break;
        }
        case 0b000100: // BEQ
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            pc++;
            simulateMIPSHelper(memory, pc, regs, lo, hi, memInstructionStartIdx, true); // execute branch delay slot instruction
            if (regs[sReg] == regs[tReg])
            {
                // Use signed immediates as relative branches could be negative
                pc += (static_cast<int16_t>(immediate) << 2) / 4;
            }
            else
            {
                pc++; // already executed branch delay slot instruction
            }
            break;
        }
        case 0b000101: // BNE
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            pc++;
            simulateMIPSHelper(memory, pc, regs, lo, hi, memInstructionStartIdx, true); // execute branch delay slot instruction
            if (regs[sReg] != regs[tReg])
            {
                // Use signed immediates as relative branches could be negative
                pc += (static_cast<int16_t>(immediate) << 2) / 4;
            }
            else
            {
                pc++; // already executed branch delay slot instruction
            }
            break;
        }
        case 0b000111: // BGTZ
        {
            uint32_t sReg, immediate;
            tie(ignore, sReg, immediate) = decodeImmediateType(instruction);
            pc++;
            simulateMIPSHelper(memory, pc, regs, lo, hi, memInstructionStartIdx, true); // execute branch delay slot instruction
            if (static_cast<int16_t>(regs[sReg]) > 0)
            {
                // Use signed immediates as relative branches could be negative
                pc += (static_cast<int16_t>(immediate) << 2) / 4;
            }
            else
            {
                pc++; // already executed branch delay slot instruction
            }
            break;
        }
        case 0b000110: // BLEZ
        {
            uint32_t sReg, immediate;
            tie(ignore, sReg, immediate) = decodeImmediateType(instruction);
            pc++;
            simulateMIPSHelper(memory, pc, regs, lo, hi, memInstructionStartIdx, true); // execute branch delay slot instruction
            if (static_cast<int16_t>(regs[sReg]) <= 0)
            {
                // Use signed immediates as relative branches could be negative
                pc += (static_cast<int16_t>(immediate) << 2) / 4;
            }
            else
            {
                pc++; // already executed branch delay slot instruction
            }
            break;
        }
        case 0b000001: // OTHER BRANCHZ
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            pc++;
            simulateMIPSHelper(memory, pc, regs, lo, hi, memInstructionStartIdx, true); // execute branch delay slot instruction

            switch (tReg)
            {
            case 0b00000: // BLTZ
            {
                if (static_cast<int16_t>(regs[sReg]) < 0)
                {
                    // Use signed immediates as relative branches could be negative
                    pc += (static_cast<int16_t>(immediate) << 2) / 4;
                }
                else
                {
                    pc++; // already executed branch delay slot instruction
                }
                break;
            }
            case 0b00001: // BGEZ
            {
                if (static_cast<int16_t>(regs[sReg]) >= 0)
                {
                    // Use signed immediates as relative branches could be negative
                    pc += (static_cast<int16_t>(immediate) << 2) / 4;
                }
                else
                {
                    pc++; // already executed branch delay slot instruction
                }
                break;
            }
            case 0b10001: // BGEZAL
            {
                // return address is the instruction after the branch delay slot
                regs[31] = (pc + 1) * 4; // address after branch delay slot (MIPS byte address rather than reference simulator word address)
                if (static_cast<int16_t>(regs[sReg]) >= 0)
                {
                    // Use signed immediates as relative branches could be negative
                    pc += (static_cast<int16_t>(immediate) << 2) / 4;
                }
                else
                {
                    pc++; // already executed branch delay slot instruction
                }
                break;
            }
            case 0b10000: // BLTZAL
            {
                // return address is the instruction after the branch delay slot
                regs[31] = (pc + 1) * 4; // address after branch delay slot (MIPS byte address rather than reference simulator word address)
                if (static_cast<int16_t>(regs[sReg]) < 0)
                {
                    // Use signed immediates as relative branches could be negative
                    pc += (static_cast<int16_t>(immediate) << 2) / 4;
                }
                else
                {
                    pc++; // already executed branch delay slot instruction
                }
                break;
            }
            default:
            {
                cerr << "Reference simulator: Invalid tReg for OTHER BRANCHZ instruction. tReg: " << tReg << endl;
                exit(1);
                break;
            }
            }
            break;
        }
        case 0b100000: // LB
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // reference simulator memory supports word rather than byte addressing
            uint32_t word = memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4]; // will be rounded down
            uint32_t byteNr = (regs[sReg] + static_cast<int16_t>(immediate)) % 4;
            regs[tReg] = getByteFromWordSigned(word, byteNr);
            pc++;
            break;
        }
        case 0b100100: // LBU
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // reference simulator memory supports word rather than byte addressing
            uint32_t word = memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4]; // will be rounded down
            uint32_t byteNr = (regs[sReg] + static_cast<int16_t>(immediate)) % 4;
            regs[tReg] = getByteFromWord(word, byteNr);
            pc++;
            break;
        }
        case 0b100001: // LH
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // reference simulator memory supports word rather than byte addressing
            uint32_t word = memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4]; // will be rounded down
            uint32_t byteNr = (regs[sReg] + static_cast<int16_t>(immediate)) % 4;
            assert(byteNr == 0 || byteNr == 2); // needs to be half-word alligned
            uint32_t lowerByte = getByteFromWord(word, byteNr);
            uint32_t higherByte = getByteFromWordSigned(word, byteNr + 1);
            // combine lowerByte and higherByte into half-word
            regs[tReg] = higherByte << 8;
            regs[tReg] |= lowerByte;
            pc++;
            break;
        }
        case 0b100101: // LHU
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // reference simulator memory supports word rather than byte addressing
            uint32_t word = memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4]; // will be rounded down
            uint32_t byteNr = (regs[sReg] + static_cast<int16_t>(immediate)) % 4;
            assert(byteNr == 0 || byteNr == 2); // needs to be half-word alligned
            uint32_t lowerByte = getByteFromWord(word, byteNr);
            uint32_t higherByte = getByteFromWord(word, byteNr + 1);
            // combine lowerByte and higherByte into half-word
            regs[tReg] = higherByte << 8;
            regs[tReg] |= lowerByte;
            pc++;
            break;
        }
        case 0b001111: // LUI
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            regs[tReg] = regs[sReg] + (immediate << 16);
            pc++;
            break;
        }
        case 0b100010: // LWL
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // reference simulator memory supports word rather than byte addressing
            uint32_t byteNr = (regs[sReg] + static_cast<int16_t>(immediate)) % 4;
            uint32_t word, byte0, byte1, byte2, byte3; // Byte 3 is MSB
            word = memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4];
            byte0 = getByteFromWord(word, 0);
            byte1 = getByteFromWord(word, 1);
            byte2 = getByteFromWord(word, 2);
            byte3 = getByteFromWord(word, 3);
            switch (byteNr)
            {
            case 0:
                regs[tReg] = replaceByteInWord(regs[tReg], byte0, 3);
                break;
            case 1:
                regs[tReg] = replaceByteInWord(regs[tReg], byte0, 2);
                regs[tReg] = replaceByteInWord(regs[tReg], byte1, 3);
                break;
            case 2:
                regs[tReg] = replaceByteInWord(regs[tReg], byte0, 1);
                regs[tReg] = replaceByteInWord(regs[tReg], byte1, 2);
                regs[tReg] = replaceByteInWord(regs[tReg], byte2, 3);
                break;
            case 3:
                regs[tReg] = replaceByteInWord(regs[tReg], byte0, 0);
                regs[tReg] = replaceByteInWord(regs[tReg], byte1, 1);
                regs[tReg] = replaceByteInWord(regs[tReg], byte2, 2);
                regs[tReg] = replaceByteInWord(regs[tReg], byte3, 3);
                break;
            }
            pc++;
            break;
        }
        case 0b100110: // LWR
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // reference simulator memory supports word rather than byte addressing
            uint32_t byteNr = (regs[sReg] + static_cast<int16_t>(immediate)) % 4;
            uint32_t word, byte0, byte1, byte2, byte3;
            word = memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4];
            byte0 = getByteFromWord(word, 0);
            byte1 = getByteFromWord(word, 1);
            byte2 = getByteFromWord(word, 2);
            byte3 = getByteFromWord(word, 3);
            switch (byteNr)
            {
            case 0:
                regs[tReg] = replaceByteInWord(regs[tReg], byte0, 0);
                regs[tReg] = replaceByteInWord(regs[tReg], byte1, 1);
                regs[tReg] = replaceByteInWord(regs[tReg], byte2, 2);
                regs[tReg] = replaceByteInWord(regs[tReg], byte3, 3);
                break;
            case 1:
                regs[tReg] = replaceByteInWord(regs[tReg], byte1, 0);
                regs[tReg] = replaceByteInWord(regs[tReg], byte2, 1);
                regs[tReg] = replaceByteInWord(regs[tReg], byte3, 2);
                break;
            case 2:
                regs[tReg] = replaceByteInWord(regs[tReg], byte2, 0);
                regs[tReg] = replaceByteInWord(regs[tReg], byte3, 1);
                break;
            case 3:
                regs[tReg] = replaceByteInWord(regs[tReg], byte3, 0);
                break;
            }
            pc++;
            break;
        }
        case 0b100011: // LW
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // word address must be divisble by 4
            assert((regs[sReg] + static_cast<int16_t>(immediate)) % 4 == 0);
            // reference simulator memory supports word rather than byte addressing
            regs[tReg] = memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4];
            pc++;
            break;
        }
        case 0b101000: // SB
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // reference simulator memory supports word rather than byte addressing
            uint32_t word = memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4]; // will be rounded down
            uint32_t byteNr = (regs[sReg] + static_cast<int16_t>(immediate)) % 4;
            uint32_t byte = getByteFromWord(regs[tReg], 0); // select least significant byte
            word = replaceByteInWord(word, byte, byteNr);
            memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4] = word;
            pc++;
            break;
        }
        case 0b101001: // SH
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // reference simulator memory supports word rather than byte addressing
            uint32_t word = memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4]; // will be rounded down
            uint32_t byteNr = (regs[sReg] + static_cast<int16_t>(immediate)) % 4;
            assert(byteNr == 0 || byteNr == 2); // needs to be half-word alligned
            // half word is least significant bytes
            uint32_t halfWord = regs[tReg];
            uint32_t lowerByte = getByteFromWord(halfWord, 0);
            uint32_t higherByte = getByteFromWord(halfWord, 1);
            word = replaceByteInWord(word, lowerByte, byteNr);
            word = replaceByteInWord(word, higherByte, byteNr + 1);
            memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4] = word;
            pc++;
            break;
        }
        case 0b101011: // SW
        {
            uint32_t tReg, sReg, immediate;
            tie(tReg, sReg, immediate) = decodeImmediateType(instruction);
            // word address must be divisble by 4
            assert((regs[sReg] + static_cast<int16_t>(immediate)) % 4 == 0);
            // reference simulator memory supports word rather than byte addressing
            memory[(regs[sReg] + static_cast<int16_t>(immediate)) / 4] = regs[tReg];
            pc++;
            break;
        }
        case 0b000010: // J
        {
            uint32_t immediate = decodeJumpType(instruction);
            simulateMIPSHelper(memory, pc + 1, regs, lo, hi, memInstructionStartIdx, true); // execute branch delay slot instruction
            // Jump to PC[31:28],imm<<2
            pc = (pc * 4 >> 28) << 28; // clear lower 28 bits
            pc |= (immediate << 2);
            pc /= 4; // MIPS supports byte addressing and reference simulator supports word addressing
            break;
        }
        case 0b000011: // JAL
        {
            uint32_t immediate = decodeJumpType(instruction);
            simulateMIPSHelper(memory, pc + 1, regs, lo, hi, memInstructionStartIdx, true); // execute branch delay slot instruction
            regs[31] = (pc + 2) * 4;                                                        // address after branch delay slot (MIPS byte address rather than reference simulator word address)
            // Jump to PC[31:28],imm<<2
            pc = (pc * 4 >> 28) << 28; // clear lower 28 bits
            pc |= (immediate << 2);
            pc /= 4; // MIPS supports byte addressing and reference simulator supports word addressing
            break;
        }
        default:
        {
            cerr << "Reference simulator: Invalid opcode: " << opcode << endl;
            exit(1);
            break;
        }
        }

        if (isDelaySlot)
        {
            return;
        }
    }
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

uint32_t decodeJumpType(uint32_t instruction)
{
    uint32_t immediate = (instruction << 6) >> 6;

    return immediate;
}

uint32_t getByteFromWord(uint32_t word, uint32_t byteNr)
{
    assert(byteNr < 4);

    // shift desired byte all the way to the right
    uint32_t byte = word >> (byteNr * 8);
    // set everything to zero apart from desired byte
    byte &= 0xFF;

    return byte;
}

uint32_t getByteFromWordSigned(uint32_t word, uint32_t byteNr)
{
    uint32_t byte = getByteFromWord(word, byteNr);

    // check if need to sign extend
    if (byte & 0x80)
    {
        byte |= 0xFFFFFF00;
    }

    return byte;
}

uint32_t replaceByteInWord(uint32_t word, uint32_t byte, uint32_t byteNr)
{
    assert(byteNr < 4);
    assert(byte < 256);

    // construct byte filter
    uint32_t filter = 0xFF << (byteNr * 8);
    filter = ~filter;

    // shift byte to correct position
    byte <<= (byteNr * 8);

    // insert byte into word
    word &= filter;
    word |= byte;

    return word;
}
