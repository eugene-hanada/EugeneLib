#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <utility>

namespace Eugene
{
	class ImageResource;
	class BufferResource;

	/// <summary>
	/// シェーダリソースビュー
	/// </summary>
	class  ShaderResourceViews
	{
	public:
		ShaderResourceViews() noexcept :
			size_{0}
		{
		}

		/// <summary>
		/// テクスチャ用のビューを生成する
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		void CreateTexture(ImageResource& resource, std::uint32_t idx);

		/// <summary>
		/// 定数バッファ用のビューを生成する
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		void CreateConstantBuffer(BufferResource& resource, std::uint32_t idx);

		/// <summary>
		/// キューブマップ用のビューを生成する
		/// </summary>
		/// <param name="resource"></param>
		/// <param name="idx"></param>
		void CreateCubeMap(ImageResource& resource, std::uint32_t idx) ;

		/// <summary>
		/// Unorderedで使用するバッファ用のビューを生成する
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		/// <param name="numElements"> 要素の数 </param>
		/// <param name="strideSize"> 要素一つ当たりのサイズ </param>
		void CreateUnorderedAccessBuffer(BufferResource& resource, std::uint32_t idx, std::uint32_t numElements, std::uint64_t strideSize);

		/// <summary>
		/// API側のビューを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		void* GetViews(void)
		{
			return descriptorHeap_.Get();
		}

		/// <summary>
		/// サイズ(保持できるビューの数)
		/// </summary>
		/// <returns></returns>
		std::uint32_t GetSize() const noexcept
		{
			return size_;
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final()noexcept
		{
			size_ = 0;
			descriptorHeap_.Reset();
		}


		ShaderResourceViews(ShaderResourceViews&& views) noexcept :
			descriptorHeap_{std::move(views.descriptorHeap_)}, size_{views.size_}
		{
			views.size_ = 0;
		}

		ShaderResourceViews& operator=(ShaderResourceViews&& views) noexcept
		{
			descriptorHeap_ = std::move(views.descriptorHeap_);
			size_ = views.size_;
			views.size_ = 0;
		}

		ShaderResourceViews(const ShaderResourceViews& views)
		{
			*this = views;
		}

		ShaderResourceViews& operator=(const ShaderResourceViews& views);
	protected:
		ShaderResourceViews(std::uint32_t size)
		{
			Init(size);
		}
		void Init(std::uint32_t size);
		
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

		std::uint32_t size_;

		friend class Graphics;
	};
}
