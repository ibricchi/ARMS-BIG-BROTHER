#include "../asm_reference_simulator.hpp"

#include <cassert>

using namespace std;

void testGetByteFromWord()
{
    uint32_t word = 0x12345678;

    uint32_t byte0 = getByteFromWord(word, 0);
    assert(byte0 == 0x78);

    uint32_t byte1 = getByteFromWord(word, 1);
    assert(byte1 == 0x56);

    uint32_t byte2 = getByteFromWord(word, 2);
    assert(byte2 == 0x34);

    uint32_t byte3 = getByteFromWord(word, 3);
    assert(byte3 == 0x12);
}

void testGetByteFromWordSigned()
{
    uint32_t word = 0x12F45698;

    uint32_t byte0 = getByteFromWordSigned(word, 0);
    assert(byte0 == 0xFFFFFF98);

    uint32_t byte1 = getByteFromWordSigned(word, 1);
    assert(byte1 == 0x56);

    uint32_t byte2 = getByteFromWordSigned(word, 2);
    assert(byte2 == 0xFFFFFFF4);

    uint32_t byte3 = getByteFromWordSigned(word, 3);
    assert(byte3 == 0x12);
}

void testReplaceByteInWord()
{
    uint32_t byte = 0x36;
    uint32_t word = 0x12345678;

    uint32_t word0 = replaceByteInWord(word, byte, 0);
    assert(word0 == 0x12345636);

    uint32_t word1 = replaceByteInWord(word, byte, 1);
    assert(word1 == 0x12343678);

    uint32_t word2 = replaceByteInWord(word, byte, 2);
    assert(word2 == 0x12365678);

    uint32_t word3 = replaceByteInWord(word, byte, 3);
    assert(word3 == 0x36345678);
}

int main()
{
    // Carefull: First need to comment out main function in asm_reference_simulator.cpp

    testGetByteFromWord();
    cout << "getByteFromWord passed" << endl;

    testGetByteFromWordSigned();
    cout << "getByteFromWordSigned passed" << endl;

    testReplaceByteInWord();
    cout << "replaceByteInWord passed" << endl;

    cout << "All tests passed!" << endl;
}
