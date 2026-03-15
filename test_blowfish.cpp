#include <iostream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <cstring>

#define private public
#include "blowfish.h"
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
        uint8_t k0[1] = {0};
        try { Blowfish bf(k0, 0); fail("ctor accepted key length 0"); } catch (const std::invalid_argument&) {}
        try { Blowfish bf(k0, 1); fail("ctor accepted key length 1"); } catch (const std::invalid_argument&) {}
        try { Blowfish bf(k0, 3); fail("ctor accepted key length 3"); } catch (const std::invalid_argument&) {}
        uint8_t k4[4] = {0,1,2,3};
        try { Blowfish bf(k4, 4); } catch (...) { fail("ctor rejected valid key length 4"); }
        uint8_t k56[56];
        for (int i=0;i<56;i++) k56[i]=uint8_t(i);
        try { Blowfish bf2(k56, 56); } catch (...) { fail("ctor rejected valid key length 56"); }
        uint8_t k57[57];
        for (int i=0;i<57;i++) k57[i]=uint8_t(i);
        try { Blowfish bf3(k57, 57); fail("ctor accepted key length 57"); } catch (const std::invalid_argument&) {}
        pass("constructor validation");
    } catch (const std::exception &e) {
        fail(std::string("constructor test threw unexpected exception: ") + e.what());
    }

    // Create a Blowfish instance for subsequent tests
    uint8_t key[] = {'t','e','s','t','k','e','y','1'}; // 8 bytes
    Blowfish bf(key, sizeof(key));

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

    std::cout << "ALL TESTS PASSED\n";
    return 0;
}
