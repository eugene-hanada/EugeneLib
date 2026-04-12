#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <EugeneLib.h>

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

TEST_CASE("Array")
{
    Eugene::Array<int> a(3);
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;

    CHECK(a[0] == 1);
    CHECK(a[1] == 2);
    CHECK(a[2] == 3);

    a.Resize(5);
    CHECK(a.size() == 5);
    CHECK(a[0] == 1);
    CHECK(a[1] == 2);
    CHECK(a[2] == 3);
    CHECK(a[3] == 0);
    CHECK(a[4] == 0);

    a.Resize(2);
    CHECK(a.size() == 2);
    CHECK(a[0] == 1);
    CHECK(a[1] == 2);

    Eugene::Array<int> b{ a };
    CHECK(b.size() == 2);
    CHECK(b[0] == 1);
    CHECK(b[1] == 2);
}

TEST_CASE("FixedHashMap")
{
    struct SimpleHash { std::uint32_t operator()(int k) const noexcept { return static_cast<std::uint32_t>(k); } };

    Eugene::FixedHashMap<int, int, SimpleHash> map(8);
    map[1] = 10;
    map[2] = 20;

    CHECK(map[1] == 10);
    CHECK(map[2] == 20);

    bool found1 = false, found2 = false;
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        int v = *it;
        if (v == 10) found1 = true;
        if (v == 20) found2 = true;
    }
    CHECK(found1);
    CHECK(found2);

    const auto& cmap = map;
    CHECK(cmap[1] == 10);
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