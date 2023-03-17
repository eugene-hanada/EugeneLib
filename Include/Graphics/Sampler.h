#pragma once
#include <cstdint>

namespace Eugene
{
	/// <summary>
	/// �e�N�X�`���A�h���b�V���O���[�h
	/// </summary>
	enum class TextureAddressMode
	{
		Wrap = 1,
		Mirror = 2,
		Clamp = 3,
		Border = 4,
		MirrorOnce = 5
	};

	/// <summary>
	/// �T���v���[�̃t�B���^�[
	/// </summary>
	enum class SampleFilter
	{
		Point = 0,
		Linear = 0x15,
		Anisotropic = 0x55
	};

	/// <summary>
	/// �T���v���[�̔�r���Z�p
	/// </summary>
	enum class SamplerComparison
	{
		Non,
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always
	};

	/// <summary>
	/// �T���v���[�̃��C�A�E�g�p
	/// </summary>
	struct SamplerLayout
	{
		SamplerLayout(
			TextureAddressMode u = TextureAddressMode::Wrap,
			TextureAddressMode v = TextureAddressMode::Wrap,
			TextureAddressMode w = TextureAddressMode::Wrap,
			SampleFilter filter = SampleFilter::Point
		);
		TextureAddressMode u_;
		TextureAddressMode v_;
		TextureAddressMode w_;
		SampleFilter filter_;
		std::uint32_t maxAnisotropy_;
		SamplerComparison comparison_;
		float maxLod_;
		float minLod_;
	};

	/// <summary>
	/// �T���v���[
	/// </summary>
	class Sampler
	{
	public:
	protected:
		Sampler();
	private:
	};
}
