#pragma once
#include <wrl.h>
#include <d3d12.h>

struct ID3D12Device;

namespace Eugene
{
	class Graphics;
	class Dx12Views
	{
	public:
		virtual ~Dx12Views();
	protected:
		Dx12Views(ID3D12Device* device, size_t size, bool isShaderVisible, D3D12_DESCRIPTOR_HEAP_TYPE type);
		Microsoft::WRL::ComPtr< ID3D12DescriptorHeap> descriptorHeap_;
	private:
	};
}
