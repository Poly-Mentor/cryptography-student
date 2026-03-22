#pragma once
#include <string>
#include <cstdint>
#include <stdexcept>
#include <vector>


using Block = std::pair<uint32_t, uint32_t>; // Represents a 64-bit block as two 32-bit halves (L and R)

class Blowfish {

    public:
        /** Constructor that takes a key and its length
         * @param key The encryption key
         * @param key_len The length of the key in bytes (must be between 4 and 56 bytes)
         * @throws std::invalid_argument if the key length is not within the valid range
         */
        Blowfish(const std::vector<uint8_t>& key);

        // Binary-oriented API
        std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext);
        std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext);

        // Text-oriented API
        /**
         * Encrypts a plaintext string and returns the ciphertext as a vector of bytes.
         */
        std::vector<uint8_t> encryptText(const std::string& plaintext);

        /**
         * Decrypts a ciphertext represented as a vector of bytes and returns the resulting plaintext string.
         */
        std::string decryptText(const std::vector<uint8_t>& ciphertext);

    private:

        /**
         * Procedure for initializing the P-array and S-boxes with the provided key.
         * This involves:
         * 1. Initializing the P-array and S-boxes with the hexadecimal digits of pi (as per the Blowfish specification).
         * 2. XORing the key with the P-array. The key is repeated as necessary to fill the entire P-array.
         */
        void initialize(const std::vector<uint8_t>& key);

        /**
         * Encrypts a single 64-bit block of data (split into two 32-bit halves, L and R) using the Blowfish algorithm.
         * Operations are performed in-place - input values of L and R are modified to contain the encrypted output.
         * @param L The left half of the block (32 bits)
         * @param R The right half of the block (32 bits)
         */
        void encryptBlock(uint32_t& L, uint32_t& R);

        /**
         * Decrypts a single 64-bit block of data (split into two 32-bit halves, L and R) using the Blowfish algorithm.
         * Operations are performed in-place - input values of L and R are modified to contain the decrypted output.
         * @param L The left half of the block (32 bits)
         * @param R The right half of the block (32 bits)
         */
        void decryptBlock(uint32_t& L, uint32_t& R);

        /**
         * F-function used in the Blowfish algorithm, which takes a 32-bit input and produces a 32-bit output based on the S-boxes.
         */
        uint32_t F(uint32_t x);

        // Helper functions for block processing and padding

        /**
         * Splits a string into 64-bit blocks (two 32-bit halves) for encryption/decryption.
         * @param text The input string to be split into blocks
         * @return A vector of Block pairs representing the 64-bit blocks derived from the input string
         */
        static std::vector<Block> textToBlocks(const std::string &text);

        /**
         * Joins a vector of 32-bit half-blocks back into a vector of bytes after encryption/decryption.
         * Used for reassembling decrypted data.
         * @param blocks a vector of 32-bit pairs representing 64-bit blocks
         * @return The resulting vector of bytes after joining the blocks together
         */
        static std::vector<uint8_t> blocksToBytes(const std::vector<Block> &blocks);

        /**
         * Splits a vector of bytes into 64-bit blocks (two 32-bit halves) for encryption/decryption.
         * Vector must have a length that is a multiple of 8 bytes (the block size for Blowfish) - ensure to apply padding before calling this function if necessary.
         * @param bytes The input vector of bytes to be split into blocks
         * @return A vector of Block pairs representing the 64-bit blocks derived from the input
         */
        static std::vector<Block> bytesToBlocks(const std::vector<uint8_t> &bytes);

        /**
         * Joins a vector of 32-bit half-blocks back into a string after encryption/decryption.
         * Used for reassembling decrypted text.
         * @param blocks a vector of 32-bit pairs representing 64-bit blocks
         * @return The resulting string after joining the blocks together
         */
        static std::string blocksToString(const std::vector<Block> &blocks);

        /**
         * Applies PKCS#7 padding to a vector of bytes to ensure its length is a multiple of the block size (8 bytes for Blowfish).
         * @param input The input vector of bytes to be padded
         * @return A new vector of bytes with PKCS#7 padding applied
         */
        static std::vector<uint8_t> pkcs7Pad(const std::vector<uint8_t> &input);

        /**
         * Removes PKCS#7 padding from a vector of bytes after decryption.
         * @param input The input vector of bytes from which padding should be removed
         * @return A new vector of bytes with PKCS#7 padding removed
         */
        static std::vector<uint8_t> pkcs7Unpad(const std::vector<uint8_t> &input);
        
        // key-initialized P-array and S-boxes
        uint32_t P[18];
        uint32_t S[4][256];
};