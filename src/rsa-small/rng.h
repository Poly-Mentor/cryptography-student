#pragma once
#include <cstdint>
#include <stdexcept>
#include <cstdlib>
#include <ctime>

class RNG {

    public:
        
        RNG();
        uint64_t get_random_odd(int bit_len);

};
