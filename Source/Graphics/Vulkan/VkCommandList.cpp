#include "VkCommandList.h"
#include "../../../Include/Utils/EugeneLibException.h"
#include "VkBufferResource.h"
#include "VkImageResource.h"
#include "VkDepthStencilViews.h"
#include "VkRenderTargetViews.h"
#include "VkGraphicsPipeline.h"
#include "VkVertexView.h"
#include "VkIndexView.h"
#include "VkShaderResourceViews.h"
#include "VkSamplerViews.h"
#include "../../../Include/Graphics/Image.h"

#ifdef USE_IMGUI
#include "VkGraphics.h"
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#endif

Eugene::VkCommandList::VkCommandList(const vk::Device& device, std::uint32_t familyIndex):
	isRendering_{false}, nowLayout_{nullptr}
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
	if (isRendering_)
	{
		commandBuffer_->endRendering();
		isRendering_ = false;
	}
	commandBuffer_->end();
}

void Eugene::VkCommandList::SetGraphicsPipeline(GraphicsPipeline& gpipeline)
{
	auto pipeline{ static_cast<VkGraphicsPipeline::PipelineType*>(gpipeline.GetPipeline()) };
	commandBuffer_->bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline->pipeline_);
	nowLayout_ = &pipeline->layout_;
}

void Eugene::VkCommandList::SetPrimitiveType(PrimitiveType type)
{
	vk::PrimitiveTopology t = static_cast<vk::PrimitiveTopology>(static_cast<size_t>(type) - 1ull);
	commandBuffer_->setPrimitiveTopology(t);
}

void Eugene::VkCommandList::SetScissorrect(const glm::ivec2& leftTop, const glm::ivec2& rightBottom)
{
	vk::Rect2D scissor{};
	scissor.setOffset({ leftTop.x,leftTop.y});
	scissor.setExtent(vk::Extent2D{static_cast<std::uint32_t>(rightBottom.x - leftTop.x), static_cast<std::uint32_t>(rightBottom.y - leftTop.y)});
	commandBuffer_->setScissor(0, scissor);
}

void Eugene::VkCommandList::SetViewPort(const glm::vec2& leftTop, const glm::vec2& size, float depthMin, float depthMax)
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
	auto buffer = static_cast<vk::Buffer*>(view.GetView());
	vk::DeviceSize deviceSize{0};
	commandBuffer_->bindVertexBuffers(0u, *buffer, deviceSize);
}

void Eugene::VkCommandList::SetIndexView(IndexView& view)
{
	auto buffer = static_cast<vk::Buffer*>(view.GetView());
	vk::DeviceSize deviceSize{0};
	commandBuffer_->bindIndexBuffer(*buffer, deviceSize, vk::IndexType::eUint16);
}

void Eugene::VkCommandList::SetShaderResourceView(ShaderResourceViews& views, std::uint64_t paramIdx)
{
	if (nowLayout_ == nullptr)
	{
		return;
	}
	auto data = static_cast<VkShaderResourceViews::Data*>(views.GetViews());
	commandBuffer_->bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics,
		*nowLayout_,
		static_cast<std::uint32_t>(paramIdx),
		1u, 
		&*data->descriptorSet_,
		0u, 
		nullptr
	);
	
}

void Eugene::VkCommandList::SetSamplerView(SamplerViews& views, std::uint64_t paramIdx)
{
	if (nowLayout_ == nullptr)
	{
		return;
	}
	auto data = static_cast<VkSamplerViews::Data*>(views.GetViews());
	commandBuffer_->bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics,
		*nowLayout_,
		static_cast<std::uint32_t>(paramIdx),
		1u,
		&*data->descriptorSet_,
		0u,
		nullptr
	);

}

void Eugene::VkCommandList::Draw(std::uint32_t vertexCount, std::uint32_t instanceCount)
{
	commandBuffer_->draw(vertexCount, instanceCount, 0, 0);
}

void Eugene::VkCommandList::DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceNum, std::uint32_t offset)
{
	commandBuffer_->drawIndexed(indexCount, instanceNum, offset, 0u, 0u);
}



