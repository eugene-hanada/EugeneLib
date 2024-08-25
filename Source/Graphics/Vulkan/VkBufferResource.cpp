#include "../../../Include/Graphics/Vulkan/VkBufferResource.h"
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"
#include "../../../Include/Graphics/Image.h"


Eugene::BufferResource::BufferResource(std::uint64_t size, bool isUnordered, GpuResourceType type) :
	mapCount_{0}, canMap_{false}
{
	size_ = size;
	vk::BufferCreateInfo bufferInfo{};
	auto usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer;
	bufferInfo.setSize(size);
	if (isUnordered)
	{
		usage |= vk::BufferUsageFlagBits::eStorageBuffer;
	}
	
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

	vma::AllocationCreateInfo allocInfo{};
	allocInfo.setUsage(vma::MemoryUsage::eAuto);
	vma::AllocationCreateFlags flags;

	switch (type)
	{
	case Eugene::GpuResourceType::Default:
		usage |= vk::BufferUsageFlagBits::eTransferDst;
		break;
	case Eugene::GpuResourceType::Upload:
		usage |= vk::BufferUsageFlagBits::eTransferSrc;
		flags = vma::AllocationCreateFlagBits::eHostAccessRandom | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead;
		allocInfo.setPreferredFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		canMap_ = true;
		break;
	case Eugene::GpuResourceType::ReadBack:
		usage |= vk::BufferUsageFlagBits::eTransferDst;
		flags = vma::AllocationCreateFlagBits::eHostAccessRandom | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead;
		allocInfo.setPreferredFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		canMap_ = true;
		break;
	default:
		break;
	}

	bufferInfo.setUsage(usage);
	allocInfo.setFlags(flags);

	// メモリ確保とバッファの生成を行う
	auto result = Graphics::GetInstance().allocator_->createBufferUnique(bufferInfo, allocInfo);
	allocation_ = std::move(result.second);
	buffer_ = std::move(result.first);

}

Eugene::BufferResource::BufferResource(Image& image) :
	mapCount_{0}, canMap_{true}
{
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.setSize(image.GetInfo().totalSize_);
	bufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

	vma::AllocationCreateInfo allocInfo{};

	// Autoで確保する
	allocInfo.setUsage(vma::MemoryUsage::eAuto);

	// CPU側からのアクセスができるようにフラグをセットしシーケンシャルに書き込めるようにする
	allocInfo.setFlags(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);

	// CPU側から見えるように設定
	allocInfo.setPreferredFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	// メモリ確保とバッファの生成を行う
	auto result = Graphics::GetInstance().allocator_->createBufferUnique(bufferInfo, allocInfo);
	allocation_ = std::move(result.second);
	buffer_ = std::move(result.first);

	// サイズを保存
	size_ = image.GetInfo().totalSize_;

	auto ptr = static_cast<std::uint8_t*>(Map());

	std::memcpy(ptr, image.GetData(), image.GetInfo().totalSize_);
	UnMap();
}

void Eugene::BufferResource::AllUnMap()
{
	// Mapした回数分Unmapする
	for (std::uint64_t i = 0ull; i < mapCount_; i++)
	{
		Graphics::GetInstance().allocator_->unmapMemory(*allocation_);
	}
	mapCount_ = 0;
}

void* Eugene::BufferResource::Map(void) noexcept
{
	mapCount_++;
	return Graphics::GetInstance().allocator_->mapMemory(*allocation_);
}

void Eugene::BufferResource::UnMap(void) noexcept
{
	mapCount_--;
	Graphics::GetInstance().allocator_->unmapMemory(*allocation_);
}

