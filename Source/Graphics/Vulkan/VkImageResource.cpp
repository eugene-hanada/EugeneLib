#include "../../../Include/Graphics/Vulkan/VkImageResource.h"
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"

Eugene::ImageResource::ImageResource(const TextureInfo& info):
	format_{info.format}
{
	auto format{ Graphics::FormatToVkFormat[static_cast<size_t>(info.format)] };
	vk::ImageCreateInfo imageInfo{};
	imageInfo.setArrayLayers(info.arraySize);
	imageInfo.setMipLevels(info.mipLevels);
	imageInfo.extent.setHeight(info.height);
	imageInfo.extent.setWidth(info.width);
	imageInfo.extent.setDepth(1u);
	imageInfo.setImageType(vk::ImageType::e2D);
	imageInfo.setTiling(vk::ImageTiling::eOptimal);
	imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
	imageInfo.setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
	imageInfo.setSharingMode(vk::SharingMode::eExclusive);
	imageInfo.setSamples(vk::SampleCountFlagBits::e1);
	imageInfo.setFormat(format);
	vma::AllocationCreateInfo allocInfo{};
	allocInfo.setUsage(vma::MemoryUsage::eAuto);

	auto result = Graphics::GetInstance().allocator_->createImageUnique(imageInfo, allocInfo);
	allocation_ = std::move(result.second);
	imageData_.image_ = std::move(result.first);

	imageData_.arraySize_ = info.arraySize;
	imageData_.mipmapLevels_ = info.mipLevels;
	imageData_.pixelPerSize_ = info.pixelPerBite;

	size_ = { static_cast<int>(info.width),static_cast<int>(info.height) };
}

Eugene::ImageResource::ImageResource(const glm::ivec2& size, float clearValue, std::uint8_t sampleCount)
{
	constexpr auto format{ vk::Format::eD32Sfloat };
	format_ = Format::D32_FLOAT;
	vk::ImageCreateInfo imageInfo{};
	imageInfo.setArrayLayers(1);
	imageInfo.setMipLevels(1);
	imageInfo.extent.setHeight(size.y);
	imageInfo.extent.setWidth(size.x);
	imageInfo.extent.setDepth(1u);
	imageInfo.setTiling(vk::ImageTiling::eOptimal);
	imageInfo.setImageType(vk::ImageType::e2D);
	imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
	imageInfo.setUsage(vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eDepthStencilAttachment);
	imageInfo.setSharingMode(vk::SharingMode::eExclusive);
	imageInfo.setSamples(static_cast<vk::SampleCountFlagBits>(sampleCount));
	imageInfo.setFormat(format);

	vma::AllocationCreateInfo allocInfo{};
	allocInfo.setUsage(vma::MemoryUsage::eAuto);

	auto result = Graphics::GetInstance().allocator_->createImageUnique(imageInfo, allocInfo);
	allocation_ = std::move(result.second);
	imageData_.image_ = std::move(result.first);

	imageData_.arraySize_ = 1;
	imageData_.mipmapLevels_ = 1;
	imageData_.pixelPerSize_ = 4u;
	size_ = size;
}

Eugene::ImageResource::ImageResource(
	const glm::ivec2& size,
	Format format,
	std::uint32_t arraySize,
	std::uint8_t mipLeveles,
	std::uint8_t sampleCount,
	std::optional<std::span<float, 4>> clearColor) :
	format_{format}
{
	vk::ImageCreateInfo imageInfo{};
	imageInfo.setArrayLayers(arraySize);
	imageInfo.setMipLevels(mipLeveles);
	imageInfo.extent.setHeight(size.y);
	imageInfo.extent.setWidth(size.x);
	imageInfo.extent.setDepth(1u);
	imageInfo.setSamples(static_cast<vk::SampleCountFlagBits>(sampleCount));
	imageInfo.setTiling(vk::ImageTiling::eOptimal);
	imageInfo.setImageType(vk::ImageType::e2D);
	imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
	imageInfo.setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled);
	imageInfo.setSharingMode(vk::SharingMode::eExclusive);
	imageInfo.setFormat(Graphics::FormatToVkFormat[static_cast<size_t>(format)]);

	vma::AllocationCreateInfo allocInfo{};
	allocInfo.setUsage(vma::MemoryUsage::eAuto);

	auto result = Graphics::GetInstance().allocator_->createImageUnique(imageInfo, allocInfo);
	allocation_ = std::move(result.second);
	imageData_.image_ = std::move(result.first);

	imageData_.arraySize_ = arraySize;
	imageData_.mipmapLevels_ = mipLeveles;
	imageData_.pixelPerSize_ = FormatSize[static_cast<std::size_t>(format)];
	size_ = size;
}

