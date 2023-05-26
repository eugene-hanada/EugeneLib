#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <Math/Vector2.h>
#include <Common/Debug.h>


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

TEST_CASE("App", "[System][Grahpics]")
{
	constexpr auto size = Eugene::Vector2{ 1280.0f, 720.0f };
	Eugene::System* window = Eugene::CreateSystem(size, u8"test");
	REQUIRE(window != nullptr);
	REQUIRE(window->GetWindowSize() == size);

	auto [graphics, gpu] = window->CreateGraphics();
	REQUIRE(graphics != nullptr);
	REQUIRE(gpu != nullptr);

	auto cmdList = graphics->CreateCommandList();
	REQUIRE(cmdList != nullptr);
	delete cmdList;

	auto uploadBuffer = graphics->CreateUploadableBufferResource(256);
	REQUIRE(uploadBuffer != nullptr);
	REQUIRE(uploadBuffer->GetSize() == 256);
	auto mapPtr = uploadBuffer->Map();
	REQUIRE(mapPtr != nullptr);
	uploadBuffer->UnMap();
	delete uploadBuffer;

	auto buffer = graphics->CreateBufferResource(256);
	REQUIRE(buffer != nullptr);
	REQUIRE(buffer->GetSize() == 256);
	REQUIRE(buffer->Map() == nullptr);
	delete buffer;


	delete gpu;
	delete graphics;

	delete window;
}
