#include "blowfish.h"
#include "crypto_bf_bf_pi.h"
#include <utility> // for std::swap

Blowfish::Blowfish(const uint8_t* key, size_t key_len) {
    if (key_len < 4 || key_len > 56) {
        throw std::invalid_argument("Key length must be between 4 and 56 bytes");
    }
    initialize(key, key_len);
}

std::vector<uint8_t> Blowfish::encrypt(const std::vector<uint8_t> &plaintext) {

    std::vector<uint8_t> padded = pkcs7Pad(plaintext);
    std::vector<Block> blocks = bytesToBlocks(padded);

    for (Block &b : blocks) {
        encryptBlock(b.first, b.second);
    }

    return blocksToBytes(blocks);
}

std::vector<uint8_t> Blowfish::decrypt(const std::vector<uint8_t> &ciphertext) {

    if (ciphertext.size() % 8 != 0) 
        throw std::invalid_argument("Ciphertext length must be a multiple of 8 bytes");

    std::vector<Block> blocks = bytesToBlocks(ciphertext);

    for (Block &b : blocks) {
        decryptBlock(b.first, b.second);
    }

    std::vector<uint8_t> padded = blocksToBytes(blocks);
    
    return pkcs7Unpad(padded);
}

std::vector<uint8_t> Blowfish::encryptText(const std::string &plaintext) {

    std::vector<Block> blocks = textToBlocks(plaintext);

    for (Block &b : blocks)
        encryptBlock(b.first, b.second);

    return blocksToBytes(blocks);
}

std::string Blowfish::decryptText(const std::vector<uint8_t> &ciphertext) {

    std::vector<Block> blocks = bytesToBlocks(ciphertext);

    for (Block &b : blocks)
        decryptBlock(b.first, b.second);
    
    return blocksToString(blocks);
}

void Blowfish::initialize(const uint8_t* key, size_t key_len) {

    // Initialize P-array and S-boxes with initial values
    // those values are derived from the hexadecimal digits of pi (as per the Blowfish specification)
    // provided in crypto_bf_bf_pi.h
    for (int i = 0; i < 18; i++) {
        P[i] = bf_init.P[i];
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 256; j++) {
            S[i][j] = bf_init.S[i * 256 + j];
        }
    }

    // XOR the key with the P-array
    size_t key_index = 0;
    for (int i = 0; i < 18; i++) {
        uint32_t data = 0;
        // Combine 4 bytes of the key into a 32-bit word
        for (int j = 0; j < 4; j++) {
            // Shift the existing data left by 8 bits and add the next byte of the key
            data = (data << 8) | key[key_index];
            // Move to the next byte of the key, wrapping around if necessary
            key_index = (key_index + 1) % key_len;
        }
        P[i] ^= data;
    }

    uint32_t L = 0, R = 0;

    // Encrypt the all-zero block to further initialize the P-array and S-boxes
    for (int i = 0; i < 18; i += 2){

        encryptBlock(L, R);
        P[i] = L;
        P[i+1] = R;
    }

    for (int i = 0; i < 4; i++){

        for (int j = 0; j < 256; j += 2){

            encryptBlock(L, R);
            S[i][j] = L;
            S[i][j+1] = R;
        }
    }

}


uint32_t Blowfish::F(uint32_t x) {

    // Split the 32-bit input into four 8-bit segments
    uint8_t xarr[4] = {
        x >> 24,
        x >> 16,
        x >> 8,
        x
    };
    // Use the segments to index into the S-boxes and combine the results
    uint32_t result;
    result = S[0][xarr[0]] + S[1][xarr[1]];
    result ^= S[2][xarr[2]];
    result += S[3][xarr[3]];

    return result;
}

std::vector<Block> Blowfish::textToBlocks(const std::string &text) {

    std::vector<uint8_t> unpadded = std::vector<uint8_t>(text.begin(), text.end());
    std::vector<uint8_t> padded = pkcs7Pad(unpadded);

    return bytesToBlocks(padded);
}

