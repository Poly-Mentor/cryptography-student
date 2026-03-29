#include "rng.h"

// Temporarily using std::rand() for simplicity, but in a production implementation, a more secure RNG should be used (e.g., std::random_device or a cryptographic library)

RNG::RNG()
{
    // Seed the random number generator with current time
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

uint64_t RNG::get_random_odd(int bit_len)
{
    if (bit_len <= 0) throw(std::invalid_argument("Bit length must be positive"));
    if (bit_len > 64) throw(std::invalid_argument("Generating random numbers with more than 64 bits is not supported"));
    uint64_t r = 0;
    r |= (uint64_t(1) << (bit_len - 1)); // set the highest bit to ensure the number has the desired bit length
    r |= 1ULL; // ensure the number is odd (set lowest bit)

    for (int i = 1; i < bit_len - 1; i++) {
        // Randomly set intermediate bits
        r |= (uint64_t(std::rand() % 2)) << (bit_len - 1 - i);
    }
    return r;
}