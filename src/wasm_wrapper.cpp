#include <emscripten/bind.h>
#include <string>
#include <vector>

#include "blowfish/blowfish.h"
#include "md5/md5.h"
#include "rsa-small/rsa-small.h"
#include "utils/utils.h"

using namespace emscripten;

// Helper function to convert hex string to bytes
std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

// Helper function to convert bytes to hex string
std::string bytesToHexString(const std::vector<uint8_t>& bytes) {
    std::string hex;
    for (uint8_t byte : bytes) {
        char buf[3];
        sprintf(buf, "%02x", byte);
        hex += buf;
    }
    return hex;
}

// MD5 wrapper
std::string md5Hash(const std::string& input) {
    MD5 md5;
    return md5.calculate(input);
}

// Blowfish wrapper
class BlowfishWrapper {
private:
    Blowfish bf;
public:
    BlowfishWrapper(const std::string& key) : bf(key) {}
    
    std::string encrypt(const std::string& plaintext) {
        std::vector<uint8_t> encrypted = bf.encryptText(plaintext);
        return bytesToHexString(encrypted);
    }
    
    std::string decrypt(const std::string& ciphertext) {
        std::vector<uint8_t> cipherBytes = hexStringToBytes(ciphertext);
        std::vector<uint8_t> decrypted = bf.decrypt(cipherBytes);
        return std::string(decrypted.begin(), decrypted.end());
    }
};

// RSA key wrapper for JavaScript compatibility
struct RSAKey {
    uint64_t exponent;
    uint64_t modulus;
    
    std::string toString() const {
        return std::to_string(exponent) + "," + std::to_string(modulus);
    }
    
    static RSAKey fromString(const std::string& str) {
        size_t comma = str.find(',');
        RSAKey k;
        k.exponent = std::stoull(str.substr(0, comma));
        k.modulus = std::stoull(str.substr(comma + 1));
        return k;
    }
};

// RSA wrapper
class RSAWrapper {
public:
    static std::string generateKeys(uint8_t bitLength) {
        auto keyPair = RSA::generate_keys(bitLength);
        // Return as JSON-like format: {pub: "e,n", priv: "d,n"}
        std::string pubKey = std::to_string(keyPair.publicKey.exponent) + "," + std::to_string(keyPair.publicKey.modulus);
        std::string privKey = std::to_string(keyPair.privateKey.exponent) + "," + std::to_string(keyPair.privateKey.modulus);
        return pubKey + "|" + privKey;  // Separator for splitting
    }
    
    static std::string encrypt(const std::string& plaintext, const std::string& publicKeyStr) {
        RSAKey pubKey = RSAKey::fromString(publicKeyStr);
        key k{pubKey.exponent, pubKey.modulus};
        std::vector<uint64_t> ciphertext = RSA::encryptText(plaintext, k);
        
        // Convert vector of uint64_t to comma-separated string
        std::string result;
        for (size_t i = 0; i < ciphertext.size(); ++i) {
            if (i > 0) result += ",";
            result += std::to_string(ciphertext[i]);
        }
        return result;
    }
    
    static std::string decrypt(const std::string& ciphertextStr, const std::string& privateKeyStr) {
        RSAKey privKey = RSAKey::fromString(privateKeyStr);
        key k{privKey.exponent, privKey.modulus};
        
        // Parse comma-separated ciphertext
        std::vector<uint64_t> ciphertext;
        size_t pos = 0;
        while (pos < ciphertextStr.length()) {
            size_t comma = ciphertextStr.find(',', pos);
            if (comma == std::string::npos) {
                ciphertext.push_back(std::stoull(ciphertextStr.substr(pos)));
                break;
            }
            ciphertext.push_back(std::stoull(ciphertextStr.substr(pos, comma - pos)));
            pos = comma + 1;
        }
        
        return RSA::decryptText(ciphertext, k);
    }
};

// Bindings
EMSCRIPTEN_BINDINGS(crypto_module) {
    function("md5Hash", &md5Hash);
    
    class_<BlowfishWrapper>("Blowfish")
        .constructor<std::string>()
        .function("encrypt", &BlowfishWrapper::encrypt)
        .function("decrypt", &BlowfishWrapper::decrypt);
    
    class_<RSAWrapper>("RSA")
        .class_function("generateKeys", &RSAWrapper::generateKeys)
        .class_function("encrypt", &RSAWrapper::encrypt)
        .class_function("decrypt", &RSAWrapper::decrypt);
}
