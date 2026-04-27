#include "md5.h"

MD5::MD5()
{
    reset();
}

std::string MD5::calculate(const std::vector<uint8_t> &inputBytes)
{
    reset();
    std::vector<block> blocks = inputToBlocks(inputBytes);
    mainLoop(blocks);
    std::string result;

    // Convert final state to little-endian byte string
    for (word_t word : {currentState.A, currentState.B, currentState.C, currentState.D})
    {
        for (int i = 0; i < 4; ++i)
        {
            uint8_t byte = (word >> (8 * i)) & 0xFF;
            result.push_back(static_cast<char>(byte));
        }
    }
    // Convert raw 16-byte digest to lowercase hexadecimal string
    static const char hexChars[] = "0123456789abcdef";
    std::string hexResult;
    hexResult.reserve(result.size() * 2);
    for (unsigned char byte : result)
    {
        hexResult.push_back(hexChars[(byte >> 4) & 0xF]);
        hexResult.push_back(hexChars[byte & 0xF]);
    }
    return hexResult;
}

std::string MD5::calculate(const std::string &inputText)
{
    std::vector<uint8_t> inputBytes = stringToBytes(inputText);
    return calculate(inputBytes);
}

void MD5::reset()
{
    currentState = initialState;
}

std::vector<uint8_t> MD5::stringToBytes(const std::string &inputText)
{
    std::vector<uint8_t> output = std::vector<uint8_t>();
    output.reserve(inputText.length());
    for (char c : inputText)
    {
        output.push_back(static_cast<uint8_t>(c));
    }
    return output;
}

std::vector<block> MD5::inputToBlocks(const std::vector<uint8_t> &inputBytes)
{
    std::vector<block> output = std::vector<block>();
    // Prepare padded message per RFC 1321 (MD5)
    std::vector<uint8_t> padded = inputBytes;

    // message length in bits (64-bit little-endian appended later)
    uint64_t bitLen = static_cast<uint64_t>(inputBytes.size()) * 8u;

    // append 0x80 byte
    padded.push_back(0x80);

    // pad with zeros until length (mod 64) == 56 bytes
    while (padded.size() % 64 != 56)
    {
        padded.push_back(0x00);
    }

    // append original length in bits as 64-bit little-endian
    for (int i = 0; i < 8; ++i)
    {
        // append least significant byte of bitLen, then shift right by 8 bits for next byte
        padded.push_back(static_cast<uint8_t>((bitLen >> (8 * i)) & 0xFF));
    }

    // split into 512-bit (64-byte) blocks, each block is 16 words (32-bit little-endian)
    size_t nBlocks = padded.size() / 64;
    output.reserve(nBlocks);
    for (size_t b = 0; b < nBlocks; ++b)
    {
        block blk;
        size_t base = b * 64;
        for (int i = 0; i < 16; ++i)
        {
            size_t idx = base + i * 4;
            uint32_t w = static_cast<uint32_t>(padded[idx]) |
                         (static_cast<uint32_t>(padded[idx + 1]) << 8) |
                         (static_cast<uint32_t>(padded[idx + 2]) << 16) |
                         (static_cast<uint32_t>(padded[idx + 3]) << 24);
            blk[i] = static_cast<word_t>(w);
        }
        output.push_back(blk);
    }

    return output;
}

void MD5::mainLoop(const std::vector<block> &blocks)
{
    for (const block &blk : blocks)
    {
        ingestBlock(blk);
    }
}

void MD5::ingestBlock(const block &blk)
{
    // Initialize working variables with current state
    word_t a = currentState.A;
    word_t b = currentState.B;
    word_t c = currentState.C;
    word_t d = currentState.D;

    for (int i = 0; i < 64; ++i)
    {
        // F - nonlinear function, g - index of word in block to use
        word_t F;
        int g;
        if (i < 16)
        {
            F = (b & c) | ((~b) & d);
            g = i;
        }
        else if (i < 32)
        {
            F = (d & b) | ((~d) & c);
            g = (5 * i + 1) % 16;
        }
        else if (i < 48)
        {
            F = b ^ c ^ d;
            g = (3 * i + 5) % 16;
        }
        else
        {
            F = c ^ (b | (~d));
            g = (7 * i) % 16;
        }

        word_t temp = a + F + k[i] + blk[g];
        a = d;
        d = c;
        c = b;
        b = b + rotate(temp, r[i]);
    }

    // Add this chunk's hash to result so far
    currentState.A += a;
    currentState.B += b;
    currentState.C += c;
    currentState.D += d;

}

word_t MD5::rotate(word_t input, int num)
{
    return (input << num) | (input >> (32 - num));
}
