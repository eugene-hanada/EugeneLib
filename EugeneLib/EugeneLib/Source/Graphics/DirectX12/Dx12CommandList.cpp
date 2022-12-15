#include "Dx12CommandList.h"
#include <d3d12.h>
#include "../../../Include/ThirdParty/d3dx12.h"
#include <array>
#include "../../../Include/Graphics/Graphics.h"
#include "../../../Include/Common/EugeneLibException.h"
#include "../../../Include/Graphics/RenderTargetViews.h"
#include "../../../Include/Graphics/GpuResource.h"
#include "../../../Include/Graphics/VertexView.h"
#include "../../../Include/Graphics/ShaderResourceViews.h"
#include "Dx12GraphicsPipeline.h"

EugeneLib::Dx12CommandList::Dx12CommandList(Graphics& graphics)
{
	auto device = static_cast<ID3D12Device*>(graphics.GetDevice());
	if (FAILED(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdAllocator_.ReleaseAndGetAddressOf()))))
	{
		throw EugeneLibException("コマンド作成できませんでした");
	}

	if (FAILED(device->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator_.Get(), nullptr, IID_PPV_ARGS(cmdList_.ReleaseAndGetAddressOf())
	)))
	{
		throw EugeneLibException("コマンド作成できませんでした");
	}

	End();
}

void* EugeneLib::Dx12CommandList::GetCommandList(void) const
{
	return cmdList_.Get();
}

void EugeneLib::Dx12CommandList::Begin(void)
{
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_.Get(), nullptr);
}

void EugeneLib::Dx12CommandList::End(void)
{
	cmdList_->Close();
}

void EugeneLib::Dx12CommandList::SetGraphicsPipeline(GraphicsPipeline& gpipeline)
{
	auto pipeline{ static_cast<Dx12GraphicsPipeline::PipeLine*>(gpipeline.GetPipeline()) };
	cmdList_->SetPipelineState(pipeline->state_.Get());
	cmdList_->SetGraphicsRootSignature(pipeline->rootSignature_.Get());
}

void EugeneLib::Dx12CommandList::SetPrimitiveType(PrimitiveType type)
{
	cmdList_->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(type));
}

void EugeneLib::Dx12CommandList::SetScissorrect(const Vector2I& leftTop, const Vector2I& rightBottom)
{
	CD3DX12_RECT rect{ leftTop.x,leftTop.y, rightBottom.x,rightBottom.y };
	cmdList_->RSSetScissorRects(1,&rect);
}

void EugeneLib::Dx12CommandList::SetViewPort(const Vector2& leftTop, const Vector2& size, float depthMin, float depthMax)
{
	CD3DX12_VIEWPORT viewport{ leftTop.x, leftTop.y, size.x,size.y,depthMin,depthMax };
	cmdList_->RSSetViewports(1, &viewport);
}

void EugeneLib::Dx12CommandList::SetVertexView(VertexView& view)
{
	auto ptr = static_cast<D3D12_VERTEX_BUFFER_VIEW*>(view.GetView());
	cmdList_->IASetVertexBuffers(0, 1, ptr);
}

