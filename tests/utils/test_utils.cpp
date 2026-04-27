#define private public
#include "../../src/utils/utils.h"
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

void readFileToBytes_unittest() {
    // Create a temporary file with known content
    const std::string tempFilePath = "temp_test_file.txt";
    const std::string fileContent = "Hello, World!";
    std::ofstream tempFile(tempFilePath);
    if (!tempFile) {
        fail("Failed to create temporary file for testing");
    }
    tempFile << fileContent;
    tempFile.close();

    // Read the file using the utility function
    std::vector<uint8_t> outputBytes = readFileToBytes(tempFilePath);
    
    // Expected byte vector (ASCII values of "Hello, World!")
    std::vector<uint8_t> expectedBytes = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21};
    
    if (outputBytes.size() != expectedBytes.size()) {
        fail("readFileToBytes output size mismatch", std::to_string(outputBytes.size()), std::to_string(expectedBytes.size()));
    }

    for (size_t i = 0; i < expectedBytes.size(); ++i) {
        if (outputBytes[i] != expectedBytes[i]) {
            fail("readFileToBytes output mismatch at index " + std::to_string(i), std::to_string(outputBytes[i]), std::to_string(expectedBytes[i]));
        }
    }
    
    pass("readFileToBytes correctly reads file content into byte vector");

    // Clean up temporary file
    if (std::remove(tempFilePath.c_str()) != 0) {
        std::cerr << "Warning: Failed to delete temporary file " << tempFilePath << "\n";
    }
}

int main() {
    readFileToBytes_unittest();
    return 0;
}