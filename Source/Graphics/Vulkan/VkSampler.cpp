#include "../../../Include/Graphics/Vulkan/VkSampler.h"
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"


Eugene::Sampler::Sampler( const SamplerLayout& layout)
{
	constexpr vk::SamplerAddressMode toAddressMode[]{
		vk::SamplerAddressMode::eRepeat,
		vk::SamplerAddressMode::eMirroredRepeat,
		vk::SamplerAddressMode::eClampToBorder,
		vk::SamplerAddressMode::eMirrorClampToEdge
	};


	vk::SamplerCreateInfo smpInfo{};

	smpInfo.setAnisotropyEnable(false);
	switch (layout.filter_)
	{
	case SampleFilter::Point:
		smpInfo.setMinFilter(vk::Filter::eNearest);
		smpInfo.setMagFilter(vk::Filter::eNearest);
		break;

	case SampleFilter::Linear:
		smpInfo.setMinFilter(vk::Filter::eLinear);
		smpInfo.setMagFilter(vk::Filter::eLinear);
		break;

	case SampleFilter::Anisotropic:
		smpInfo.setMinFilter(vk::Filter::eNearest);
		smpInfo.setMagFilter(vk::Filter::eNearest);
		smpInfo.setAnisotropyEnable(VK_TRUE);
		break;
	default:
		// 特に実装しない
		break;
	}

	

	smpInfo.setAddressModeU(toAddressMode[static_cast<size_t>(layout.u_) -1]);
	smpInfo.setAddressModeV(toAddressMode[static_cast<size_t>(layout.v_) - 1]);
	smpInfo.setAddressModeW(toAddressMode[static_cast<size_t>(layout.w_) - 1]);
	smpInfo.setMaxAnisotropy(layout.maxAnisotropy_);
	smpInfo.setBorderColor(vk::BorderColor::eIntOpaqueBlack);
	smpInfo.setUnnormalizedCoordinates(false);
	smpInfo.setCompareEnable(SamplerComparison::Non != layout.comparison_);
	smpInfo.setCompareOp(static_cast<vk::CompareOp>(static_cast<size_t>(layout.comparison_) - 1ull));
	smpInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
	smpInfo.setMipLodBias(0.0f);
	smpInfo.setMinLod(layout.minLod_);
	smpInfo.setMaxLod(layout.maxLod_);
	
	sampler_ = Graphics::GetInstance().device_->createSamplerUnique(smpInfo);

}
