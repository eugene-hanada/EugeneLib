#pragma once

namespace EugeneLib
{
	class Graphics;
	class GpuResource;
	class ShaderResourceViews
	{
	public:
		virtual ~ShaderResourceViews();
		virtual void CreateTexture(GpuResource& resource, size_t idx) = 0;
		virtual void CreateConstantBuffer(GpuResource& resource, size_t idx) = 0;
		virtual void* GetViews(void) const = 0;
		const size_t GetSize(void);
	protected:
		ShaderResourceViews(size_t size);
		size_t size_;
	};

	ShaderResourceViews* CreateShaderResourceViews(Graphics& graphics, size_t size);
}
