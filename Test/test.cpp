#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <EugeneLib.h>

TEST_CASE("Game")
{
	REQUIRE_NOTHROW(Eugene::System::Create({ 1280.0f, 720.0f }, u8"Test"));

	// �E�B���h�E�T�C�Y�̎擾���`�F�b�N
	CHECK(static_cast<glm::ivec2>(Eugene::System::GetInstance().GetWindowSize()) == glm::ivec2{1280, 720});

	CHECK(Eugene::System::GetInstance().GetWindow());

	constexpr auto bufferNum = 2u;
	Eugene::GpuEngine gpuEngine;
	REQUIRE_NOTHROW(
		Eugene::Graphics::Create(bufferNum)
	);

	// �o�b�t�@�̐����`�F�b�N
	CHECK(Eugene::Graphics::GetInstance().GetViews().GetSize() == bufferNum);

	{
		Eugene::BufferResource buffer;

		// �o�b�t�@���\�[�X����
		REQUIRE_NOTHROW(buffer = Eugene::Graphics::GetInstance().CreateBufferResource(256));

		// API���̃��\�[�X�擾���ă`�F�b�N
		CHECK(buffer.GetResource());

		// �T�C�Y�`�F�b�N
		CHECK(buffer.GetSize() == 256);

		// �}�b�v�\���̃`�F�b�N
		CHECK(!buffer.CanMap());

		buffer.Final();

		buffer = Eugene::Graphics::GetInstance().CreateUnloadableBufferResource(256);

		// API���̃��\�[�X�擾���ă`�F�b�N
		CHECK(buffer.GetResource());

		// �T�C�Y�`�F�b�N
		CHECK(buffer.GetSize() == 256);

		// �}�b�v�\���̃`�F�b�N
		CHECK(buffer.CanMap());

		buffer.Final();
	}

	{
		Eugene::ImageResource imageResource;
		// Image���\�[�X����
		REQUIRE_NOTHROW(imageResource = Eugene::Graphics::GetInstance().CreateImageResource({640,480}, Eugene::Format::AUTO_BACKBUFFER));

		// API���̃��\�[�X�擾���ă`�F�b�N
		CHECK(imageResource.GetResource());

		// �T�C�Y�`�F�b�N
		CHECK(imageResource.GetSize() == glm::ivec2{640, 480});

		// �}�b�v�\���̃`�F�b�N
		CHECK(!imageResource.CanMap());

		//�t�H�[�}�b�g�`�F�b�N
		CHECK(imageResource.GetFormat() == Eugene::Graphics::BackBufferFormat());

		imageResource.Final();
	}

	gpuEngine.Final();
	REQUIRE_NOTHROW(Eugene::Graphics::Destroy());

#ifdef EUGENE_SOUND
	REQUIRE_NOTHROW(Eugene::Sound::Create());
	
	REQUIRE_NOTHROW(Eugene::Sound::Destroy());
#endif

	REQUIRE_NOTHROW(Eugene::System::Destroy());
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