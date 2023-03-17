#pragma once
#include "../../../Include/Graphics/ImageResource.h"
#include <span>
#include <wrl.h>
#include "../../../Include/Math/Vector2.h"

struct ID3D12Resource;
struct ID3D12Device;
struct IDXGISwapChain4;

namespace Eugene
{
	class Dx12ImageResource :
		public ImageResource
	{
	public:
		Dx12ImageResource(ID3D12Device* device,const TextureInfo& info);
		Dx12ImageResource(ID3D12Device* device, const Vector2I& size, Format format, std::span<float,4> clearColor);
		Dx12ImageResource(ID3D12Device* device, const Vector2I& size, Format format, float clearValue);
		Dx12ImageResource(IDXGISwapChain4* swapChain, std::uint32_t idx);
	private:
		Vector2I GetSize(void) final;
		bool CanMap(void) const final;
		void* GetResource(void) const final;

		/// <summary>
		/// リソース
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	};

	class Dx12UploadableImageResource :
		public ImageResource
	{
	public:
	private:

		/// <summary>
		/// リソース
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	};
}