//Eugene::VkImageResource::VkImageResource(const vma::Allocator& allocator, const TextureInfo& info) :
//	ImageResource{info.format }
//{
//	auto format{ VkGraphics::FormatToVkFormat[static_cast<size_t>(info.format)] };
//	vk::ImageCreateInfo imageInfo{};
//	imageInfo.setArrayLayers(info.arraySize);
//	imageInfo.setMipLevels(info.mipLevels);
//	imageInfo.extent.setHeight(info.height);
//	imageInfo.extent.setWidth(info.width);
//	imageInfo.extent.setDepth(1u);
//	imageInfo.setImageType(vk::ImageType::e2D);
//	imageInfo.setTiling(vk::ImageTiling::eOptimal);
//	imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
//	imageInfo.setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
//	imageInfo.setSharingMode(vk::SharingMode::eExclusive);
//	imageInfo.setSamples(vk::SampleCountFlagBits::e1);
//	imageInfo.setFormat(format);
//	vma::AllocationCreateInfo allocInfo{};
//	allocInfo.setUsage(vma::MemoryUsage::eAuto);
//
//	auto result = allocator.createImageUnique(imageInfo, allocInfo);
//	data_.allocation_ = std::move(result.second);
//	data_.image_ = std::move(result.first);
//
//	data_.arraySize_ = info.arraySize;
//	data_.mipmapLevels_ = info.mipLevels;
//	data_.pixelPerSize = info.pixelPerBite;
//
//	size_ = { static_cast<int>(info.width),static_cast<int>(info.height) };
//}
//
//Eugene::VkImageResource::VkImageResource(const vma::Allocator& allocator, const glm::ivec2& size, float clearValue,  std::uint8_t sampleCount) :
//	ImageResource{Format::D32_FLOAT}
//{
//	constexpr auto format{ vk::Format::eD32Sfloat };
//	vk::ImageCreateInfo imageInfo{};
//	imageInfo.setArrayLayers(1);
//	imageInfo.setMipLevels(1);
//	imageInfo.extent.setHeight(size.y);
//	imageInfo.extent.setWidth(size.x);
//	imageInfo.extent.setDepth(1u);
//	imageInfo.setTiling(vk::ImageTiling::eOptimal);
//	imageInfo.setImageType(vk::ImageType::e2D);
//	imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
//	imageInfo.setUsage(vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eDepthStencilAttachment);
//	imageInfo.setSharingMode(vk::SharingMode::eExclusive);
//	imageInfo.setSamples(static_cast<vk::SampleCountFlagBits>(sampleCount));
//	imageInfo.setFormat(format);
//
//	vma::AllocationCreateInfo allocInfo{};
//	allocInfo.setUsage(vma::MemoryUsage::eAuto);
//
//	auto result = allocator.createImageUnique(imageInfo, allocInfo);
//	data_.allocation_ = std::move(result.second);
//	data_.image_ = std::move(result.first);
//
//	data_.arraySize_ = 1;
//	data_.mipmapLevels_ = 1;
//	data_.pixelPerSize = 4u;
//	size_ = size;
//}
//
//Eugene::VkImageResource::VkImageResource(
//	const vma::Allocator& allocator, 
//	const glm::ivec2& size,
//	Format format,
//	std::uint32_t arraySize,
//	std::uint8_t mipLeveles,
//	std::uint8_t sampleCount,
//	std::optional<std::span<float, 4>> clearColor
//) :
//	ImageResource{format}
//{
//	vk::ImageCreateInfo imageInfo{};
//	imageInfo.setArrayLayers(arraySize);
//	imageInfo.setMipLevels(mipLeveles);
//	imageInfo.extent.setHeight(size.y);
//	imageInfo.extent.setWidth(size.x);
//	imageInfo.extent.setDepth(1u);
//	imageInfo.setSamples(static_cast<vk::SampleCountFlagBits>(sampleCount));
//	imageInfo.setTiling(vk::ImageTiling::eOptimal);
//	imageInfo.setImageType(vk::ImageType::e2D);
//	imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
//	imageInfo.setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled);
//	imageInfo.setSharingMode(vk::SharingMode::eExclusive);
//	imageInfo.setFormat(VkGraphics::FormatToVkFormat[static_cast<size_t>(format)]);
//
//	vma::AllocationCreateInfo allocInfo{};
//	allocInfo.setUsage(vma::MemoryUsage::eAuto);
//
//	auto result = allocator.createImageUnique(imageInfo, allocInfo);
//	data_.allocation_ = std::move(result.second);
//	data_.image_ = std::move(result.first);
//
//	data_.arraySize_ = arraySize;
//	data_.mipmapLevels_ = mipLeveles;
//	data_.pixelPerSize = FormatSize[static_cast<std::size_t>(format)];
//	size_ = size;
//}


Eugene::ImageResource::ImageResource(const glm::ivec2& size, Format format, vk::Image& image) :
	format_{format}
{
	format_ = Graphics::backBufferFormat_;
	size_ = size;
	imageData_.arraySize_ = 1;
	imageData_.mipmapLevels_ = 1;
	imageData_.image_ = vma::UniqueImage{image};
	isBackBuffer_ = true;
}

Eugene::ImageResource::~ImageResource()
{
	if (isBackBuffer_)
	{
		imageData_.image_.release();
	}
}
