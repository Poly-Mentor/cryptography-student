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

    /** Destructor performing cleanup */
    ~File();

    /** Returns a copy of the file's content as a vector of bytes */
    std::vector<uint8_t>* getContentBytes();

    /** Saves a copy of the file's content to a new path 
     * @return a pointer to the new File object */
    File* saveFileAs(const std::vector<uint8_t> &contentBytes, std::filesystem::path targetPath);

private:
    std::filesystem::path input_path;
    std::vector<uint8_t>* content_bytes;

    std::filesystem::path getPathDialog();

    /** Reads a file to a vector of bytes 
     * @param path the path to the file to read
     * @return a pointer to a vector of bytes containing the file's content
    */
    static std ::vector<uint8_t>* readFileToBytes(const std::filesystem::path &path);

};