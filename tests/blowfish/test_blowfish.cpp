#include <iostream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <cstring>

#define private public
#include "../../src/blowfish/blowfish.h"
#undef private

static void fail(const std::string &msg) {
    std::cout << "FAIL: " << msg << "\n";
    std::exit(1);
}

static void pass(const std::string &msg) {
    std::cout << "PASS: " << msg << "\n";
}

int main() {

    // Test 1: constructor throws for invalid key lengths and doesn't throw for valid
    try {
        std::vector<uint8_t> k0; // length 0
        try { Blowfish bf(k0); fail("ctor accepted key length 0"); } catch (const std::invalid_argument&) {}
        k0 = {0};
        try { Blowfish bf(k0); fail("ctor accepted key length 1"); } catch (const std::invalid_argument&) {}
        k0 = {0,1,2};
        try { Blowfish bf(k0); fail("ctor accepted key length 3"); } catch (const std::invalid_argument&) {}

        std::vector<uint8_t> k4 = {0,1,2,3};
        try { Blowfish bf(k4); } catch (...) { fail("ctor rejected valid key length 4"); }

        std::vector<uint8_t> k56(56);
        for (int i=0;i<56;i++) k56[i]=uint8_t(i);
        try { Blowfish bf2(k56); } catch (...) { fail("ctor rejected valid key length 56"); }

        std::vector<uint8_t> k57(57);
        for (int i=0;i<57;i++) k57[i]=uint8_t(i);
        try { Blowfish bf3(k57); fail("ctor accepted key length 57"); } catch (const std::invalid_argument&) {}
        pass("constructor validation");
    } catch (const std::exception &e) {
        fail(std::string("constructor test threw unexpected exception: ") + e.what());
    }

    // Create a Blowfish instance for subsequent tests
    Blowfish bf("testkey1");

    // Test 2: pkcs7Pad/unpad roundtrip for sizes 0,1,7,8,9 and invalid padding detection
    try {
        std::vector<size_t> sizes = {0,1,7,8,9};
        for (size_t s : sizes) {
            std::vector<uint8_t> data(s);
            for (size_t i=0;i<s;i++) data[i]=uint8_t(i+1);
            auto padded = Blowfish::pkcs7Pad(data);
            auto unpadded = Blowfish::pkcs7Unpad(padded);
            if (unpadded != data) fail("pkcs7 pad/unpad mismatch for size " + std::to_string(s));
        }
        // invalid padding detection - construct a vector where last byte indicates padLen but bytes don't match
        std::vector<uint8_t> bad = {0,0,0,0,4,4,4,3}; // last byte 3 but preceding padding bytes not all 3
        try {
            auto r = Blowfish::pkcs7Unpad(bad);
            (void)r;
            fail("pkcs7Unpad accepted invalid padding");
        } catch (const std::invalid_argument&) {
            // expected
        }
        // empty input should throw
        try {
            std::vector<uint8_t> empty;
            auto r = Blowfish::pkcs7Unpad(empty);
            (void)r;
            fail("pkcs7Unpad accepted empty input");
        } catch (const std::invalid_argument&) {
            // expected
        }
        pass("pkcs7 pad/unpad and invalid padding detection");
    } catch (const std::exception &e) {
        fail(std::string("pkcs7 tests threw unexpected exception: ") + e.what());
    }

    // Test 3: encryptBlock followed by decryptBlock returns original L,R for some sample values
    try {
        std::vector<std::pair<uint32_t,uint32_t>> samples = {
            {0x00000000u, 0x00000000u},
            {0x12345678u, 0x9ABCDEF0u},
            {0xFFFFFFFFu, 0x00000000u},
            {0xDEADBEEFu, 0xCAFEBABEu}
        };
        for (auto &p : samples) {
            uint32_t L = p.first, R = p.second;
            uint32_t origL = L, origR = R;
            bf.encryptBlock(L, R);
            if (origL == 0 && origR == 0) {
                std::cout << "DEBUG: after encrypt L=" << std::hex << L << " R=" << R << std::dec << "\n";
            }
            bf.decryptBlock(L, R);
            if (origL == 0 && origR == 0) {
                std::cout << "DEBUG: after decrypt L=" << std::hex << L << " R=" << R << std::dec << "\n";
            }
            if (L != origL || R != origR) {
                fail("encryptBlock/decryptBlock roundtrip failed for sample (" +
                     std::to_string(origL) + "," + std::to_string(origR) + ")");
            }
        }
        pass("encryptBlock/decryptBlock roundtrip");
    } catch (const std::exception &e) {
        fail(std::string("block encrypt/decrypt tests threw unexpected exception: ") + e.what());
    }

    // Test 4: F is deterministic (same input -> same output)
    try {
        std::vector<uint32_t> vals = {0u, 1u, 0x7FFFFFFFu, 0x80000000u, 0xDEADBEEFu};
        for (uint32_t v : vals) {
            uint32_t a = bf.F(v);
            uint32_t b = bf.F(v);
            if (a != b) fail("F not deterministic for input " + std::to_string(v));
        }
        pass("F determinism");
    } catch (const std::exception &e) {
        fail(std::string("F tests threw unexpected exception: ") + e.what());
    }

    // Test 5: bytesToBlocks should split bytes into 32-bit halves in big-endian order
    try {
        std::vector<uint8_t> input = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77};
        auto blocks = Blowfish::bytesToBlocks(input);
        if (blocks.size() != 1) fail("bytesToBlocks should produce exactly 1 block for 8 bytes");
        if (blocks[0].first != 0x00112233u || blocks[0].second != 0x44556677u) {
            fail("bytesToBlocks output mismatch: got (" + std::to_string(blocks[0].first) + "," + std::to_string(blocks[0].second) + ") expected (0x00112233,0x44556677)");
        }

        // Multi-block check
        input = {0xAA,0xBB,0xCC,0xDD,0x11,0x22,0x33,0x44, 0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC};
        blocks = Blowfish::bytesToBlocks(input);
        if (blocks.size() != 2) fail("bytesToBlocks should produce exactly 2 blocks for 16 bytes");
        if (blocks[0].first != 0xAABBCCDDu || blocks[0].second != 0x11223344u) fail("bytesToBlocks first block mismatch");
        if (blocks[1].first != 0x55667788u || blocks[1].second != 0x99AABBCCu) fail("bytesToBlocks second block mismatch");

        pass("bytesToBlocks block splitting");
    } catch (const std::exception &e) {
        fail(std::string("bytesToBlocks test threw unexpected exception: ") + e.what());
    }

    // Test 6: blocksToBytes and bytesToBlocks roundtrip
    try {
        std::vector<uint8_t> original = {
            0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
            0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x01,0x02
        };
        auto blocks = Blowfish::bytesToBlocks(original);
        auto roundtrip = Blowfish::blocksToBytes(blocks);
        if (roundtrip != original) fail("blocksToBytes(bytesToBlocks(x)) should equal x");

        pass("blocksToBytes/bytesToBlocks roundtrip");
    } catch (const std::exception &e) {
        fail(std::string("blocksToBytes roundtrip test threw unexpected exception: ") + e.what());
    }

    // Test 7: textToBlocks and blocksToString roundtrip
    try {
        std::vector<std::string> testStrings = {
            "Hello, Blowfish! ＾_＾",
            "",                      // empty string (padding only)
            "12345678",              // exactly one block
            "1234567",               // one byte short
            "The quick brown fox jumps over the lazy dog",
            "\n\tSpecial chars: !@#$%^&*()_+{}[]<>?",
            "Polish: ąćęłńóśżź"
        };

        for (const auto &text : testStrings) {
            auto blocks = bf.textToBlocks(text);
            auto out = bf.blocksToString(blocks);
            if (out != text) fail("blocksToString(textToBlocks(text)) should equal original string for '" + text + "'");
        }

        pass("textToBlocks/blocksToString roundtrip");
    } catch (const std::exception &e) {
        fail(std::string("textToBlocks/blocksToString roundtrip test threw unexpected exception: ") + e.what());
    }

    // Test 8: encrypt/decrypt roundtrip on raw bytes and text
    try {
        std::vector<uint8_t> plaintext = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
        auto ciphertext = bf.encrypt(plaintext);
        auto decrypted = bf.decrypt(ciphertext);
        if (decrypted != plaintext) fail("decrypt(encrypt(bytes)) should equal original bytes");

        std::string text = "This is a test string for Blowfish encrypt/decrypt";
        std::vector<uint8_t> textBytes(text.begin(), text.end());
        auto textCipher = bf.encrypt(textBytes);
        auto textDec = bf.decrypt(textCipher);
        if (textDec != textBytes) fail("decrypt(encrypt(textBytes)) should equal original text bytes");

        pass("encrypt/decrypt roundtrip");
    } catch (const std::exception &e) {
        fail(std::string("encrypt/decrypt roundtrip test threw unexpected exception: ") + e.what());
    }

    // Test 9: encryptText/decryptText roundtrip
    try {
        std::vector<std::string> testStrings = {
            "Hello API",
            "",
            "Short",
            "Longer text that spans multiple blocks and includes special characters ąęćłńóśżź!",
        };

        for (const auto &text : testStrings) {
            auto c = bf.encryptText(text);
            auto out = bf.decryptText(c);
            if (out != text) fail("decryptText(encryptText(text)) should equal original text for '" + text + "'");
        }

        pass("encryptText/decryptText roundtrip");
    } catch (const std::exception &e) {
        fail(std::string("encryptText/decryptText roundtrip test threw unexpected exception: ") + e.what());
    }



    // Test 10: integration test
    try {
        // case 1
        std::string text = "Integration test for Blowfish encryption and decryption.";
        Blowfish bf("integrationkey");
        auto c = bf.encryptText(text);
        auto out = bf.decryptText(c);
        if (out != text) fail("Integration test failed: decrypted text does not match original");
        pass("integration test case 1");
        
        // case 2
        text = "Another test with different text and key! 1234567890";
        Blowfish bf2("anotherkey12345");
        auto c2 = bf2.encryptText(text);
        auto out2 = bf2.decryptText(c2);
        if (out2 != text) fail("Integration test failed: decrypted text does not match original for case 2");
        pass("integration test case 2");

    } catch (const std::exception &e) {
        fail(std::string("integration test threw unexpected exception: ") + e.what());
    }

        std::cout << "ALL TESTS PASSED\n";
    return 0;
}