void Eugene::VkCommandList::SetRenderTarget(
	RenderTargetViews& renderTargetViews,
	DepthStencilViews& depthViews,
	std::optional<std::span<float, 4>> rtClear,
	std::pair<std::uint32_t, std::uint32_t> rtRange,
	std::optional<float> depthClear,
	std::uint32_t depthIndex
)
{
	if (isRendering_)
	{
		// すでにDynamicRenderingを開始しているので終了しとく
		commandBuffer_->endRendering();
	}

	
	auto& renderTarget{ (*static_cast<VkRenderTargetViews::ViewsType*>(renderTargetViews.GetViews()))};
	vk::RenderingInfo rdInfo{};
	
	// レンダーターゲットをセットする
	std::array<vk::RenderingAttachmentInfo, 8> colorAttachments;
	auto colorAttachmentIdx{ 0 };
	for (std::uint32_t i = rtRange.first; i < rtRange.second; i++)
	{
		colorAttachments[colorAttachmentIdx].setImageView(*renderTarget[i].imageView);
		colorAttachments[colorAttachmentIdx].setImageLayout(vk::ImageLayout::eAttachmentOptimal);
		colorAttachments[colorAttachmentIdx].setStoreOp(vk::AttachmentStoreOp::eStore);
		if (rtClear.has_value())
		{
			colorAttachments[colorAttachmentIdx].setLoadOp(vk::AttachmentLoadOp::eClear);
			colorAttachments[colorAttachmentIdx].setClearValue(vk::ClearColorValue{std::array{rtClear.value()[0], rtClear.value()[1], rtClear.value()[2], rtClear.value()[3]} });
		}
		else
		{
			colorAttachments[colorAttachmentIdx].setLoadOp(vk::AttachmentLoadOp::eLoad);
		}
	}

	// 深度バッファをセットする
	vk::RenderingAttachmentInfo depthAttachment{};
	auto& depth{ (*static_cast<std::vector<vk::UniqueImageView>*>(depthViews.GetViews())) };
	depthAttachment.setImageView(*depth[depthIndex]);
	depthAttachment.setImageLayout(vk::ImageLayout::eDepthAttachmentOptimal);
	depthAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
	if (depthClear.has_value())
	{
		depthAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
		depthAttachment.setClearValue(vk::ClearDepthStencilValue{depthClear.value()});
	}
	else
	{
		depthAttachment.setLoadOp(vk::AttachmentLoadOp::eLoad);
	}
	rdInfo.setPDepthAttachment(&depthAttachment);
	rdInfo.setColorAttachmentCount(rtRange.second);
	rdInfo.setPColorAttachments(colorAttachments.data());
	rdInfo.setRenderArea(vk::Rect2D{vk::Offset2D{}, vk::Extent2D{static_cast<std::uint32_t>(renderTarget[0].size.x), static_cast<std::uint32_t>(renderTarget[0].size.y)}});
	rdInfo.setLayerCount(1);

	commandBuffer_->beginRendering(rdInfo);
	isRendering_ = true;
}

void Eugene::VkCommandList::SetRenderTarget(
	RenderTargetViews& renderTargetViews,
	std::optional<std::span<float, 4>> rtClear,
	std::pair<std::uint32_t, std::uint32_t> rtRange
)
{
	if (isRendering_)
	{
		// すでにDynamicRenderingを開始しているので終了しとく
		commandBuffer_->endRendering();
	}


	auto& renderTarget{ (*static_cast<VkRenderTargetViews::ViewsType*>(renderTargetViews.GetViews())) };
	vk::RenderingInfo rdInfo{};

	// レンダーターゲットをセットする
	std::array<vk::RenderingAttachmentInfo, 8> colorAttachments;
	auto colorAttachmentIdx{ 0 };
	for (std::uint32_t i = rtRange.first; i < (rtRange.second + rtRange.first); i++)
	{
		colorAttachments[colorAttachmentIdx].setImageView(*renderTarget[i].imageView);
		colorAttachments[colorAttachmentIdx].setImageLayout(vk::ImageLayout::eAttachmentOptimal);
		colorAttachments[colorAttachmentIdx].setStoreOp(vk::AttachmentStoreOp::eStore);
		if (rtClear.has_value())
		{
			colorAttachments[colorAttachmentIdx].setLoadOp(vk::AttachmentLoadOp::eClear);
			colorAttachments[colorAttachmentIdx].setClearValue(vk::ClearColorValue{ std::array{rtClear.value()[0], rtClear.value()[1], rtClear.value()[2], rtClear.value()[3]} });
		}
		else
		{
			colorAttachments[colorAttachmentIdx].setLoadOp(vk::AttachmentLoadOp::eLoad);
		}
	}

	rdInfo.setPColorAttachments(colorAttachments.data());
	rdInfo.setColorAttachmentCount(rtRange.second);
	rdInfo.setRenderArea(vk::Rect2D{vk::Offset2D{}, vk::Extent2D{static_cast<std::uint32_t>(renderTarget[0].size.x), static_cast<std::uint32_t>(renderTarget[0].size.y)}});
	rdInfo.setLayerCount(1);
	rdInfo.setLayerCount(1);
	commandBuffer_->beginRendering(rdInfo);
	isRendering_ = true;
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
		vk::PipelineStageFlagBits::eAllGraphics,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		static_cast<vk::DependencyFlagBits>(0),
		0, nullptr, 0, nullptr, 1,
		&barrier
	);
}

