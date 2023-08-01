#include <catch2/catch_test_macros.hpp>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <Math/Vector2.h>
#include <Common/Debug.h>

#pragma comment(lib,"EugeneLib.lib")

TEST_CASE("Vector2","[Math]")
{
	Eugene::Vector2 pos{5.0f,5.0f};

	SECTION("EUQL")
	{
		REQUIRE(pos.x == 5.0f);
		REQUIRE(pos.y == 5.0f);
		REQUIRE(pos == Eugene::Vector2{5.0f,5.0f});
	}

	SECTION("ADD")
	{
		pos += Eugene::Vector2{1.0f,2.0f};
		REQUIRE(pos == Eugene::Vector2{6.0f,7.0f});
	}

	SECTION("SUB")
	{
		pos -= Eugene::Vector2{1.0f,2.0f};
		REQUIRE(pos == Eugene::Vector2{4.0f, 3.0f});
	}

	SECTION("MUL")
	{
		pos *= Eugene::Vector2{1.0f,2.0f};
		REQUIRE(pos == Eugene::Vector2{5.0f, 10.0f});
	}

	SECTION("DIV")
	{
		pos /= Eugene::Vector2{1.0f,2.0f};
		REQUIRE(pos == Eugene::Vector2{5.0f, 2.5f});
	}

}
