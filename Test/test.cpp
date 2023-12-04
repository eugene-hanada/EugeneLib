#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <EugeneLib.h>

TEST_CASE("Game")
{
	std::unique_ptr<Eugene::System> sys;
	REQUIRE_NOTHROW(sys.reset(Eugene::CreateSystem({ 1280.0f, 720.0f }, u8"Test")));
	CHECK(static_cast<glm::ivec2>(sys->GetWindowSize()) == glm::ivec2{ 1280, 720 });

	std::unique_ptr<Eugene::Graphics> graphics;
	std::unique_ptr<Eugene::GpuEngine> gpuEngine;
	REQUIRE_NOTHROW(
		auto tmp = sys->CreateGraphicsUnique();
		graphics = std::move(tmp.first);
		gpuEngine = std::move(tmp.second);
	);

	std::unique_ptr<Eugene::BufferResource> buffer;
	REQUIRE_NOTHROW(buffer.reset(graphics->CreateBufferResource(256)));

	std::unique_ptr<Eugene::Sound> sound;
	REQUIRE_NOTHROW(sound.reset(Eugene::CreateSound()));
}

TEST_CASE("AlignmentedSize")
{
	std::size_t alig{ 8ull };
	
	CHECK(Eugene::AlignmentedSize(6ull, alig) == alig);
	CHECK(Eugene::AlignmentedSize(14 , alig) == alig * 2ull);

	alig = 10;
	CHECK(Eugene::AlignmentedSize(3ull, alig) == alig);
	CHECK(Eugene::AlignmentedSize(12ull, alig) == alig * 2ull);

	static_assert(Eugene::AlignmentedSize(13ull, 16ull) == 16ull);
	static_assert(Eugene::AlignmentedSize(17ull, 16ull) == 32ull);
	
	static_assert(Eugene::AlignmentedSize(11ull, 15ull) == 15ull);
	static_assert(Eugene::AlignmentedSize(16ull, 15ull) == 30ull);
}

TEST_CASE("Random")
{
	{
		Eugene::RandomXorShift64 random;
		{
			int max = 10;
			int min = 0;
			auto result = random(min, max);
			CHECK((result <= max && result >= min));
		}

		{
			float max = 5.0f;
			float min = -5.0f;
			auto result = random(min, max);
			CHECK((result <= max && result >= min));
		}
	}

	{
		Eugene::RandomXoshiro128pp random;
		{
			int max = 10;
			int min = 0;
			auto result = random(min, max);
			CHECK((result <= max && result >= min));
		}

		{
			float max = 5.0f;
			float min = -5.0f;
			auto result = random(min, max);
			CHECK((result <= max && result >= min));
		}
	}
}