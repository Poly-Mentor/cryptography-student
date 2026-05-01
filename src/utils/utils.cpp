#include "utils.h"

std::vector<uint8_t> readFileToBytes(const std::string &filePath)
{
        std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    // Move the file pointer to the end
    file.seekg(0, std::ios::end);
    // Get the current position, which is the size of the file
    std::streampos sp = file.tellg();
    // Check if tellg() succeeded
    if (sp == std::streampos(-1)) {
        throw std::runtime_error("Failed to determine file size: " + filePath);
    }

    // Convert the position to a size_t for the vector allocation
    std::size_t n = static_cast<std::size_t>(sp);
    // Move the file pointer back to the beginning of the file
    file.seekg(0, std::ios::beg);

    // Initialize a vector with predetermined size
    std::vector<uint8_t> bytes(n);
    if (n > 0) {
        // Read the file content into the vector in one go
        file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(n));
        // Check how many bytes were actually read
        std::streamsize actuallyRead = file.gcount();
        if (actuallyRead < static_cast<std::streamsize>(n)) {
            bytes.resize(static_cast<std::size_t>(actuallyRead));
        }
    }

    return bytes;
}
