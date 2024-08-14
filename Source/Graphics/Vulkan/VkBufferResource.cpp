#include "../../../Include/Graphics/Vulkan/VkBufferResource.h"
#include "../../../Include/Graphics/Vulkan/VkGraphics.h"
#include "../../../Include/Graphics/Image.h"


void Eugene::BufferResource::Init(std::uint64_t size, bool isUnordered, GpuResourceType type)
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
		break;
	case Eugene::GpuResourceType::ReadBack:
		usage |= vk::BufferUsageFlagBits::eTransferDst;
		flags = vma::AllocationCreateFlagBits::eHostAccessRandom | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead;
		allocInfo.setPreferredFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
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

void Eugene::BufferResource::Init(Image& image)
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

//Eugene::BufferResource::BufferResource(const vma::Allocator& allocator, std::uint64_t size, bool isUnordered) :
//	BufferResource{}
//{
//	vk::BufferCreateInfo bufferInfo{};
//	bufferInfo.setSize(size);
//	auto usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
//	if (isUnordered)
//	{
//		usage |= vk::BufferUsageFlagBits::eStorageBuffer;
//	}
//	bufferInfo.setUsage(usage);
//	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);
//	
//	vma::AllocationCreateInfo allocInfo{};
//	
//	// Autoで確保するようにする
//	allocInfo.setUsage(vma::MemoryUsage::eAuto);
//
//	// メモリ確保とバッファの生成を行う
//	auto result = allocator.createBufferUnique(bufferInfo, allocInfo);
//	data_.allocation_ = std::move(result.second);
//	data_.buffer_ = std::move(result.first);
//
//	// サイズを知るすべがないので保存しとく
//	data_.size_ = size;
//}
//
//
//
//Eugene::BufferResource::BufferResource(const vma::Allocator& allocator, std::uint64_t size, bool isUnordered) :
//	BufferResource{},allocator_{allocator}, mapCount_{0ull}
//{
//	vk::BufferCreateInfo bufferInfo{};
//
//	auto usage{ vk::BufferUsageFlagBits::eUniformBuffer |
//		vk::BufferUsageFlagBits::eTransferSrc |
//		vk::BufferUsageFlagBits::eTransferDst |
//		vk::BufferUsageFlagBits::eVertexBuffer };
//
//	if (isUnordered)
//	{
//		usage |= vk::BufferUsageFlagBits::eStorageBuffer;
//	}
//
//	bufferInfo.setSize(size);
//	bufferInfo.setUsage(
//		usage
//	);
//	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);
//
//	vma::AllocationCreateInfo allocInfo{};
//
//	// Autoで確保する
//	allocInfo.setUsage(vma::MemoryUsage::eAuto);
//
//	// CPU側からのアクセスができるようにフラグをセット(主にバッファ向けの設定)
//	allocInfo.setFlags(vma::AllocationCreateFlagBits::eHostAccessRandom | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead);
//	
//	// CPU側から見えるように設定
//	allocInfo.setPreferredFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
//
//	// メモリ確保とバッファの生成を行う
//	auto result = allocator_.createBufferUnique(bufferInfo, allocInfo);
//	data_.allocation_ = std::move(result.second);
//	data_.buffer_ = std::move(result.first);
//
//	// サイズを知るすべがないので保存しとく
//	data_.size_ = size;
//}
//
//Eugene::BufferResource::BufferResource(const vma::Allocator& allocator, Image& image):
//	BufferResource{}, allocator_{ allocator }, mapCount_{ 0ull }
//{
//	vk::BufferCreateInfo bufferInfo{};
//	bufferInfo.setSize(image.GetInfo().totalSize_);
//	bufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer);
//	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);
//	
//	vma::AllocationCreateInfo allocInfo{};
//
//	// Autoで確保する
//	allocInfo.setUsage(vma::MemoryUsage::eAuto);
//	
//	// CPU側からのアクセスができるようにフラグをセットしシーケンシャルに書き込めるようにする
//	allocInfo.setFlags(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);
//
//	// CPU側から見えるように設定
//	allocInfo.setPreferredFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
//
//	// メモリ確保とバッファの生成を行う
//	auto result = allocator_.createBufferUnique(bufferInfo, allocInfo);
//	data_.allocation_ = std::move(result.second);
//	data_.buffer_ = std::move(result.first);
//
//	// サイズを保存
//	data_.size_ = image.GetInfo().totalSize_;
//
//	auto ptr = static_cast<std::uint8_t*>(Map());
//
//	std::memcpy(ptr, image.GetData(), image.GetInfo().totalSize_);
//	UnMap();
//}

void Eugene::BufferResource::AllUnMap()
{
	// Mapした回数分Unmapする
	for (std::uint64_t i = 0ull; i < mapCount_; i++)
	{
		Graphics::GetInstance().allocator_->unmapMemory(*allocation_);
	}
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