std::vector<uint8_t> Blowfish::blocksToBytes(const std::vector<Block> &blocks)
{

    std::vector<uint8_t> result;

    for (Block b : blocks) {

        for (int i = 0; i < 4; i++) {

            result.push_back(static_cast<uint8_t>(b.first >> ((3 -i) * 8)));
        }

        for (int i = 0; i < 4; i++) {

            result.push_back(static_cast<uint8_t>(b.second >> ((3 - i) * 8)));
        }
    }

    return result;
}

std::vector<Block> Blowfish::bytesToBlocks(const std::vector<uint8_t> &bytes) {

    if (bytes.size() % 8 != 0) throw(std::invalid_argument("passed byte vector length must be a multiple of 8 for block conversion"));
    std::vector<Block> result;

    for (size_t i = 0; i < bytes.size(); i += 8) {

        Block b = {0, 0};

        // Convert the first 4 bytes into the left 32-bit half (big-endian)
        for (int j = 0; j < 4; j++) {
            b.first |= static_cast<uint32_t>(bytes[i + j]) << ((3 - j) * 8);
        }

        // Convert the next 4 bytes into the right 32-bit half (big-endian)
        for (int j = 0; j < 4; j++) {
            b.second |= static_cast<uint32_t>(bytes[i + 4 + j]) << ((3 - j) * 8);
        }

        result.push_back(b);

    }

    return result;
}

std::string Blowfish::blocksToString(const std::vector<Block> &blocks) {

    std::vector<uint8_t> bytes = blocksToBytes(blocks);
    std::vector<uint8_t> unpadded = pkcs7Unpad(bytes);

    return std::string(unpadded.begin(), unpadded.end());
}

std::vector<uint8_t> Blowfish::pkcs7Pad(const std::vector<uint8_t> &input)
{

    int blockSize = 8; // Blowfish block size in bytes
    int paddingLength = blockSize - (input.size() % blockSize);
    if (paddingLength == 0) {
        paddingLength = blockSize; // If input is already a multiple of block size, add a full block of padding
    }

    // Create a new vector that includes the original data followed by the padding bytes
    std::vector<uint8_t> output = std::vector<uint8_t>(input);
    output.insert(output.end(), paddingLength, static_cast<uint8_t>(paddingLength));
    
    return output;
}

std::vector<uint8_t> Blowfish::pkcs7Unpad(const std::vector<uint8_t> &input) {

    if (input.empty()) throw(std::invalid_argument("Input cannot be empty for unpadding"));
    int padLen = input.back();
    if (padLen <= 0 || padLen > 8) throw(std::invalid_argument("Invalid padding length"));

    // Last padLen bytes should all be equal to padLen
    for (size_t i = input.size() - padLen; i < input.size(); i++) {
        if (input[i] != padLen) throw(std::invalid_argument("Invalid padding bytes"));
    }
    if (input.size() < static_cast<size_t>(padLen)) throw(std::invalid_argument("Input is too short for the specified padding length"));

    std::vector<uint8_t> output(input.begin(), input.end() - padLen);
    
    return output;
}

void Blowfish::encryptBlock(uint32_t &L, uint32_t &R) {

    // 16 rounds of the Feistel network
    for (int round = 0; round < 16; round++){
        L ^= P[round];
        R ^= F(L);
        std::swap(L, R);
    }

    // Undo the final swap and apply the final P-array transformations
    std::swap(L, R);
    R ^= P[16];
    L ^= P[17];
}

void Blowfish::decryptBlock(uint32_t &L, uint32_t &R) {
    
    L ^= P[17];
    R ^= P[16];

    for (int round = 15; round >= 0; round--) {
        std::swap(L, R);
        L ^= F(R);
        R ^= P[round];
    }
    
    std::swap(L, R);
}