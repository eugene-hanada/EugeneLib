#pragma once
#include <cstdint>

namespace Eugene
{
	class Graphics;
	class GpuResource;
	class ImageResource;

	class Sampler;
	class ShaderResourceViews
	{
	public:
		virtual ~ShaderResourceViews();
		virtual void CreateTexture(GpuResource& resource, size_t idx) = 0;
		virtual void CreateTexture(ImageResource& resource, std::uint64_t idx) = 0;
		virtual void CreateConstantBuffer(GpuResource& resource, size_t idx) = 0;
		virtual void* GetViews(void) const = 0;
		const size_t GetSize(void);
	protected:
		ShaderResourceViews(size_t size);
		size_t size_;
	};
}
