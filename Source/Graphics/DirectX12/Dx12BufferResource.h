#pragma once
#include <wrl.h>
#include "../../../Include/Graphics/BufferResource.h"

struct ID3D12Resource;
struct ID3D12Device;

namespace D3D12MA
{
	struct Allocation;
	struct Allocator;
}


namespace Eugene
{
	class Image;

	class Dx12BufferResource :
		public BufferResource
	{
	public:
		Dx12BufferResource(D3D12MA::Allocator* allocator,std::uint64_t size);
		~Dx12BufferResource();
	private:
		bool CanMap(void) const final;
		void* GetResource(void)  final;
		std::uint64_t GetSize(void) final;

		/// <summary>
		/// アロケーターで確保したメモリ
		/// </summary>
		Microsoft::WRL::ComPtr<D3D12MA::Allocation> allocation_;

		/// <summary>
		/// リソース
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

	};

	class Dx12UploadableBufferResource :
		public BufferResource
	{
	public:
		Dx12UploadableBufferResource(ID3D12Device* device, D3D12MA::Allocator* allocator, Image& image);
		Dx12UploadableBufferResource(D3D12MA::Allocator* allocator, std::uint64_t size);
		~Dx12UploadableBufferResource();
	private:
		void* Map(void) final;
		void UnMap(void) final;

		bool CanMap(void) const final;
		void* GetResource(void)  final;
		std::uint64_t GetSize(void) final;

		/// <summary>
		/// アロケーターで確保したメモリ
		/// </summary>
		Microsoft::WRL::ComPtr<D3D12MA::Allocation> allocation_;

		/// <summary>
		/// リソース
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	};

}
