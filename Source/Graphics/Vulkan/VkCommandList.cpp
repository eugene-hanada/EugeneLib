#include "VkCommandList.h"
#include "../../../Include/Common/EugeneLibException.h"
#include "VkBufferResource.h"
#include "VkImageResource.h"
#include "VkDepthStencilViews.h"
#include "VkRenderTargetViews.h"
#include "VkGraphicsPipeline.h"
#include "VkVertexView.h"
#include "VkShaderResourceViews.h"
#include "VkSamplerViews.h"

Eugene::VkCommandList::VkCommandList(const vk::Device& device, std::uint32_t familyIndex):
	isRendering_{false}, nowLayout_{nullptr}, semaphoreCount_{0ull}
{
	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.setQueueFamilyIndex(familyIndex);
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	commandPool_ = device.createCommandPoolUnique(poolInfo);

	vk::CommandBufferAllocateInfo bufferInfo{};
	bufferInfo.setCommandPool(*commandPool_);
	bufferInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	bufferInfo.setCommandBufferCount(1);
	data_.commandBuffer_ = std::move(device.allocateCommandBuffersUnique(bufferInfo)[0]);
}

void Eugene::VkCommandList::Begin(void)
{
	data_.commandBuffer_->begin(vk::CommandBufferBeginInfo{});
}

void Eugene::VkCommandList::End(void)
{
	data_.commandBuffer_->end();
	semaphoreCount_ = 0ull;
}

void Eugene::VkCommandList::SetGraphicsPipeline(GraphicsPipeline& gpipeline)
{
	auto pipeline{ static_cast<VkGraphicsPipeline::PipelineType*>(gpipeline.GetPipeline()) };
	data_.commandBuffer_->bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline->pipeline_);
	nowLayout_ = &pipeline->layout_;
}

void Eugene::VkCommandList::SetPrimitiveType(PrimitiveType type)
{
	vk::PrimitiveTopology t = static_cast<vk::PrimitiveTopology>(static_cast<size_t>(type) - 1ull);
	data_.commandBuffer_->setPrimitiveTopology(t);
}

void Eugene::VkCommandList::SetScissorrect(const Vector2I& leftTop, const Vector2I& rightBottom)
{
	vk::Rect2D scissor{};
	scissor.setOffset({ leftTop.x,leftTop.y});
	scissor.setExtent(vk::Extent2D{static_cast<std::uint32_t>(rightBottom.x - leftTop.x), static_cast<std::uint32_t>(rightBottom.y - leftTop.y)});
	data_.commandBuffer_->setScissor(0, scissor);
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
	data_.commandBuffer_->setViewport(0, viewPort);
}

void Eugene::VkCommandList::SetVertexView(VertexView& view)
{
	auto buffer = static_cast<vk::Buffer*>(view.GetView());
	vk::DeviceSize deviceSize{0};
	data_.commandBuffer_->bindVertexBuffers(0u, *buffer, deviceSize);
}

void Eugene::VkCommandList::SetIndexView(IndexView& view)
{
}

void Eugene::VkCommandList::SetShaderResourceView(ShaderResourceViews& views, std::uint64_t viewsIdx, std::uint64_t paramIdx)
{
	if (nowLayout_ == nullptr)
	{
		return;
	}
	auto data = static_cast<VkShaderResourceViews::Data*>(views.GetViews());
	data_.commandBuffer_->bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics,
		*nowLayout_,
		static_cast<std::uint32_t>(paramIdx),
		1u, 
		&*data->descriptorSet_,
		0u, 
		nullptr
	);
	
}

