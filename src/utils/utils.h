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
    File();

    /** File constructor with path argument
     * Content will be read if file exists, otherwise empty file will be created */
    File(std::filesystem::path path);

    /** File constructor with path and content (content will be written to path) */
    File(std::filesystem::path path, std::vector<uint8_t> contentBytes);

        /** File constructor with path and content (content will be written to path) */
    File(std::filesystem::path path, std::string content); // TODO

    /** Destructor performing cleanup */
    ~File();

    /** Returns a const reference to the file's content as a vector of bytes (lazy-loaded and cached) */
    const std::vector<uint8_t>& getContentBytes();

    /** Saves a copy of the file's content to a new path 
     * @return a pointer to the new File object */
    File* saveFileAs(const std::vector<uint8_t> &contentBytes, std::filesystem::path targetPath);

private:
    std::filesystem::path input_path;
    std::vector<uint8_t>* content_bytes;

    /** Reads a file to a vector of bytes 
     * @param path the path to the file to read
     * @return a pointer to a vector of bytes containing the file's content
    */
    static std ::vector<uint8_t>* readFileToBytes(const std::filesystem::path &path);

};