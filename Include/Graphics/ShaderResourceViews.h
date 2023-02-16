#pragma once
#include <cstdint>

namespace Eugene
{
	class Graphics;
	class BufferResource;
	class ImageResource;

	class Sampler;
	class ShaderResourceViews
	{
	public:
		virtual ~ShaderResourceViews();
		virtual void CreateTexture(ImageResource& resource, std::uint64_t idx) = 0;
		virtual void CreateConstantBuffer(BufferResource& resource, std::uint64_t idx) = 0;
		virtual void* GetViews(void) const = 0;
		const std::uint64_t GetSize(void);
	protected:
		ShaderResourceViews(std::uint64_t size);
		std::uint64_t size_;
	};
}
