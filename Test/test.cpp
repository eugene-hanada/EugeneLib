#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <cmath>
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
		REQUIRE((pos + Eugene::Vector2{1.0f, 2.0f}) == Eugene::Vector2{6.0f, 7.0f});
		pos += Eugene::Vector2{1.0f,2.0f};
		REQUIRE(pos == Eugene::Vector2{6.0f,7.0f});
	}

	SECTION("SUB")
	{
		REQUIRE((pos - Eugene::Vector2{1.0f, 2.0f}) == Eugene::Vector2{4.0f, 3.0f});
		pos -= Eugene::Vector2{1.0f,2.0f};
		REQUIRE(pos == Eugene::Vector2{4.0f, 3.0f});
	}

	SECTION("MUL")
	{
		REQUIRE((pos * Eugene::Vector2{1.0f, 2.0f}) == Eugene::Vector2{5.0f, 10.0f});
		pos *= Eugene::Vector2{1.0f,2.0f};
		REQUIRE(pos == Eugene::Vector2{5.0f, 10.0f});
	}

	SECTION("DIV")
	{
		REQUIRE((pos / Eugene::Vector2{1.0f, 2.0f}) == Eugene::Vector2{5.0f, 2.5f});
		pos /= Eugene::Vector2{1.0f,2.0f};
		REQUIRE(pos == Eugene::Vector2{5.0f, 2.5f});
	}

	SECTION("MOD")
	{
		REQUIRE((pos % Eugene::Vector2{1.0f, 2.0f}) == Eugene::Vector2{0.0f, 1.0f});
		pos %= Eugene::Vector2{1.0f, 2.0f};
		REQUIRE(pos == Eugene::Vector2{0.0f, 1.0f});
	}

	
	SECTION("MINUS")
	{
		auto result = std::signbit((-pos).x) && std::signbit((-pos).y);
		REQUIRE(result);
	}

	SECTION("PLUS")
	{
		auto result = (!std::signbit((+pos).x)) && (!std::signbit((+pos).y));
		REQUIRE(result);
	}
}

