#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "../../ThirdParty/D3D12MemoryAllocator/include/D3D12MemAlloc.h"
#include "../../../ThirdParty/glm/glm/vec2.hpp"
#include "../GraphicsCommon.h"


namespace D3D12MA
{
	struct Allocation;
	struct Allocator;
}


namespace Eugene
{
	class Image;

	/// <summary>
	/// �O���t�B�b�N�XAPI�p�̃o�b�t�@���\�[�X
	/// </summary>
	class BufferResource
	{
	public:
		BufferResource() noexcept = default;
		BufferResource(std::uint64_t size, bool isUnordered, GpuResourceType type);
		BufferResource(Image& image);
		~BufferResource() = default;

		/// <summary>
		/// �}�b�v�\���H
		/// </summary>
		/// <returns> �}�b�v�\�ȏꍇtrue�s�\�ȏꍇfalse </returns>
		bool CanMap(void) const noexcept
		{
			return canMap_;
		}
		
		/// <summary>
		/// API���̃��\�[�X�̎擾
		/// </summary>
		/// <returns> ���\�[�X�̃|�C���^ </returns>
		void* GetResource(void) noexcept
		{
			return resource_.Get();
		}

		/// <summary>
		/// �T�C�Y���擾����(�o�C�g��)
		/// </summary>
		/// <returns> �T�C�Y(�o�C�g��) </returns>
		std::uint64_t GetSize(void) const
		{
			return resource_->GetDesc().Width;
		}

		/// <summary>
		/// �I������
		/// </summary>
		void Final()noexcept
		{
			resource_.Reset();
			allocation_.Reset();
		}

		/// <summary>
		/// ���[�u�R���X�g���N�^
		/// </summary>
		/// <param name="bufferResource"></param>
		BufferResource(BufferResource&& bufferResource) noexcept :
			resource_{std::move(bufferResource.resource_)}, allocation_{std::move(bufferResource.allocation_)}, canMap_{bufferResource.canMap_}
		{
		}

		/// <summary>
		/// ���[�u���Z�q
		/// </summary>
		/// <param name="bufferResource"></param>
		/// <returns></returns>
		BufferResource& operator=(BufferResource&& bufferResource) noexcept
		{
			resource_ = std::move(bufferResource.resource_);
			allocation_ = std::move(bufferResource.allocation_);
			canMap_ = bufferResource.canMap_;
		}

		/// <summary>
		/// �}�b�v����
		/// </summary>
		/// <returns> �|�C���^ </returns>
		void* Map() noexcept
		{
			void* ptr{ nullptr };
			resource_->Map(0, nullptr, &ptr);
			return ptr;
		}

		/// <summary>
		/// �A���}�b�v����
		/// </summary>
		void UnMap() noexcept
		{
			resource_->Unmap(0, nullptr);
		}
	private:
		BufferResource(const BufferResource&) = delete;
		BufferResource& operator=(const BufferResource&) = delete;

		/// <summary>
		/// �A���P�[�^�[�Ŋm�ۂ���������
		/// </summary>
		Microsoft::WRL::ComPtr<D3D12MA::Allocation> allocation_;

		/// <summary>
		/// ���\�[�X
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

		/// <summary>
		/// �}�b�v�\���H
		/// </summary>
		bool canMap_;

	};
}
