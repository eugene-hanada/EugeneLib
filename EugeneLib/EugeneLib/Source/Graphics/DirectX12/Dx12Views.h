#pragma once
#include <wrl.h>
#include <d3d12.h>

namespace EugeneLib
{
	class Graphics;
	class Dx12Views
	{
	public:
		virtual ~Dx12Views();
	protected:
		Dx12Views(Graphics& graphics,size_t size, bool isShaderVisible, D3D12_DESCRIPTOR_HEAP_TYPE type);
		Microsoft::WRL::ComPtr< ID3D12DescriptorHeap> descriptorHeap_;
	private:
	};
}