void Eugene::VkCommandList::TransitionRenderTargetEnd(ImageResource& resource)
{
	if (isRendering_)
	{
		commandBuffer_->endRendering();
	}
	isRendering_ = false;

	auto data{ static_cast<VkImageResource::Data*>(resource.GetResource()) };

	// メモリバリアをレンダーターゲットとして使用できるように変更します
	vk::ImageMemoryBarrier barrier{};

	// レイアウトを未定義からカラーアタッチメント(レンダーターゲット)に
	barrier.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal);
#ifdef USE_IMGUI

#else
	barrier.setNewLayout(vk::ImageLayout::ePresentSrcKHR);

	barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
	barrier.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
	barrier.setImage(*data->image_);
	barrier.subresourceRange.setLayerCount(1);
	barrier.subresourceRange.setLevelCount(1);

	commandBuffer_->pipelineBarrier(
		vk::PipelineStageFlagBits::eAllGraphics,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		static_cast<vk::DependencyFlagBits>(0),
		0, nullptr, 0, nullptr, 1,
		&barrier
	);
#endif
}

void Eugene::VkCommandList::TransitionShaderResourceBegin(ImageResource& resource)
{
	auto data{ static_cast<VkImageResource::Data*>(resource.GetResource()) };

	// メモリバリアをレンダーターゲットとして使用できるように変更します
	vk::ImageMemoryBarrier barrier{};

	barrier.setOldLayout(vk::ImageLayout::eUndefined);
	barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
	barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eShaderRead);
	barrier.setDstAccessMask(vk::AccessFlagBits::eNone);
	barrier.setImage(*data->image_);
	barrier.subresourceRange.setLayerCount(1);
	barrier.subresourceRange.setLevelCount(1);

	commandBuffer_->pipelineBarrier(
		vk::PipelineStageFlagBits::eAllGraphics,
		vk::PipelineStageFlagBits::eAllGraphics,
		static_cast<vk::DependencyFlagBits>(0),
		0, nullptr, 0, nullptr, 1,
		&barrier
	);
}

void Eugene::VkCommandList::TransitionShaderResourceEnd(ImageResource& resource)
{
	auto data{ static_cast<VkImageResource::Data*>(resource.GetResource()) };

	// メモリバリアをレンダーターゲットとして使用できるように変更します
	vk::ImageMemoryBarrier barrier{};

	barrier.setOldLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
	barrier.setNewLayout(vk::ImageLayout::eUndefined);
	barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
	barrier.setDstAccessMask(vk::AccessFlagBits::eNone);
	barrier.setImage(*data->image_);
	barrier.subresourceRange.setLayerCount(1);
	barrier.subresourceRange.setLevelCount(1);

	commandBuffer_->pipelineBarrier(
		vk::PipelineStageFlagBits::eAllGraphics,
		vk::PipelineStageFlagBits::eAllGraphics,
		static_cast<vk::DependencyFlagBits>(0),
		0, nullptr, 0, nullptr, 1,
		&barrier
	);
}

