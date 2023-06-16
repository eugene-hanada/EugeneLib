#include "../../Include/Math/Random.h"

Eugene::XorShift64Engine::result_type Eugene::XorShift64Engine::operator()()
{
    std::uint64_t x = state_;
    x ^= x << 7ull;
    x ^= x >> 9ull;
    state_ = x;
    return x;
}

void Eugene::XorShift64Engine::seed(std::uint64_t seed)
{
	state_ = seed;
}

