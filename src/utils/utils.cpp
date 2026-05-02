#include "utils.h"


File::File(std::filesystem::path path)
{
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("File does not exist: " + path.string());
    }

    if (!std::filesystem::is_regular_file(path)) {
        throw std::runtime_error("Path is not a regular file: " + path.string());
    }
    
    input_path = path;

}

std::vector<uint8_t> *File::getContentBytes()
{

    if (content_bytes == nullptr and !input_path.empty()) {
        content_bytes = File::readFileToBytes(input_path);
    }

    return new std::vector<uint8_t>(*content_bytes);
}

std::vector<uint8_t> *File::readFileToBytes(const std::filesystem::path &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file: " + path.string());
    }

    return new std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

File::File()
    : input_path(), content_bytes(nullptr)
{
}

File::~File()
{
    if (content_bytes != nullptr) {
        delete content_bytes;
        content_bytes = nullptr;
    }
}

File::File(std::filesystem::path path, std::vector<uint8_t> contentBytes)
    : input_path(path), content_bytes(new std::vector<uint8_t>(contentBytes))
{
    // write content to the path
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Could not open file for writing: " + path.string());
    }
    if (!contentBytes.empty()) {
        out.write(reinterpret_cast<const char*>(contentBytes.data()), static_cast<std::streamsize>(contentBytes.size()));
    }
}

File* File::saveFileAs(const std::vector<uint8_t> &contentBytes, std::filesystem::path targetPath)
{
    std::ofstream out(targetPath, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Could not open file for writing: " + targetPath.string());
    }
    if (!contentBytes.empty()) {
        out.write(reinterpret_cast<const char*>(contentBytes.data()), static_cast<std::streamsize>(contentBytes.size()));
    }

    return new File(targetPath, contentBytes);
}
