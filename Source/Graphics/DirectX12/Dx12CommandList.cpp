#include "../../../Include/Graphics/DirectX12/Dx12CommandList.h"
#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"
#include "../../../Include/ThirdParty/d3dx12.h"
#include <array>
#include <cassert>
#include "../../../Include/Graphics/Graphics.h"
#include "../../../Include/Utils/EugeneLibException.h"
#include "../../../Include/Graphics/RenderTargetViews.h"

#include "../../../Include/Graphics/GpuResource.h"
#include "../../../Include/Graphics/BufferResource.h"
#include "../../../Include/Graphics/ImageResource.h"

#include "../../../Include/Graphics/VertexView.h"
#include "../../../Include/Graphics/IndexView.h"
#include "../../../Include/Graphics/DirectX12/Dx12ShaderResourceViews.h"
#include "../../../Include/Graphics/DepthStencilViews.h"
#include "../../../Include/Graphics/DirectX12/Dx12GraphicsPipeline.h"
#include "../../../Include/Graphics/DirectX12/Dx12SamplerViews.h"

#ifdef USE_IMGUI
#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"
#include <backends/imgui_impl_dx12.h>
#endif

Eugene::CommandList::CommandList()
{
	if (FAILED(Graphics::GetInstance().device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdAllocator_.ReleaseAndGetAddressOf()))))
	{
		throw CreateErrorException("コマンド(ID3D12CommandAllocator)作成できませんでした");
	}

	if (FAILED(Graphics::GetInstance().device_->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator_.Get(), nullptr, IID_PPV_ARGS(cmdList_.ReleaseAndGetAddressOf())
	)))
	{
		throw CreateErrorException("コマンド(ID3D12CommandList)作成できませんでした");
	}

	End();
}

void* Eugene::CommandList::GetCommandList(void)
{
	return cmdList_.Get();
}

void Eugene::CommandList::Begin(void)
{
	cmdAllocator_->Reset();
	cmdList_->Reset(cmdAllocator_.Get(), nullptr);
}

void Eugene::CommandList::End(void)
{
	cmdList_->Close();
}

void Eugene::CommandList::SetGraphicsPipeline(Pipeline& gpipeline)
{
	auto pipeline{ static_cast<Pipeline::PipeLineSet*>(gpipeline.GetPipeline()) };
	cmdList_->SetPipelineState(pipeline->state_.Get());
	cmdList_->SetGraphicsRootSignature(pipeline->rootSignature_.Get());
}

void Eugene::CommandList::SetComputePipeline(Pipeline& cpipeline)
{
	auto pipeline{ static_cast<Pipeline::PipeLineSet*>(cpipeline.GetPipeline()) };
	cmdList_->SetPipelineState(pipeline->state_.Get());
	cmdList_->SetComputeRootSignature(pipeline->rootSignature_.Get());
}

void Eugene::CommandList::SetPrimitiveType(PrimitiveType type)
{
	cmdList_->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(type));
}

void Eugene::CommandList::SetScissorrect(const glm::ivec2& leftTop, const glm::ivec2& rightBottom)
{
	CD3DX12_RECT rect{ leftTop.x,leftTop.y, rightBottom.x,rightBottom.y };
	cmdList_->RSSetScissorRects(1,&rect);
}

void Eugene::CommandList::SetViewPort(const glm::vec2& leftTop, const glm::vec2& size, float depthMin, float depthMax)
{
	CD3DX12_VIEWPORT viewport{ leftTop.x, leftTop.y, size.x,size.y,depthMin,depthMax };
	cmdList_->RSSetViewports(1, &viewport);
}

void Eugene::CommandList::SetVertexView(VertexView& view)
{
	auto ptr = static_cast<D3D12_VERTEX_BUFFER_VIEW*>(view.GetView());
	cmdList_->IASetVertexBuffers(0, 1, ptr);
}

void Eugene::CommandList::SetIndexView(IndexView& view)
{
	auto ptr = static_cast<D3D12_INDEX_BUFFER_VIEW*>(view.GetView());
	cmdList_->IASetIndexBuffer(ptr);
}

