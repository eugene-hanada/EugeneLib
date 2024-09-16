#include "../../../Include/Graphics/DirectX12/Dx12BufferResource.h"
#include "../../../Include/Graphics/DirectX12/Dx12Graphics.h"
#include <dxgi1_6.h>

#include "../../../Include/ThirdParty/d3dx12.h"
#include "../../../Include/Utils/EugeneLibException.h"
#include "../../../Include/Graphics/Image.h"
#include "../../../Include/Graphics/Graphics.h"

Eugene::BufferResource::BufferResource(std::uint64_t size, bool isUnordered, GpuResourceType type)
{
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	
	D3D12MA::ALLOCATION_DESC allocationDesc{};
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

	switch (type)
	{
	case Eugene::GpuResourceType::Default:
		allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		canMap_ = false;
		break;
	case Eugene::GpuResourceType::Upload:
		allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
		canMap_ = true;
		break;
	case Eugene::GpuResourceType::ReadBack:
		allocationDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
		state = D3D12_RESOURCE_STATE_COPY_DEST;
		canMap_ = false;
		break;
	default:
		break;
	}

	if (isUnordered)
	{
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	if (FAILED(Graphics::GetInstance().allocator_->CreateResource(
			&allocationDesc,
			&resourceDesc,
			state,
			nullptr,
			allocation_.ReleaseAndGetAddressOf(),
			IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
		)))
	{
		throw EugeneLibException("ID3D12Resource���f�t�H���g���\�[�X�Ő����Ɏ��s");
	}
}

Eugene::BufferResource::BufferResource( Image& image)
{
	auto subResource = std::min(std::max(static_cast<int>(image.GetInfo().mipLevels), 1) * static_cast<int>(image.GetInfo().arraySize), static_cast<int>(maxSubResource));
	canMap_ = false;
	// �A�b�v���[�h���desc�������Ă�����
	std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT, maxSubResource>  footprint;

	// �g�[�^���̃o�C�g��
	std::uint64_t totalSize;

	std::array<std::uint64_t, maxSubResource> rowSize;
	std::array<std::uint32_t, maxSubResource> numRaw;


	auto tmp = static_cast<DXGI_FORMAT>(Graphics::FormatToDxgiFormat_.at(static_cast<int>(image.GetInfo().format)));
	auto footDesc = CD3DX12_RESOURCE_DESC::Tex2D(tmp, image.GetInfo().width, image.GetInfo().height, image.GetInfo().arraySize, image.GetInfo().mipLevels);
	Graphics::GetInstance().device_->GetCopyableFootprints(&footDesc, 0, subResource, 0, footprint.data(), numRaw.data(), rowSize.data(), &totalSize);

	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);
	D3D12MA::ALLOCATION_DESC allocationDesc{};
	allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

	if (FAILED(Graphics::GetInstance().allocator_->CreateResource(
		&allocationDesc,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		allocation_.ReleaseAndGetAddressOf(),
		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf()))))
	{
		throw EugeneLibException("ID3D12Resource���A�b�v���[�h���\�[�X�ō쐬�Ɏ��s");
	}

	std::uint8_t* ptr{ nullptr };
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&ptr));

	// array��
	for (std::uint32_t j = 0ull; j < image.GetInfo().arraySize; j++)
	{
		// �~�b�v�}�b�v��
		for (std::uint32_t m = 0; m < image.GetInfo().mipLevels; m++)
		{
			// �摜�f�[�^�̐擪�|�C���^���擾����
			auto texPtr = image.GetData(j, m);

			// �C���f�b�N�X���v�Z
			auto idx = j * image.GetInfo().mipLevels + m;

			// raw���擾
			auto tmpNumRaw = numRaw[idx];
			for (std::uint32_t i = 0u; i < tmpNumRaw; i++)
			{
				// �R�s�[
				std::copy_n(texPtr, rowSize[idx], ptr);
				ptr += footprint[idx].Footprint.RowPitch;
				texPtr += rowSize[idx];
			}
		}
	}
	resource_->Unmap(0, nullptr);
}

