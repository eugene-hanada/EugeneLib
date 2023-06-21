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

Eugene::Xoshiro128ppEngine::result_type Eugene::Xoshiro128ppEngine::operator()()
{
	const std::uint64_t s0 = state_[0];
	std::uint64_t s1 = state_[1];
	const uint64_t result = s0 + s1;
	s1 ^= s0;
	state_[0] = ((s0 << 55) | (s0 >> 9)) ^ s1 ^ (s1 << 14);
	state_[1] = (s1 << 36) | (s1 >> 28);
	return result;
}

void Eugene::Xoshiro128ppEngine::seed(std::uint64_t seed)
{
	state_[0] = seed;
	state_[1] = (seed >> 32) | (seed << 32);
}