void Eugene::CommandList::SetShaderResourceView(ShaderResourceViews& views, std::uint64_t paramIdx)
{
	auto descriptorHeap{ static_cast<ID3D12DescriptorHeap*>(views.GetViews())};
	auto handle{ descriptorHeap->GetGPUDescriptorHandleForHeapStart()};
	cmdList_->SetDescriptorHeaps(1, &descriptorHeap);
	cmdList_->SetGraphicsRootDescriptorTable(static_cast<std::uint32_t>(paramIdx), handle);
}

void Eugene::CommandList::SetShaderResourceViewComputeShader(ShaderResourceViews& views, std::uint64_t paramIdx)
{
	auto descriptorHeap{ static_cast<ID3D12DescriptorHeap*>(views.GetViews()) };
	auto handle{ descriptorHeap->GetGPUDescriptorHandleForHeapStart() };
	cmdList_->SetDescriptorHeaps(1, &descriptorHeap);
	cmdList_->SetComputeRootDescriptorTable(static_cast<std::uint32_t>(paramIdx), handle);
}

void Eugene::CommandList::SetSamplerView(SamplerViews& views, std::uint64_t paramIdx)
{
	ID3D12Device* device{ nullptr };
	auto descriptorHeap{ static_cast<ID3D12DescriptorHeap*>(views.GetViews()) };
	if (FAILED(descriptorHeap->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}
	auto handle{ descriptorHeap->GetGPUDescriptorHandleForHeapStart() };
	cmdList_->SetDescriptorHeaps(1, &descriptorHeap);
	cmdList_->SetGraphicsRootDescriptorTable(static_cast<std::uint32_t>(paramIdx), handle);
}



void Eugene::CommandList::Draw(std::uint32_t vertexCount, std::uint32_t instanceCount)
{
	cmdList_->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void Eugene::CommandList::DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceNum, std::uint32_t offset)
{
	cmdList_->DrawIndexedInstanced(indexCount, instanceNum, offset, 0, 0);
}

void Eugene::CommandList::Dispatch(const glm::u32vec3& count)
{
	cmdList_->Dispatch(count.x, count.y, count.z);
}


void Eugene::CommandList::TransitionRenderTargetBegin(ImageResource& resource)
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12Resource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cmdList_->ResourceBarrier(1, &barrier);
}

void Eugene::CommandList::TransitionRenderTargetEnd(ImageResource& resource)
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12Resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
	cmdList_->ResourceBarrier(1, &barrier);
}

void Eugene::CommandList::TransitionShaderResourceBegin(ImageResource& resource)
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12Resource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList_->ResourceBarrier(1, &barrier);
}

void Eugene::CommandList::TransitionShaderResourceEnd(ImageResource& resource)
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12Resource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);
	cmdList_->ResourceBarrier(1, &barrier);
}

void Eugene::CommandList::TransitionDepthBegin(ImageResource& resource)
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12Resource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	cmdList_->ResourceBarrier(1, &barrier);
}

void Eugene::CommandList::TransitionDepthEnd(ImageResource& resource)
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12Resource, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON);
	cmdList_->ResourceBarrier(1, &barrier);
}

void Eugene::CommandList::TransitionUnorderedAccessBegin(BufferResource& resource)
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12Resource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	cmdList_->ResourceBarrier(1, &barrier);
}

void Eugene::CommandList::TransitionUnorderedAccessEnd(BufferResource& resource)
{
	auto dx12Resource{ static_cast<ID3D12Resource*>(resource.GetResource()) };
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dx12Resource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
	cmdList_->ResourceBarrier(1, &barrier);
}