//Eugene::Dx12UnloadableBufferResource::Dx12UnloadableBufferResource(ID3D12Device* device, D3D12MA::Allocator* allocator, Image& image)
//{
//	auto subResource = std::min(std::max(static_cast<int>(image.GetInfo().mipLevels) , 1) * static_cast<int>(image.GetInfo().arraySize), static_cast<int>(maxSubResource));
//
//	 �A�b�v���[�h���desc�������Ă�����
//	std::array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT,maxSubResource>  footprint;
//	
//	 �g�[�^���̃o�C�g��
//	std::uint64_t totalSize;
//
//	std::array<std::uint64_t,maxSubResource> rowSize;	
//	std::array<std::uint32_t,maxSubResource> numRaw;
//	
//
//	auto tmp = static_cast<DXGI_FORMAT>(Graphics::FormatToDxgiFormat_.at(static_cast<int>(image.GetInfo().format)));
//	auto footDesc = CD3DX12_RESOURCE_DESC::Tex2D(tmp, image.GetInfo().width, image.GetInfo().height, image.GetInfo().arraySize, image.GetInfo().mipLevels);
//	device->GetCopyableFootprints(&footDesc, 0, subResource, 0, footprint.data(), numRaw.data(), rowSize.data(), &totalSize);
//
//	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);
//	D3D12MA::ALLOCATION_DESC allocationDesc{};
//	allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
//	
//	if (FAILED(allocator->CreateResource(
//		&allocationDesc,
//		&resourceDesc,
//		D3D12_RESOURCE_STATE_COMMON,
//		nullptr,
//		allocation_.ReleaseAndGetAddressOf(),
//		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf()))))
//	{
//		throw EugeneLibException("ID3D12Resource���A�b�v���[�h���\�[�X�ō쐬�Ɏ��s");
//	}
//
//	std::uint8_t* ptr{ nullptr };
//	resource_->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
//	
//	 array��
//	for (std::uint32_t j = 0ull; j < image.GetInfo().arraySize; j++)
//	{
//		 �~�b�v�}�b�v��
//		for (std::uint32_t m = 0; m < image.GetInfo().mipLevels; m++)
//		{
//			 �摜�f�[�^�̐擪�|�C���^���擾����
//			auto texPtr = image.GetData(j,m);
//
//			 �C���f�b�N�X���v�Z
//			auto idx = j * image.GetInfo().mipLevels + m;
//
//			 raw���擾
//			auto tmpNumRaw = numRaw[idx];
//			for (std::uint32_t i = 0u; i < tmpNumRaw; i++)
//			{
//				 �R�s�[
//				std::copy_n(texPtr, rowSize[idx], ptr);
//				ptr += footprint[idx].Footprint.RowPitch;
//				texPtr += rowSize[idx];
//			}
//		}
//	}
//	resource_->Unmap(0, nullptr);
//}
//
//Eugene::Dx12UnloadableBufferResource::Dx12UnloadableBufferResource(D3D12MA::Allocator* allocator, std::uint64_t size) :
//	BufferResource{}
//{
//	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
//
//	D3D12MA::ALLOCATION_DESC allocationDesc{};
//	allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
//
//
//	if (FAILED(allocator->CreateResource(
//		&allocationDesc,
//		&resourceDesc,
//		D3D12_RESOURCE_STATE_COMMON,
//		nullptr,
//		allocation_.ReleaseAndGetAddressOf(),
//		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
//	)))
//	{
//		throw EugeneLibException("ID3D12Resource���A�b�v���[�h���\�[�X�ō쐬�Ɏ��s");
//	}
//}
//
//Eugene::Dx12UnloadableBufferResource::~Dx12UnloadableBufferResource()
//{
//}
//
//void* Eugene::Dx12UnloadableBufferResource::Map(void)
//{
//	void* ptr{ nullptr };
//	resource_->Map(0,nullptr, &ptr);
//    return ptr;
//}
//
//void Eugene::Dx12UnloadableBufferResource::UnMap(void)
//{
//	resource_->Unmap(0, nullptr);
//}
//
//bool Eugene::Dx12UnloadableBufferResource::CanMap(void) const
//{
//	return true;
//}
//
//void* Eugene::Dx12UnloadableBufferResource::GetResource(void) 
//{
//	return resource_.Get();
//}
//
//std::uint64_t Eugene::Dx12UnloadableBufferResource::GetSize(void)
//{
//	return resource_->GetDesc().Width;
//}
//
//Eugene::Dx12ReadableBuffeResource::Dx12ReadableBuffeResource(D3D12MA::Allocator* allocator, std::uint64_t size, bool isUnordered)
//{
//	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
//	D3D12MA::ALLOCATION_DESC allocationDesc{};
//	allocationDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
//	if (FAILED(allocator->CreateResource(
//		&allocationDesc,
//		&resourceDesc,
//		D3D12_RESOURCE_STATE_COPY_DEST,
//		nullptr,
//		allocation_.ReleaseAndGetAddressOf(),
//		IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())
//	)))
//	{
//		throw EugeneLibException("ID3D12Resource��ǂݎ��p���\�[�X�Ő����Ɏ��s");
//	}
//}
//
//Eugene::Dx12ReadableBuffeResource::~Dx12ReadableBuffeResource()
//{
//}
//
//void* Eugene::Dx12ReadableBuffeResource::Map(void)
//{
//	void* ptr{ nullptr };
//	resource_->Map(0, nullptr, &ptr);
//	return ptr;
//}
//
//void Eugene::Dx12ReadableBuffeResource::UnMap(void)
//{
//	resource_->Unmap(0, nullptr);
//}
//
//bool Eugene::Dx12ReadableBuffeResource::CanMap(void) const
//{
//	return true;
//}
//
//void* Eugene::Dx12ReadableBuffeResource::GetResource(void)
//{
//	return resource_.Get();
//}
//
//std::uint64_t Eugene::Dx12ReadableBuffeResource::GetSize(void)
//{
//	return resource_->GetDesc().Width;
//}
