#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <EugeneLib.h>
#include <vector>
#include <cstddef>
#include <cstdint>

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

	Eugene::FixedHashMap<int, int, std::uint32_t, SimpleHash> map(8);
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

TEST_CASE("SwissTable")
{
	struct SimpleHash { std::uint32_t operator()(int k) const noexcept { return static_cast<std::uint32_t>(k); } };

	Eugene::SwissTable<int, int, std::uint32_t, SimpleHash> swissTable;
	swissTable[1] = 10;
	swissTable[2] = 20;

	CHECK(swissTable[1] == 10);
	CHECK(swissTable[2] == 20);

	// Use Find to locate entries rather than iterating raw storage
	bool found1 = false, found2 = false;
	auto it1 = swissTable.Find(1);
	if (it1 != swissTable.end())
	{
		CHECK(it1.first() == 1);
		CHECK(it1.second() == 10);
		found1 = true;
	}
	auto it2 = swissTable.Find(2);
	if (it2 != swissTable.end())
	{
		CHECK(it2.first() == 2);
		CHECK(it2.second() == 20);
		found2 = true;
	}

	CHECK(found1);
	CHECK(found2);

	const auto& cswissTable = swissTable;
	CHECK(cswissTable.At(1) == 10);
}

TEST_CASE("FNV1aHash_Basic")
{
	const std::uint8_t bytes[] = { 1, 2, 3, 4, 5 };

	auto h64_a = Eugene::FNV1aHash64(bytes, std::size(bytes));
	auto h64_b = Eugene::FNV1aHash64(reinterpret_cast<const char*>(bytes), std::size(bytes));
	CHECK(h64_a == h64_b);

	auto h32_a = Eugene::FNV1aHash32(bytes, std::size(bytes));
	auto h32_b = Eugene::FNV1aHash32(reinterpret_cast<const char*>(bytes), std::size(bytes));
	CHECK(h32_a == h32_b);

	// Deterministic
	CHECK(h64_a == Eugene::FNV1aHash64(bytes, std::size(bytes)));
	CHECK(h32_a == Eugene::FNV1aHash32(bytes, std::size(bytes)));

	CHECK(h64_a != 0);
	CHECK(h32_a != 0);
}

TEST_CASE("FNV1aHash_String")
{
	std::string_view a = "hello";
	std::string_view b = "world";

	auto ha64 = Eugene::FNV1aHash64(a);
	auto hb64 = Eugene::FNV1aHash64(b);
	CHECK(ha64 != hb64);
	CHECK(ha64 == Eugene::FNV1aHash64(a.data(), a.size()));

	auto ha32 = Eugene::FNV1aHash32(a);
	auto hb32 = Eugene::FNV1aHash32(b);
	CHECK(ha32 != hb32);
	CHECK(ha32 == Eugene::FNV1aHash32(a.data(), a.size()));

	// u16/u32 overloads are available and deterministic
	std::u16string u16 = u"hello";
	auto hu16_64 = Eugene::FNV1aHash64(u16.data(), u16.size());
	CHECK(hu16_64 == Eugene::FNV1aHash64(u16.data(), u16.size()));
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