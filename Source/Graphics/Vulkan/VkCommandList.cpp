#include "VkCommandList.h"
#include "../../../Include/Common/EugeneLibException.h"
#include "VkBufferResource.h"
#include "VkImageResource.h"

Eugene::VkCommandList::VkCommandList(const vk::Device& device, std::uint32_t familyIndex)
{
	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.setQueueFamilyIndex(familyIndex);
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	commandPool_ = device.createCommandPoolUnique(poolInfo);

	vk::CommandBufferAllocateInfo bufferInfo{};
	bufferInfo.setCommandPool(*commandPool_);
	bufferInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	bufferInfo.setCommandBufferCount(1);
	commandBuffer_ = std::move(device.allocateCommandBuffersUnique(bufferInfo)[0]);
}

void Eugene::VkCommandList::Begin(void)
{
	commandBuffer_->begin(vk::CommandBufferBeginInfo{});
}

void Eugene::VkCommandList::End(void)
{
	commandBuffer_->end();
}

void Eugene::VkCommandList::SetGraphicsPipeline(GraphicsPipeline& gpipeline)
{
}

void Eugene::VkCommandList::SetPrimitiveType(PrimitiveType type)
{
}

void Eugene::VkCommandList::SetScissorrect(const Vector2I& leftTop, const Vector2I& rightBottom)
{
	vk::Rect2D scissor{};
	scissor.setOffset({ leftTop.x,leftTop.y});
	scissor.setExtent(vk::Extent2D{static_cast<std::uint32_t>(rightBottom.x - leftTop.x), static_cast<std::uint32_t>(rightBottom.y - leftTop.y)});
	commandBuffer_->setScissor(0, scissor);
}

void Eugene::VkCommandList::SetViewPort(const Vector2& leftTop, const Vector2& size, float depthMin, float depthMax)
{
	vk::Viewport viewPort{};
	viewPort.setX(0.0f);
	viewPort.setY(0.0f);
	viewPort.setWidth(size.x);
	viewPort.setHeight(size.y);
	viewPort.setMaxDepth(depthMax);
	viewPort.setMinDepth(depthMin);
	commandBuffer_->setViewport(0, viewPort);
}

void Eugene::VkCommandList::SetVertexView(VertexView& view)
{
}

void Eugene::VkCommandList::SetIndexView(IndexView& view)
{
}

void Eugene::VkCommandList::SetShaderResourceView(ShaderResourceViews& views, std::uint64_t viewsIdx, std::uint64_t paramIdx)
{
}

void Eugene::VkCommandList::SetSamplerView(SamplerViews& views, std::uint64_t viewsIdx, std::uint64_t paramIdx)
{
}

void Eugene::VkCommandList::Draw(std::uint32_t vertexCount, std::uint32_t instanceCount)
{
}

void Eugene::VkCommandList::DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceNum, std::uint32_t offset)
{
}

void Eugene::VkCommandList::SetRenderTarget(RenderTargetViews& views, std::uint64_t idx)
{
}

void Eugene::VkCommandList::SetRenderTarget(RenderTargetViews& views, std::uint64_t startIdx, std::uint64_t endIdx)
{
}

void Eugene::VkCommandList::SetRenderTarget(RenderTargetViews& views)
{
}

void Eugene::VkCommandList::SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, std::uint64_t rtViewsIdx, std::uint64_t dsViewsIdx)
{
}

void Eugene::VkCommandList::ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color, std::uint64_t idx)
{
}

void Eugene::VkCommandList::ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color)
{
}

void Eugene::VkCommandList::TransitionRenderTargetBegin(ImageResource& resource)
{
	auto data{ static_cast<VkImageResource::Data*>(resource.GetResource()) };
	
	// メモリバリアをレンダーターゲットとして使用できるように変更します
	vk::ImageMemoryBarrier barrier{};

	// レイアウトを未定義からカラーアタッチメント(レンダーターゲット)に
	barrier.setOldLayout(vk::ImageLayout::eUndefined);
	barrier.setNewLayout(vk::ImageLayout::eColorAttachmentOptimal);
	barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
	barrier.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
	barrier.setImage(*data->image_);
	barrier.subresourceRange.setLayerCount(1);
	barrier.subresourceRange.setLevelCount(1);

	commandBuffer_->pipelineBarrier(
		vk::PipelineStageFlagBits::eAllCommands,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		static_cast<vk::DependencyFlagBits>(0),
		0, nullptr, 0, nullptr, 1,
		&barrier
	);
}

void Eugene::VkCommandList::TransitionRenderTargetEnd(ImageResource& resource)
{
}

void Eugene::VkCommandList::TransitionShaderResourceBegin(ImageResource& resource)
{
}

void Eugene::VkCommandList::TransitionShaderResourceEnd(ImageResource& resource)
{
}

void Eugene::VkCommandList::TransitionDepthBegin(ImageResource& resource)
{
}

void Eugene::VkCommandList::TransitionDepthEnd(ImageResource& resource)
{
}

void Eugene::VkCommandList::ClearDepth(DepthStencilViews& views, float clearValue, std::uint64_t idx)
{
}

void Eugene::VkCommandList::Copy(GpuResource& destination, GpuResource& source)
{
	throw EugeneLibException{"非対応です"};
}

void Eugene::VkCommandList::CopyTexture(GpuResource& destination, GpuResource& source)
{
	throw EugeneLibException{"非対応です"};
}

void Eugene::VkCommandList::CopyTexture(ImageResource& dest, BufferResource& src)
{
	auto destData = static_cast<VkImageResource::Data*>(dest.GetResource());
	auto srcData = static_cast<VkBufferData*>(src.GetResource());
	vk::ImageMemoryBarrier barrier{};
	barrier.setOldLayout(vk::ImageLayout::eUndefined);
	barrier.setNewLayout(vk::ImageLayout::eTransferDstOptimal);
	barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
	barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
	barrier.setImage(*destData->image_);
	barrier.subresourceRange.setLayerCount(1);
	barrier.subresourceRange.setLevelCount(1);

	auto texSize = dest.GetSize();
	commandBuffer_->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, static_cast<vk::DependencyFlagBits>(0), 0, nullptr, 0, nullptr, 1, &barrier);
	vk::BufferImageCopy region{};
	region.imageSubresource.layerCount = 1;
	region.imageExtent = vk::Extent3D{ static_cast<std::uint32_t>(texSize.x),static_cast<std::uint32_t>(texSize.y),1 };
	region.imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
	commandBuffer_->copyBufferToImage(*srcData->buffer_, *destData->image_, vk::ImageLayout::eTransferDstOptimal, region);

	barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
	barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
	barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

	commandBuffer_->pipelineBarrier(vk::PipelineStageFlagBits::eAllGraphics, vk::PipelineStageFlagBits::eAllGraphics, static_cast<vk::DependencyFlagBits>(0), 0, nullptr, 0, nullptr, 1, &barrier);



}

void Eugene::VkCommandList::CopyBuffer(BufferResource& dest, BufferResource& src)
{
	auto destData =  static_cast<VkBufferData*>(dest.GetResource());
	auto srcData = static_cast<VkBufferData*>(src.GetResource());
	vk::BufferCopy cpy{};
	cpy.setDstOffset(0);
	cpy.setSrcOffset(0);
	cpy.setSize(dest.GetSize());
	commandBuffer_->copyBuffer(*srcData->buffer_, *destData->buffer_, cpy);
}

void* Eugene::VkCommandList::GetCommandList(void)
{
	return &*commandBuffer_;
}
