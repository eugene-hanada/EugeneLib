#include "Catch2/src/catch2/catch_test_macros.hpp""
#include <Eugene.h>
#pragma comment(lib,"EugeneLib.lib")

TEST_CASE("CreateWindow", "[function]")
{
	Eugene::System* window = CreateSystem({ 1280.0f, 720.0f }, u8"test");
	delete window;
}