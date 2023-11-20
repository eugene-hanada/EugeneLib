#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <EugeneLib.h>

TEST_CASE("Game")
{
	std::unique_ptr<Eugene::System> sys;
	REQUIRE_NOTHROW(sys.reset(Eugene::CreateSystem({ 1280.0f, 720.0f }, u8"Test")));
	CHECK(sys->GetWindowSize() == glm::vec2{ 1280.0f, 720.0f });

	std::unique_ptr<Eugene::Graphics> graphics;
	std::unique_ptr<Eugene::GpuEngine> gpuEngine;
	REQUIRE_NOTHROW(
		auto tmp = sys->CreateGraphicsUnique();
		graphics = std::move(tmp.first);
		gpuEngine = std::move(tmp.second);
	);

	std::unique_ptr<Eugene::BufferResource> buffer;
	REQUIRE_NOTHROW(buffer.reset(graphics->CreateBufferResource(256)));
}
