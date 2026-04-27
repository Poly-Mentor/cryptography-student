#include "md5.h"

MD5::MD5()
{
    reset();
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