TEST_CASE("System,Graphics,Sound","[System][Graphics][Sound]")
{
	// System����
	constexpr auto size = Eugene::Vector2{ 1280.0f, 720.0f };
	Eugene::UniqueSystem sys;
	REQUIRE_NOTHROW((sys = Eugene::CreateSystemUnique( size, u8"TestCode")));

	// �T�C�Y�擾���e�X�g
	CHECK(sys->GetWindowSize() == size);

	// �Đ�������throw���������邩�e�X�g
	Eugene::UniqueSystem tmp;
	REQUIRE_THROWS((tmp = Eugene::CreateSystemUnique(size, u8"TestCode")));

	// Sound����
	Eugene::UniqueSound sound;
	REQUIRE_NOTHROW((sound = Eugene::CreateSoundUnique()));

	// Sound���Đ�������throw���������邩�e�X�g
	Eugene::UniqueSound tmpSound;
	REQUIRE_THROWS((tmpSound = Eugene::CreateSoundUnique()));

	// SoundControl�����e�X�g
	std::unique_ptr<Eugene::SoundControl> soundCtrl;
	CHECK_NOTHROW(soundCtrl.reset(sound->CreateSoundControl()));

	// Graphics�e�X�g
	std::pair<Eugene::UniqueGraphics, Eugene::UniqueGpuEngine> pair;
	REQUIRE_NOTHROW((pair = sys->CreateGraphicsUnique()));

	// Graphics���Đ�������throw���������邩�e�X�g
	std::pair<Eugene::UniqueGraphics, Eugene::UniqueGpuEngine> tmpPair;
	REQUIRE_THROWS((tmpPair = sys->CreateGraphicsUnique()));

	// BufferResource�����e�X�g
	std::unique_ptr<Eugene::BufferResource> buffer;
	CHECK_NOTHROW(buffer.reset(pair.first->CreateBufferResource(256ull)));
	if (buffer)
	{
		CHECK_FALSE(buffer->CanMap());
		CHECK(buffer->GetSize() == 256ull);

		// ���\�[�X�ɑ΂��ăV�F�[�_�[���\�[�X�Ƃ��Ẵr���[�𐶐����e�X�g
		std::unique_ptr<Eugene::ShaderResourceViews> srv;
		CHECK_NOTHROW(srv.reset(pair.first->CreateShaderResourceViews(1)));
		if (srv)
		{
			CHECK(srv->GetSize() == 1ull);
			CHECK_NOTHROW(srv->CreateConstantBuffer(*buffer,0ull));
		}

		// ���\�[�X�ɑ΂��Ē��_�r���[�𐶐����e�X�g
		std::unique_ptr<Eugene::VertexView> vertexView;
		CHECK_NOTHROW(vertexView.reset(pair.first->CreateVertexView(4ull,4ull,*buffer)));
		if (vertexView)
		{
			CHECK(vertexView->GetVertexNum() == 4ull);
		}

		// ���\�[�X�ɑ΂��ăC���f�b�N�X�r���[�𐶐����e�X�g
		std::unique_ptr<Eugene::IndexView> indexView;
		CHECK_NOTHROW(indexView.reset(pair.first->CreateIndexView(2ull, 6u, Eugene::Format::R16_UINT, *buffer)));
		if (indexView)
		{
			CHECK(indexView->GetIndexNum() == 6ull);
		}
	}

	// Map�\��BufferResource�����e�X�g
	std::unique_ptr<Eugene::BufferResource> upBuffer;
	CHECK_NOTHROW(upBuffer.reset(pair.first->CreateUploadableBufferResource(256ull)));
	if (upBuffer)
	{
		CHECK(upBuffer->CanMap());
		CHECK(upBuffer->GetSize() == 256ull);

		std::unique_ptr<Eugene::ShaderResourceViews> srv;
		CHECK_NOTHROW(srv.reset(pair.first->CreateShaderResourceViews(1)));
		if (srv)
		{
			CHECK(srv->GetSize() == 1ull);
			CHECK_NOTHROW(srv->CreateConstantBuffer(*upBuffer, 0ull));
		}

		std::unique_ptr<Eugene::VertexView> vertexView;
		CHECK_NOTHROW(vertexView.reset(pair.first->CreateVertexView(4ull, 4ull, *upBuffer)));
		if (vertexView)
		{
			CHECK(vertexView->GetVertexNum() == 4ull);
		}

		std::unique_ptr<Eugene::IndexView> indexView;
		CHECK_NOTHROW(indexView.reset(pair.first->CreateIndexView(2ull, 6u, Eugene::Format::R16_UINT, *upBuffer)));
		if (indexView)
		{
			CHECK(indexView->GetIndexNum() == 6ull);
		}
	}

	// �摜�⃌���_�[�^�[�Q�b�g�p���\�[�X�̃e�X�g
	std::unique_ptr<Eugene::ImageResource> image;
	float color[4]{ 1.0f,1.0f,1.0f, 1.0f };
	CHECK_NOTHROW(image.reset(pair.first->CreateImageResource(Eugene::Vector2I{1280, 720}, Eugene::Format::R8G8B8A8_UNORM, color)));
	if (image)
	{
		CHECK_FALSE(image->CanMap());
		CHECK(image->GetSize() == Eugene::Vector2I{1280,720});

		std::unique_ptr<Eugene::ShaderResourceViews> srv;
		CHECK_NOTHROW(srv.reset(pair.first->CreateShaderResourceViews(1)));
		if (srv)
		{
			CHECK(srv->GetSize() == 1ull);
			CHECK_NOTHROW(srv->CreateTexture(*image, 0ull));
		}

		std::unique_ptr<Eugene::RenderTargetViews> rtv;
		CHECK_NOTHROW(rtv.reset(pair.first->CreateRenderTargetViews(1, false)));
		if (rtv)
		{
			CHECK(rtv->GetSize() == 1ull);
			CHECK_NOTHROW(rtv->Create(*image, 0ull, Eugene::Format::R8G8B8A8_UNORM));
		}
	}
}