void Eugene::VkCommandList::TransitionDepthBegin(ImageResource& resource)
{

	auto data{ static_cast<VkImageResource::Data*>(resource.GetResource()) };

	// メモリバリアをレンダーターゲットとして使用できるように変更します
	vk::ImageMemoryBarrier barrier{};

	// レイアウトを未定義からデプスに
	barrier.setOldLayout(vk::ImageLayout::eUndefined);
	barrier.setNewLayout(vk::ImageLayout::eDepthAttachmentOptimal);
	barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
	barrier.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite);
	barrier.setImage(*data->image_);
	barrier.subresourceRange.setLayerCount(1);
	barrier.subresourceRange.setLevelCount(1);

	commandBuffer_->pipelineBarrier(
		vk::PipelineStageFlagBits::eAllGraphics,
		vk::PipelineStageFlagBits::eAllGraphics,
		static_cast<vk::DependencyFlagBits>(0),
		0, nullptr, 0, nullptr, 1,
		&barrier
	);

}

void Eugene::VkCommandList::TransitionDepthEnd(ImageResource& resource)
{
	//auto data{ static_cast<VkImageResource::Data*>(resource.GetResource()) };

	//// メモリバリアをレンダーターゲットとして使用できるように変更します
	//vk::ImageMemoryBarrier barrier{};

	//// レイアウトを未定義からデプスに
	//barrier.setOldLayout(vk::ImageLayout::eDepthAttachmentOptimal);
	//barrier.setNewLayout(vk::ImageLayout::eDepthAttachmentOptimal);
	//barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	//barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	//barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
	//barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
	//barrier.setDstAccessMask(vk::AccessFlagBits::eNone);
	//barrier.setImage(*data->image_);
	//barrier.subresourceRange.setLayerCount(1);
	//barrier.subresourceRange.setLevelCount(1);

	//commandBuffer_->pipelineBarrier(
	//	vk::PipelineStageFlagBits::eAllGraphics,
	//	vk::PipelineStageFlagBits::eAllGraphics,
	//	static_cast<vk::DependencyFlagBits>(0),
	//	0, nullptr, 0, nullptr, 1,
	//	&barrier
	//);
}


void Eugene::VkCommandList::CopyTexture(ImageResource& dest, BufferResource& src)
{
	auto destData = static_cast<VkImageResource::Data*>(dest.GetResource());
	auto srcData = static_cast<VkBufferData*>(src.GetResource());
	auto mipmapLevels = destData->mipmapLevels_;
	auto arraySize = destData->arraySize_;
	vk::ImageMemoryBarrier barrier{};
	barrier.setOldLayout(vk::ImageLayout::eUndefined);
	barrier.setNewLayout(vk::ImageLayout::eTransferDstOptimal);
	barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
	barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
	barrier.setImage(*destData->image_);
	barrier.subresourceRange.setLayerCount(arraySize);
	barrier.subresourceRange.setLevelCount(mipmapLevels);
	
	auto texSize = dest.GetSize();
	commandBuffer_->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, static_cast<vk::DependencyFlagBits>(0), 0, nullptr, 0, nullptr, 1, &barrier);
	std::array<vk::BufferImageCopy, maxSubResource> regionArray;
	
	
	auto nowSize = texSize;
	auto OffsetSize = 0ull;
	const auto pixelPerSize = destData->pixelPerSize;

	// ミップマップ分BufferImageCopyを設定する
	for (auto i = 0u; i < mipmapLevels; i++)
	{
		regionArray[i].imageExtent = vk::Extent3D{ static_cast<std::uint32_t>(nowSize.x),static_cast<std::uint32_t>(nowSize.y),1 };
		regionArray[i].bufferOffset = OffsetSize;
		regionArray[i].imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
		regionArray[i].imageSubresource.setMipLevel(i);
		regionArray[i].imageSubresource.setLayerCount(arraySize);

		// オフセットサイズを計算
		OffsetSize += calcSizeMap.at(dest.GetFormat())( nowSize.x, nowSize.y, pixelPerSize);
		
		// 次のサイズを計算
		nowSize /= 2;
	}

	commandBuffer_->copyBufferToImage(*srcData->buffer_, *destData->image_, vk::ImageLayout::eTransferDstOptimal, std::span{ regionArray.data(),mipmapLevels });

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

