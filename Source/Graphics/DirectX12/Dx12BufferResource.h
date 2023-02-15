#pragma once
#include <wrl.h>
#include "../../../Include/Graphics/BufferResource.h"

struct ID3D12Resource;
struct ID3D12Device;

namespace Eugene
{
	class Image;

	class Dx12BufferResource :
		public BufferResource
	{
	public:
		Dx12BufferResource(ID3D12Device* device,std::uint64_t size);
		~Dx12BufferResource();
	private:
		bool CanMap(void) const final;
		void* GetResource(void) const final;
		std::uint64_t GetSize(void) final;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	};

	class Dx12UploadableBufferResource :
		public BufferResource
	{
	public:
		Dx12UploadableBufferResource(ID3D12Device* device, Image& image);
		Dx12UploadableBufferResource(ID3D12Device* device, std::uint64_t size);
		~Dx12UploadableBufferResource();
	private:
		void* Map(void) final;
		void UnMap(void) final;

		bool CanMap(void) const final;
		void* GetResource(void) const final;
		std::uint64_t GetSize(void) final;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	};

}
