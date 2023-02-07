#pragma once

namespace Eugene
{
	enum class TextureAddressMode
	{
		Wrap = 1,
		Mirror = 2,
		Clamp = 3,
		Border = 4,
		MirrorOnce = 5
	};

	enum class SampleFilter
	{
		Point = 0,
		Linear = 0x15,
		Anisotropic = 0x55
	};

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
		float maxAnisotropy_;
		SamplerComparison comparison_;
		float maxLod_;
		float minLod_;
	};

	class Sampler
	{
	public:
	protected:
		Sampler();
	private:
	};
}
