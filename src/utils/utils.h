#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>
#include <filesystem>


class File
{
public:
    /** Interactive File constructor asking user for a path from which to read content */
    File();
    /** File constructor with path argument (content will be read) */
    File(std::filesystem::path path);
    /** File constructor with path and content (content will be written to path) */
    File(std::filesystem::path path, std::vector<uint8_t> contentBytes);
    ~File();
    std::vector<uint8_t>* getContentBytes();

private:
    std::filesystem::path input_path;
    std::vector<uint8_t>* content_bytes;
    std::filesystem::path getPathDialog();
    std::vector<uint8_t>* readFileToBytes(const std::filesystem::path &path);
    File* saveFileAs(const std::vector<uint8_t> &contentBytes, std::filesystem::path targetPath);

};