void Eugene::VkCommandList::SetSamplerView(SamplerViews& views, std::uint64_t viewsIdx, std::uint64_t paramIdx)
{
	if (nowLayout_ == nullptr)
	{
		return;
	}
	auto data = static_cast<VkSamplerViews::Data*>(views.GetViews());
	data_.commandBuffer_->bindDescriptorSets(
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
	data_.commandBuffer_->draw(vertexCount, instanceCount, 0, 0);
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
	if (isRendering_)
	{
		// すでにDynamicRenderingを開始しているので終了しとく
		data_.commandBuffer_->endRendering();
	}

	auto& depth{ (*static_cast<std::vector<vk::UniqueImageView>*>(depthViews.GetViews()))[dsViewsIdx]};
	auto& renderTarget{ (*static_cast<VkRenderTargetViews::ViewsType*>(renderTargetViews.GetViews()))[rtViewsIdx] };
	
	vk::RenderingInfo rdInfo{};
	vk::RenderingAttachmentInfo colorAttachment{};
	colorAttachment.setImageView(*renderTarget.imageView);
	colorAttachment.setImageLayout(vk::ImageLayout::eAttachmentOptimal);
	colorAttachment.setLoadOp(vk::AttachmentLoadOp::eLoad);
	colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);

	vk::RenderingAttachmentInfo depthAttachment{};
	depthAttachment.setImageView(*depth);
	depthAttachment.setImageLayout(vk::ImageLayout::eDepthAttachmentOptimal);
	depthAttachment.setLoadOp(vk::AttachmentLoadOp::eLoad);
	depthAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);

	rdInfo.setColorAttachments(colorAttachment);
	//rdInfo.setPDepthAttachment(&depthAttachment);
	rdInfo.setRenderArea(vk::Rect2D{vk::Offset2D{}, vk::Extent2D{static_cast<std::uint32_t>(renderTarget.size.x), static_cast<std::uint32_t>(renderTarget.size.y)}});
	rdInfo.setLayerCount(1);
	
	data_.commandBuffer_->beginRendering(rdInfo);
	isRendering_ = true;
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
		data_.commandBuffer_->endRendering();
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
			colorAttachments[colorAttachmentIdx].setClearValue(vk::ClearColorValue{ std::array{0.0f,0.0f,0.0f,1.0f} });
		}
		else
		{
			colorAttachments[colorAttachmentIdx].setLoadOp(vk::AttachmentLoadOp::eLoad);
		}

		if (renderTarget[i].semaphore_.has_value())
		{
			// セマフォがある場合
			if (data_.semaphores_.size() <= semaphoreCount_)
			{
				data_.semaphores_.emplace_back(renderTarget[i].semaphore_.value());
			}
			else
			{
				data_.semaphores_[semaphoreCount_] = renderTarget[i].semaphore_.value();
			}
			semaphoreCount_++;
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

	data_.commandBuffer_->beginRendering(rdInfo);
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
		data_.commandBuffer_->endRendering();
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
			colorAttachments[colorAttachmentIdx].setClearValue(vk::ClearColorValue{ std::array{0.0f,0.0f,0.0f,1.0f} });
		}
		else
		{
			colorAttachments[colorAttachmentIdx].setLoadOp(vk::AttachmentLoadOp::eLoad);
		}

		if (renderTarget[i].semaphore_.has_value())
		{
			// セマフォがある場合
			if (data_.semaphores_.size() <= semaphoreCount_)
			{
				data_.semaphores_.emplace_back(renderTarget[i].semaphore_.value());
			}
			else
			{
				data_.semaphores_[semaphoreCount_] = renderTarget[i].semaphore_.value();
			}
			semaphoreCount_++;
		}
	}

	rdInfo.setPColorAttachments(colorAttachments.data());
	rdInfo.setColorAttachmentCount(rtRange.second);
	rdInfo.setRenderArea(vk::Rect2D{vk::Offset2D{}, vk::Extent2D{static_cast<std::uint32_t>(renderTarget[0].size.x), static_cast<std::uint32_t>(renderTarget[0].size.y)}});
	rdInfo.setLayerCount(1);
	rdInfo.setLayerCount(1);
	data_.commandBuffer_->beginRendering(rdInfo);
	isRendering_ = true;
}

void Eugene::VkCommandList::ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color, std::uint64_t idx)
{
	throw EugeneLibException{"対応していません"};
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

	data_.commandBuffer_->pipelineBarrier(
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
		data_.commandBuffer_->endRendering();
	}
	isRendering_ = false;

	auto data{ static_cast<VkImageResource::Data*>(resource.GetResource()) };

	// メモリバリアをレンダーターゲットとして使用できるように変更します
	vk::ImageMemoryBarrier barrier{};

	// レイアウトを未定義からカラーアタッチメント(レンダーターゲット)に
	barrier.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal);
	barrier.setNewLayout(vk::ImageLayout::ePresentSrcKHR);
	barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
	barrier.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
	barrier.setImage(*data->image_);
	barrier.subresourceRange.setLayerCount(1);
	barrier.subresourceRange.setLevelCount(1);

	data_.commandBuffer_->pipelineBarrier(
		vk::PipelineStageFlagBits::eAllGraphics,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		static_cast<vk::DependencyFlagBits>(0),
		0, nullptr, 0, nullptr, 1,
		&barrier
	);
}

void Eugene::VkCommandList::TransitionShaderResourceBegin(ImageResource& resource)
{
}

void Eugene::VkCommandList::TransitionShaderResourceEnd(ImageResource& resource)
{
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

	data_.commandBuffer_->pipelineBarrier(
		vk::PipelineStageFlagBits::eAllGraphics,
		vk::PipelineStageFlagBits::eAllGraphics,
		static_cast<vk::DependencyFlagBits>(0),
		0, nullptr, 0, nullptr, 1,
		&barrier
	);

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
	data_.commandBuffer_->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, static_cast<vk::DependencyFlagBits>(0), 0, nullptr, 0, nullptr, 1, &barrier);
	vk::BufferImageCopy region{};
	region.imageSubresource.layerCount = 1;
	region.imageExtent = vk::Extent3D{ static_cast<std::uint32_t>(texSize.x),static_cast<std::uint32_t>(texSize.y),1 };
	region.imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
	data_.commandBuffer_->copyBufferToImage(*srcData->buffer_, *destData->image_, vk::ImageLayout::eTransferDstOptimal, region);

	barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
	barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
	barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

	data_.commandBuffer_->pipelineBarrier(vk::PipelineStageFlagBits::eAllGraphics, vk::PipelineStageFlagBits::eAllGraphics, static_cast<vk::DependencyFlagBits>(0), 0, nullptr, 0, nullptr, 1, &barrier);



}

void Eugene::VkCommandList::CopyBuffer(BufferResource& dest, BufferResource& src)
{
	auto destData =  static_cast<VkBufferData*>(dest.GetResource());
	auto srcData = static_cast<VkBufferData*>(src.GetResource());
	vk::BufferCopy cpy{};
	cpy.setDstOffset(0);
	cpy.setSrcOffset(0);
	cpy.setSize(dest.GetSize());
	data_.commandBuffer_->copyBuffer(*srcData->buffer_, *destData->buffer_, cpy);
}

void* Eugene::VkCommandList::GetCommandList(void)
{
	return &data_;
}
