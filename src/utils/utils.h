#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cstdint>

std::vector<uint8_t> readFileToBytes(const std::string &filePath);