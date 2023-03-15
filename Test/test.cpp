#include "Catch2/src/catch2/catch_test_macros.hpp""
#include <EugeneLib.h>
#pragma comment(lib,"EugeneLib.lib")

TEST_CASE("CreateWindow", "[function]")
{
	constexpr auto size = Eugene::Vector2{ 1280.0f, 720.0f };
	Eugene::System* window = Eugene::CreateSystem(size, u8"test");
	REQUIRE(window->GetWindowSize() == size);
	delete window;
}