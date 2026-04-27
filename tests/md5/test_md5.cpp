#define private public
#include "../../src/md5/md5.h"
#undef private

#include <iostream>
#include <string>

static void fail(const std::string &msg) {
    std::cout << "FAIL: " << msg << "\n";
    std::exit(1);
}

static void fail(const std::string &msg, std::string got, std::string expected) {
    std::cout << "FAIL: " << msg << "\n";
    std::cout << "Got: " << got << "\nExpected: " << expected << "\n";
    std::exit(1);
}

static void pass(const std::string &msg) {
    std::cout << "PASS: " << msg << "\n";
}



void stringToBytes_unittest()
{

    const std::string input = "abc";
    std::vector<uint8_t> output = MD5::stringToBytes(input);
    std::vector<uint8_t> expectedOutput = {0x61, 0x62, 0x63}; // ASCII values of 'a', 'b', 'c'
    if (output.size() != expectedOutput.size())
    {
        fail("stringToBytes output size mismatch", std::to_string(output.size()), std::to_string(expectedOutput.size()));
    }

    for (size_t i = 0; i < input.size(); ++i)
    {
        if (output[i] != expectedOutput[i])
        {
            fail("stringToBytes output mismatch at index " + std::to_string(i), std::to_string(output[i]), std::to_string(expectedOutput[i]));
        }
    }
    pass("stringToBytes produces correct byte vector");
}

void inputToBlocks_unittest()
{
    const std::vector<uint8_t> inputBytes = {0x61, 0x62, 0x63}; // "abc"
    std::vector<block> blocks = MD5::inputToBlocks(inputBytes);
    
    // Expected block (little-endian words): "abc" + padding + length
    // bytes: 0x61 0x62 0x63 0x80 -> word0 = 0x80636261
    // length in bits (24) is placed in word index 14 (low word), word 15 = high word = 0
    block expectedBlock = {0x80636261, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0};
    if (blocks.size() != 1)
    {
        fail("inputToBlocks output mismatch", std::to_string(blocks.size()) + " blocks", "1 block with specific content");
    }
    
    if (blocks[0] != expectedBlock)
    {
        std::string gotBlockStr, expectedBlockStr;
        for (int i = 0; i < 16; ++i) {
            gotBlockStr += "0x" + std::to_string(blocks[0][i]) + " ";
            expectedBlockStr += "0x" + std::to_string(expectedBlock[i]) + " ";
        }
        fail("inputToBlocks block content mismatch for 'abc'", gotBlockStr, expectedBlockStr);
    }
    pass("inputToBlocks produces correct block vector for 'abc'");

    std::vector<uint8_t> inputBytes2;

    for (uint8_t i = 0; i < 64; ++i) {
        inputBytes2.push_back(i);
    }

    // First block should contain the 64 input bytes packed as little-endian words
    block expectedBlock2 = {
        0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C,
        0x13121110, 0x17161514, 0x1B1A1918, 0x1F1E1D1C,
        0x23222120, 0x27262524, 0x2B2A2928, 0x2F2E2D2C,
        0x33323130, 0x37363534, 0x3B3A3938, 0x3F3E3D3C
    };

    std::vector<block> blocks2 = MD5::inputToBlocks(inputBytes2);
    if (blocks2.size() != 2)
    {
        fail("inputToBlocks for 64-byte input should produce 2 blocks (data + padding)");
    }
    if (blocks2[0] != expectedBlock2)
    {
        fail("inputToBlocks first block mismatch for 64-byte input", std::to_string(blocks2[0][0]), std::to_string(expectedBlock2[0]));
    }

    // Second block should be padding: 0x80 at first byte, zeros, and length (512 bits) at index 14
    block expectedPad = {0x00000080, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 512, 0};
    if (blocks2[1] != expectedPad)
    {
        fail("inputToBlocks padding block mismatch for 64-byte input", std::to_string(blocks2[1][0]), std::to_string(expectedPad[0]));
    }
    pass("inputToBlocks produces correct blocks for 64-byte input");
}

// -----------------------------------------------------------------------------------------

// void rotate_unittest()
// {
//     word_t input = 0x12345678;
//     int num = 4;
//     word_t expectedOutput = 0x23456781; // Rotate left by 4 bits
//     word_t output = MD5::rotate(input, num);
    
//     if (output != expectedOutput)
//     {
//         fail("rotate output mismatch: got " + std::to_string(output) + " expected " + std::to_string(expectedOutput));
//     }
//     pass("rotate produces correct output for input 0x12345678 rotated by 4 bits");

//     // ------------------------------------------------------------------------------------------

//     input = 0xAAAA5555;
//     num = 16;
//     word_t expectedOutput2 = 0x5555AAAA; // Rotate left by 16 bits
//     word_t output2 = MD5::rotate(input, num);

//     if (output2 != expectedOutput2)
//     {
//         fail("rotate output mismatch: got " + std::to_string(output2) + " expected " + std::to_string(expectedOutput2));
//     }
//     pass("rotate produces correct output for input 0xAAAA5555 rotated by 16 bits");
// }

int main(){
    stringToBytes_unittest();
    inputToBlocks_unittest();
    // rotate_unittest();
    return 0;
}
