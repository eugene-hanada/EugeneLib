#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <utility>

namespace Eugene
{
	class Sampler;

	/// <summary>
	/// �T���v���[�r���[
	/// </summary>
	class SamplerViews
	{
	public:
		SamplerViews() noexcept : size_{ 0 }
		{
		}

		/// <summary>
		/// �r���[�𐶐�����
		/// </summary>
		/// <param name="sampler"> �T���v���[ </param>
		/// <param name="idx"> �C���f�b�N�X </param>
		void CreateSampler(Sampler& sampler, std::uint32_t idx);

		/// <summary>
		/// API���̃r���[���擾����
		/// </summary>
		/// <returns> �r���[�̃|�C���^ </returns>
		void* GetViews(void) noexcept
		{
			return descriptorHeap_.Get();
		}

		/// <summary>
		/// �T�C�Y���擾(�ێ��ł���r���[�̐�)
		/// </summary>
		/// <returns></returns>
		std::uint32_t GetSize() const noexcept
		{
			return size_;
		}

		/// <summary>
		/// �I������
		/// </summary>
		void Final() noexcept
		{
			descriptorHeap_.Reset();
		}

		/// <summary>
		/// ���[�u�R���X�g���N�^
		/// </summary>
		/// <param name="samplerViews"></param>
		SamplerViews(SamplerViews&& samplerViews) noexcept :
			descriptorHeap_{std::move(samplerViews.descriptorHeap_)},size_{samplerViews.size_}
		{
			size_ = 0;
		}

		/// <summary>
		/// ���[�u���Z�q
		/// </summary>
		/// <param name="samplerViews"></param>
		/// <returns></returns>
		SamplerViews& operator=(SamplerViews&& samplerViews) noexcept
		{
			descriptorHeap_ = std::move(samplerViews.descriptorHeap_);
			size_ = samplerViews.size_;
			samplerViews.size_ = 0;
		}
	private:
		SamplerViews(std::uint32_t size)
		{
			Init(size);
		}

		void Init(std::uint32_t size);

		SamplerViews(const SamplerViews& ) = delete;
		SamplerViews& operator=(const SamplerViews&) = delete;

		std::uint32_t size_;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

		friend class Graphics;
	};
}