void Eugene::VkCommandList::Resolve(ImageResource& dest, ImageResource& src)
{
	auto destData = static_cast<VkImageResource::Data*>(dest.GetResource());
	auto srcData = static_cast<VkImageResource::Data*>(src.GetResource());

	vk::ImageMemoryBarrier barrier{};
	barrier.setOldLayout(vk::ImageLayout::eUndefined);
	barrier.setNewLayout(vk::ImageLayout::eTransferDstOptimal);
	barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
	barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
	barrier.setImage(*destData->image_);
	barrier.subresourceRange.setLayerCount(destData->arraySize_);
	barrier.subresourceRange.setLevelCount(destData->mipmapLevels_);

	commandBuffer_->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, static_cast<vk::DependencyFlagBits>(0), 0, nullptr, 0, nullptr, 1, &barrier);

	barrier.setImage(*srcData->image_);
	barrier.setNewLayout(vk::ImageLayout::eTransferSrcOptimal);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
	barrier.setDstAccessMask(vk::AccessFlagBits::eNone);
	barrier.subresourceRange.setLayerCount(srcData->arraySize_);
	barrier.subresourceRange.setLevelCount(srcData->mipmapLevels_);

	commandBuffer_->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, static_cast<vk::DependencyFlagBits>(0), 0, nullptr, 0, nullptr, 1, &barrier);

	vk::ImageResolve region;
	const auto& size{ src.GetSize() };
	region.dstSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
	region.dstSubresource.setLayerCount(destData->arraySize_);
	region.dstSubresource.setLayerCount(destData->mipmapLevels_);
	region.srcSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
	region.srcSubresource.setLayerCount(srcData->arraySize_);
	region.srcSubresource.setLayerCount(srcData->mipmapLevels_);
	region.setExtent({ static_cast<std::uint32_t>(size .x),static_cast<std::uint32_t>(size .y),1});

	commandBuffer_->resolveImage(
		*srcData->image_,
		vk::ImageLayout::eTransferSrcOptimal,
		*destData->image_,
		vk::ImageLayout::eTransferDstOptimal,
		region
	);

	barrier.setImage(*destData->image_);
	barrier.setNewLayout(vk::ImageLayout::ePresentSrcKHR);
	barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
	barrier.setDstAccessMask(vk::AccessFlagBits::eNone);
	barrier.subresourceRange.setLayerCount(destData->arraySize_);
	barrier.subresourceRange.setLevelCount(destData->mipmapLevels_);
	commandBuffer_->pipelineBarrier(vk::PipelineStageFlagBits::eAllGraphics, vk::PipelineStageFlagBits::eAllGraphics, static_cast<vk::DependencyFlagBits>(0), 0, nullptr, 0, nullptr, 1, &barrier);

	//barrier.setImage(*srcData->image_);
	//barrier.setNewLayout(vk::ImageLayout::eUndefined);
	//barrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal);
	//barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
	//barrier.setDstAccessMask(vk::AccessFlagBits::eNone);
	//barrier.subresourceRange.setLayerCount(srcData->arraySize_);
	//barrier.subresourceRange.setLevelCount(srcData->mipmapLevels_);
	//commandBuffer_->pipelineBarrier(vk::PipelineStageFlagBits::eAllGraphics, vk::PipelineStageFlagBits::eAllGraphics, static_cast<vk::DependencyFlagBits>(0), 0, nullptr, 0, nullptr, 1, &barrier);

}

void* Eugene::VkCommandList::GetCommandList(void)
{
	return &commandBuffer_;
}

#ifdef USE_IMGUI
void Eugene::VkCommandList::SetImguiCommand(ImDrawData* data, Graphics& graphics) const
{
	const bool main_is_minimized = (data->DisplaySize.x <= 0.0f || data->DisplaySize.y <= 0.0f);
	if (main_is_minimized)
	{
		return;
	}
	auto& vkGraphics = static_cast<VkGraphics&>(graphics);
	vk::RenderPassBeginInfo info{};
	info.setRenderPass(vkGraphics.GetRenderPass());
	auto size = graphics.GetBackBufferResource().GetSize();
	info.setRenderArea(vk::Rect2D{ vk::Offset2D{0,0} , vk::Extent2D{static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y)} });
	info.setFramebuffer(vkGraphics.GetFrameBuffer());
	
	commandBuffer_->beginRenderPass(info, vk::SubpassContents::eInline);
	ImGui_ImplVulkan_RenderDrawData(data, *commandBuffer_);
	commandBuffer_->endRenderPass();
}
#endif