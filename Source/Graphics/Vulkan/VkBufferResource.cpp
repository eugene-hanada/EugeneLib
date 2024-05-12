#include "VkBufferResource.h"
#include "VkGraphics.h"
#include "../../../Include/Graphics/Image.h"

Eugene::VkBufferResource::VkBufferResource(const vma::Allocator& allocator, std::uint64_t size) :
	BufferResource{}
{
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.setSize(size);
	bufferInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);
	
	vma::AllocationCreateInfo allocInfo{};
	
	// Autoで確保するようにする
	allocInfo.setUsage(vma::MemoryUsage::eAuto);

	// メモリ確保とバッファの生成を行う
	auto result = allocator.createBufferUnique(bufferInfo, allocInfo);
	data_.allocation_ = std::move(result.second);
	data_.buffer_ = std::move(result.first);

	// サイズを知るすべがないので保存しとく
	data_.size_ = size;
}

bool Eugene::VkBufferResource::CanMap(void) const
{
	return false;
}

void* Eugene::VkBufferResource::GetResource(void)
{
	return &data_;
}

std::uint64_t Eugene::VkBufferResource::GetSize(void)
{
	return data_.size_;
}

Eugene::VkUploadableBufferResource::VkUploadableBufferResource(const vma::Allocator& allocator, std::uint64_t size) :
	BufferResource{},allocator_{allocator}, mapCount_{0ull}
{
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.setSize(size);
	bufferInfo.setUsage(
		vk::BufferUsageFlagBits::eUniformBuffer | 
		vk::BufferUsageFlagBits::eTransferSrc | 
		vk::BufferUsageFlagBits::eTransferDst | 
		vk::BufferUsageFlagBits::eVertexBuffer
	);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

	vma::AllocationCreateInfo allocInfo{};

	// Autoで確保する
	allocInfo.setUsage(vma::MemoryUsage::eAuto);

	// CPU側からのアクセスができるようにフラグをセット(主にバッファ向けの設定)
	allocInfo.setFlags(vma::AllocationCreateFlagBits::eHostAccessRandom | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead);
	
	// CPU側から見えるように設定
	allocInfo.setPreferredFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	// メモリ確保とバッファの生成を行う
	auto result = allocator_.createBufferUnique(bufferInfo, allocInfo);
	data_.allocation_ = std::move(result.second);
	data_.buffer_ = std::move(result.first);

	// サイズを知るすべがないので保存しとく
	data_.size_ = size;
}

Eugene::VkUploadableBufferResource::VkUploadableBufferResource(const vma::Allocator& allocator, Image& image):
	BufferResource{}, allocator_{ allocator }, mapCount_{ 0ull }
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
	auto result = allocator_.createBufferUnique(bufferInfo, allocInfo);
	data_.allocation_ = std::move(result.second);
	data_.buffer_ = std::move(result.first);

	// サイズを保存
	data_.size_ = image.GetInfo().totalSize_;

	auto ptr = static_cast<std::uint8_t*>(Map());

	std::memcpy(ptr, image.GetData(), image.GetInfo().totalSize_);
	UnMap();
}

Eugene::VkUploadableBufferResource::~VkUploadableBufferResource()
{
	// Mapした回数分Unmapする
	for (std::uint64_t i = 0ull; i < mapCount_; i++)
	{
		allocator_.unmapMemory(*data_.allocation_);
	}
}

bool Eugene::VkUploadableBufferResource::CanMap(void) const
{
	return true;
}

void* Eugene::VkUploadableBufferResource::GetResource(void)
{
	return &data_;
}

std::uint64_t Eugene::VkUploadableBufferResource::GetSize(void)
{
	return data_.size_;
}

void* Eugene::VkUploadableBufferResource::Map(void)
{
	mapCount_++;
	return allocator_.mapMemory(*data_.allocation_);
}

void Eugene::VkUploadableBufferResource::UnMap(void)
{
	mapCount_--;
	allocator_.unmapMemory(*data_.allocation_);
}
