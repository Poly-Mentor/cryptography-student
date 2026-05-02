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
    const std::filesystem::path tempFilePath = "temp_test_file.txt";
    const std::string fileContent = "Hello, World!";
    std::ofstream tempFile(tempFilePath);
    if (!tempFile) {
        fail("Failed to create temporary file for testing");
    }
    tempFile << fileContent;
    tempFile.close();

    File* file = new File(tempFilePath);

    // Read the file using the utility function
    std::vector<uint8_t> outputBytes = *file->getContentBytes();
    
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

void saveFileAs_unittest() {
    // Create a byte vector with known content
    std::vector<uint8_t> contentBytes = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    const std::filesystem::path targetFilePath = "temp_output_file.txt";

    File* file = new File();
    file->saveFileAs(contentBytes, targetFilePath);

    // Read the file back to verify its content
    std::ifstream inputFile(targetFilePath, std::ios::binary);
    if (!inputFile) {
        fail("Failed to open target file for verification");
    }

    std::vector<uint8_t> readBytes((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    
    if (readBytes.size() != contentBytes.size()) {
        fail("saveFileAs output size mismatch", std::to_string(readBytes.size()), std::to_string(contentBytes.size()));
    }

    for (size_t i = 0; i < contentBytes.size(); ++i) {
        if (readBytes[i] != contentBytes[i]) {
            fail("saveFileAs output mismatch at index " + std::to_string(i), std::to_string(readBytes[i]), std::to_string(contentBytes[i]));
        }
    }
    
    pass("saveFileAs correctly writes byte vector to file");
}

void read_modify_save_test(){

    // Create a temporary file with known content
    const std::filesystem::path tempInputFilePath = "temp_test_file.txt";
    const std::string fileContent = "Hello, World!";
    std::ofstream tempFile(tempInputFilePath);
    if (!tempFile) {
        fail("Failed to create temporary file for testing");
    }
    tempFile << fileContent;
    tempFile.close();
    File* file = new File(tempInputFilePath);
    std::vector<uint8_t> initialOutputBytes = *file->getContentBytes();

    // Modify the byte vector
    std::vector<uint8_t> modifiedBytes = initialOutputBytes;
    modifiedBytes.push_back(0x21); // Add an exclamation mark
    modifiedBytes.push_back(0x21); // Add an exclamation mark

    // Save the modified byte vector to a new file
    const std::filesystem::path tempOutputFilePath = "temp_modified_output_file.txt";
    file->saveFileAs(modifiedBytes, tempOutputFilePath);

    // Read the modified file back to verify its content
    std::ifstream inputFile(tempOutputFilePath, std::ios::binary);
    if (!inputFile) {
        fail("Failed to open modified output file for verification");
    }
    std::vector<uint8_t> readBytes((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    if (readBytes.size() != modifiedBytes.size()) {
        fail("Modified file output size mismatch", std::to_string(readBytes.size()), std::to_string(modifiedBytes.size()));
    }
    for (size_t i = 0; i < modifiedBytes.size(); ++i) {
        if (readBytes[i] != modifiedBytes[i]) {
            fail("Modified file output mismatch at index " + std::to_string(i), std::to_string(readBytes[i]), std::to_string(modifiedBytes[i]));
        }
    }
    pass("Read-modify-save test passed successfully");
}

int main() {
    readFileToBytes_unittest();
    saveFileAs_unittest();
    read_modify_save_test();
    return 0;
}