void Eugene::CommandList::CopyTexture(ImageResource& dest, BufferResource& src)
{
	auto dx12source{ static_cast<ID3D12Resource*>(src.GetResource()) };
	auto dx12destination{ static_cast<ID3D12Resource*>(dest.GetResource()) };
	ID3D12Device* device{ nullptr };
	if (FAILED(dx12source->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}

	auto subResource = dx12destination->GetDesc().MipLevels * dx12destination->GetDesc().DepthOrArraySize;
	std::array<CD3DX12_RESOURCE_BARRIER,maxSubResource> barrier ;

	for (int i = 0; i < subResource; i++)
	{
		// すべてのサブリソースにバリアをセット
		barrier[i] = CD3DX12_RESOURCE_BARRIER::Transition(dx12destination, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, i);
	}

	
	auto defState = D3D12_RESOURCE_STATE_COMMON;
	

	cmdList_->ResourceBarrier(subResource, barrier.data());


	D3D12_TEXTURE_COPY_LOCATION d{};
	d.pResource = dx12destination;
	d.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	d.SubresourceIndex = 0;

	std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, maxSubResource>  footprint;
	UINT64  totalSize = 0;
	D3D12_RESOURCE_DESC desc{ dx12destination->GetDesc() };
	
	D3D12_TEXTURE_COPY_LOCATION  s{};
	device->GetCopyableFootprints(&desc, 0, subResource, 0, footprint.data(), nullptr, nullptr, &totalSize);
	s.pResource = dx12source;
	s.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	
	for (int i = 0; i < subResource; i++)
	{
		// サブリソースごとのコピー
		s.PlacedFootprint = footprint[i];
		d.SubresourceIndex = i;
		cmdList_->CopyTextureRegion(&d, 0, 0, 0, &s, nullptr);
	}

	for (int i = 0; i < subResource; i++)
	{
		// すべてのサブリソースにバリアをセット
		barrier[i] = CD3DX12_RESOURCE_BARRIER::Transition(dx12destination, D3D12_RESOURCE_STATE_COPY_DEST | defState, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | defState);
	}
	cmdList_->ResourceBarrier(subResource, barrier.data());
}

void Eugene::CommandList::Resolve(ImageResource& dest, ImageResource& src)
{
	auto dx12source{ static_cast<ID3D12Resource*>(src.GetResource()) };
	const auto& sourceDesc{ dx12source->GetDesc() };
	auto sourceSubNum{ sourceDesc.MipLevels * sourceDesc.DepthOrArraySize };

	auto dx12destination{ static_cast<ID3D12Resource*>(dest.GetResource()) };
	const auto& destDesc{ dx12source->GetDesc() };
	auto destSubNum{ destDesc.MipLevels * destDesc.DepthOrArraySize };

	ID3D12Device* device{ nullptr };
	if (FAILED(dx12source->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}

	std::array<CD3DX12_RESOURCE_BARRIER, maxSubResource> barrier;

	// ソースに対してバリアを設定
	for (int i = 0; i < sourceSubNum; i++)
	{
		// すべてのサブリソースにバリアをセット
		barrier[i] = CD3DX12_RESOURCE_BARRIER::Transition(dx12source, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, i);
	}
	cmdList_->ResourceBarrier(sourceSubNum, barrier.data());

	// 書き込み先にバリアを設定
	for (int i = 0; i < destSubNum; i++)
	{
		// すべてのサブリソースにバリアをセット
		barrier[i] = CD3DX12_RESOURCE_BARRIER::Transition(dx12destination, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RESOLVE_DEST, i);
	}
	cmdList_->ResourceBarrier(destSubNum, barrier.data());

	cmdList_->ResolveSubresource(
		dx12destination, 0,
		dx12source, 0,
		sourceDesc.Format
	);

	// ソースに対してバリアを設定
	for (int i = 0; i < sourceSubNum; i++)
	{
		// すべてのサブリソースにバリアをセット
		barrier[i] = CD3DX12_RESOURCE_BARRIER::Transition(dx12source, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_COMMON, i);
	}
	cmdList_->ResourceBarrier(sourceSubNum, barrier.data());

	// 書き込み先にバリアを設定
	for (int i = 0; i < sourceSubNum; i++)
	{
		// すべてのサブリソースにバリアをセット
		barrier[i] = CD3DX12_RESOURCE_BARRIER::Transition(dx12destination, D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_COMMON, i);
	}
	cmdList_->ResourceBarrier(destSubNum, barrier.data());
}

void Eugene::CommandList::SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, std::optional<std::span<float, 4>> rtClear, std::pair<std::uint32_t, std::uint32_t> rtRange, std::optional<float> depthClear, std::uint32_t depthIndex)
{
	auto rtDh{ static_cast<ID3D12DescriptorHeap*>(renderTargetViews.GetViews()) };
	auto rtHandle{ rtDh->GetCPUDescriptorHandleForHeapStart() };
	ID3D12Device* device{ nullptr };
	if (FAILED(rtDh->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}

	// レンダーターゲットの最大数が8なのでその分確保
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 8> handles;
	std::uint64_t count{ 0ull };
	for (std::uint64_t i = rtRange.first; i < (rtRange.first + rtRange.second); i++)
	{
		// 最大数もしくはビューにある最大数分格納
		handles[count] = rtHandle;
		rtHandle.ptr += static_cast<ULONG_PTR>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		count++;
	}

	if (rtClear.has_value())
	{
		for (std::uint64_t i =0ull; i < count; i++)
		{
			cmdList_->ClearRenderTargetView(handles[i], rtClear.value().data(), 0, nullptr);
		}
	}

	auto depthDh{ static_cast<ID3D12DescriptorHeap*>(depthViews.GetViews()) };
	auto depthHandle{ depthDh->GetCPUDescriptorHandleForHeapStart() };
	if (FAILED(depthDh->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}
	depthHandle.ptr += static_cast<ULONG_PTR>(depthIndex) * static_cast<ULONG_PTR>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	if (depthClear.has_value())
	{
		cmdList_->ClearDepthStencilView(depthHandle, D3D12_CLEAR_FLAG_DEPTH, depthClear.value(), 0, 0, nullptr);
	}

	cmdList_->OMSetRenderTargets(static_cast<std::uint32_t>(count), handles.data(), false, &depthHandle);

}
void Eugene::CommandList::SetRenderTarget(RenderTargetViews& renderTargetViews,
	std::optional<std::span<float, 4>> rtClear,
	std::pair<std::uint32_t, std::uint32_t> rtRange
)
{
	auto rtDh{ static_cast<ID3D12DescriptorHeap*>(renderTargetViews.GetViews()) };
	
	ID3D12Device* device{ nullptr };
	if (FAILED(rtDh->GetDevice(__uuidof(*device), reinterpret_cast<void**>(&device))))
	{
		return;
	}
	auto rtHandle{ rtDh->GetCPUDescriptorHandleForHeapStart() };
	rtHandle.ptr += static_cast<ULONG_PTR>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)) * rtRange.first;

	// レンダーターゲットの最大数が8なのでその分確保
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 8> handles;
	std::uint64_t count{ 0ull };

	for (std::uint32_t i = 0; i < rtRange.second; i++)
	{
		// 最大数もしくはビューにある最大数分格納
		handles[i] = rtHandle;
		rtHandle.ptr += static_cast<ULONG_PTR>(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		count++;
	}

	if (rtClear.has_value())
	{
		for (std::uint64_t i = 0ull; i < count; i++)
		{
			cmdList_->ClearRenderTargetView(handles[i], rtClear.value().data(), 0, nullptr);
		}
	}

	cmdList_->OMSetRenderTargets(static_cast<std::uint32_t>(count), handles.data(), false, nullptr);
}
void Eugene::CommandList::CopyBuffer(BufferResource& dest, BufferResource& src)
{
	ID3D12Resource* srcResource{ static_cast<ID3D12Resource*>(src.GetResource()) };
	ID3D12Resource* destResource{ static_cast<ID3D12Resource*>(dest.GetResource()) };

	auto barrier{ CD3DX12_RESOURCE_BARRIER::Transition(srcResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE) };

	cmdList_->ResourceBarrier(1, &barrier);
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(destResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList_->ResourceBarrier(1, &barrier);
	cmdList_->CopyResource(destResource, srcResource);

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(srcResource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON);
	cmdList_->ResourceBarrier(1, &barrier);
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(destResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
	cmdList_->ResourceBarrier(1, &barrier);
}

#ifdef USE_IMGUI
void Eugene::CommandList::SetImguiCommand(ImDrawData* data) const
{
	auto dh = Graphics::GetInstance().imguiDescriptorHeap_.Get();
	cmdList_->SetDescriptorHeaps(1,&dh);
	ImGui_ImplDX12_RenderDrawData(
		data,
		cmdList_.Get()
	);
}

#endif
