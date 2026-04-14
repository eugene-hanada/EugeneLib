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

TEST_CASE("Vector")
{
	Eugene::Vector<int> v;
	CHECK(v.size() == 0);

	v.PushBack(1);
	v.PushBack(2);
	v.PushBack(3);
	CHECK(v.size() == 3);
	CHECK(v[0] == 1);
	CHECK(v[1] == 2);
	CHECK(v[2] == 3);

	v.EmplaceBack(4);
	CHECK(v.size() == 4);
	CHECK(v[3] == 4);

	v.Insert(1, 10);
	CHECK(v.size() == 5);
	CHECK(v[0] == 1);
	CHECK(v[1] == 10);
	CHECK(v[2] == 2);

	v.Emplace(2, 20);
	CHECK(v.size() == 6);
	CHECK(v[2] == 20);

	v.Erase(1);
	CHECK(v.size() == 5);
	CHECK(v[1] == 20);
	CHECK(v[4] == 4);

	v.PopBack();
	CHECK(v.size() == 4);
	CHECK(v[v.size() - 1] == 3);

	v.Resize(6);
	CHECK(v.size() == 6);
	CHECK(v[4] == 0);
	CHECK(v[5] == 0);

	v.Resize(2);
	CHECK(v.size() == 2);

	v.Reserve(10);
	CHECK(v.size() == 2);

	int sum = 0;
	for (auto it = v.begin(); it != v.end(); ++it)
	{
		sum += *it;
	}
	CHECK(sum == v[0] + v[1]);

	Eugene::Vector<int> v2{ 1, 2, 3 };
	CHECK(v2.size() == 3);
    CHECK(v2[0] == 1);
    CHECK(v2[1] == 2);
    CHECK(v2[2] == 3);

	Eugene::Vector<int> v3{ v2 };
	CHECK(v3.size() == 3);
	CHECK(v3[1] == 2);

	Eugene::Vector<int> v4{ std::move(v3) };
	CHECK(v4.size() == 3);
	CHECK(v4[2] == 3);
	CHECK(v3.size() == 0);
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
            CHECK(result <= max);
            CHECK(result >= min);
		}

		{
			float max = 5.0f;
			float min = -5.0f;
            auto result = random(min, max);
            CHECK(result <= max);
            CHECK(result >= min);
		}
	}

	{
		Eugene::RandomXoshiro128pp random;
		{
			int max = 10;
			int min = 0;
            auto result = random(min, max);
            CHECK(result <= max);
            CHECK(result >= min);
		}

		{
			float max = 5.0f;
			float min = -5.0f;
            auto result = random(min, max);
            CHECK(result <= max);
            CHECK(result >= min);
		}
	}
}