void EugeneLib::Dx12CommandList::SetShaderResourceView(ShaderResourceViews& views, size_t viewsIdx, size_t paramIdx)
{
	auto descriptorHeap{ static_cast<ID3D12DescriptorHeap*>(views.GetViews()) };
	ID3D12Device* device{ nullptr };
	if (FAILED(descriptorHeap->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}
	auto handle{ descriptorHeap->GetGPUDescriptorHandleForHeapStart()};
	handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * viewsIdx;
	cmdList_->SetDescriptorHeaps(1, &descriptorHeap);
	cmdList_->SetGraphicsRootDescriptorTable(static_cast<std::uint32_t>(paramIdx), handle);
}

void EugeneLib::Dx12CommandList::Draw(std::uint32_t vertexCount, std::uint32_t instanceCount)
{
	cmdList_->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void EugeneLib::Dx12CommandList::SetRenderTarget(RenderTargetViews& views, size_t idx)
{
	auto descriptorHeap{ static_cast<ID3D12DescriptorHeap*>(views.GetViews()) };
	auto handle{ descriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	ID3D12Device* device{ nullptr };
	if (FAILED(descriptorHeap->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}
	handle.ptr += static_cast<ULONG_PTR>(idx) * static_cast<ULONG_PTR>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	cmdList_->OMSetRenderTargets(1, &handle, false, nullptr);
}

void EugeneLib::Dx12CommandList::SetRenderTarget(RenderTargetViews& views)
{
	auto descriptorHeap{ static_cast<ID3D12DescriptorHeap*>(views.GetViews()) };
	auto handle{ descriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	ID3D12Device* device{ nullptr };
	if (FAILED(descriptorHeap->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}

	// レンダーターゲットの最大数が8なのでその分確保
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 8> handles;
	size_t count{ 0ull };
	for (size_t i = 0; i < views.GetSize() || i < handles.size(); i++)
	{
		// 最大数もしくはビューにある最大数分格納
		handles[i] = handle;
		handle.ptr += static_cast<ULONG_PTR>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		count++;
	}

	cmdList_->OMSetRenderTargets(static_cast<std::uint32_t>(count), handles.data(), false, nullptr);
}

void EugeneLib::Dx12CommandList::SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, size_t rtViewsIdx, size_t dsViewsIdx)
{
}

void EugeneLib::Dx12CommandList::SetRenderTarget(RenderTargetViews& views, DepthStencilViews& depthViews, size_t rtViewsIdx)
{
}

void EugeneLib::Dx12CommandList::ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color, size_t idx)
{
	auto descriptorHeap{ static_cast<ID3D12DescriptorHeap*>(views.GetViews()) };
	auto handle{ descriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	ID3D12Device* device{ nullptr };
	if (FAILED(descriptorHeap->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}
	handle.ptr += static_cast<ULONG_PTR>(idx) * static_cast<ULONG_PTR>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	cmdList_->ClearRenderTargetView(handle, color.data(), 0, nullptr);
}

void EugeneLib::Dx12CommandList::ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color)
{
}

void EugeneLib::Dx12CommandList::TransitionRenderTargetBegin(GpuResource& resource)
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12Resource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmdList_->ResourceBarrier(1, &barrier);
}

void EugeneLib::Dx12CommandList::TransitionRenderTargetEnd(GpuResource& resource)
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12Resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	cmdList_->ResourceBarrier(1, &barrier);
}

void EugeneLib::Dx12CommandList::Copy(GpuResource& destination, GpuResource& source)
{
	ID3D12Resource* src{ static_cast<ID3D12Resource*>(source.GetResource()) };
	ID3D12Resource* dst{ static_cast<ID3D12Resource*>(destination.GetResource()) };
	cmdList_->CopyResource(dst, src);
}

void EugeneLib::Dx12CommandList::CopyTexture(GpuResource& destination, GpuResource& source)
{
	auto dx12source{ static_cast<ID3D12Resource*>(source.GetResource()) };
	auto dx12destination{ static_cast<ID3D12Resource*>(destination.GetResource()) };
	ID3D12Device* device{ nullptr };
	if (FAILED(dx12source->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}


	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12destination, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList_->ResourceBarrier(1, &barrier);


	D3D12_TEXTURE_COPY_LOCATION dest{};
	dest.pResource = dx12destination;
	dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dest.SubresourceIndex = 0;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT  footprint;
	UINT64  totalSize = 0;
	D3D12_RESOURCE_DESC desc{ dx12destination->GetDesc() };
	D3D12_TEXTURE_COPY_LOCATION  src{};
	device->GetCopyableFootprints(&desc, 0, 1, 0, &footprint, nullptr, nullptr, &totalSize);
	src.pResource = dx12source;
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint = footprint;

	cmdList_->CopyTextureRegion(&dest, 0, 0, 0, &src, nullptr);

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12destination, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList_->ResourceBarrier(1, &barrier);
}
