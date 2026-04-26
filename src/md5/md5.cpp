#include "md5.h"

MD5::MD5()
{
    reset();
}

void MD5::reset()
{
    currentState = initialState;
}

std::vector<uint8_t> MD5::stringToBytes(const std::string &inputText)
{
    std::vector<uint8_t> output = std::vector<uint8_t>();
    output.reserve(inputText.length());
    for (char c : inputText)
    {
        output.push_back(static_cast<uint8_t>(c));
    }
    return output;
}
