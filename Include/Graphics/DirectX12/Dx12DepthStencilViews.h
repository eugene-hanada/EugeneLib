#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <utility>

namespace Eugene
{
	class ImageResource;

	/// <summary>
	/// デプスステンシル用のビュー
	/// </summary>
	class DepthStencilViews
	{
	public:

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		DepthStencilViews() noexcept :
			size_{0}, isShaderVisible_{false}
		{
		}

		/// <summary>
		/// ビューを生成する
		/// </summary>
		/// <param name="resource"> 対象のリソース </param>
		/// <param name="idx"> インデックス </param>
		void Create(ImageResource& resource, std::uint32_t idx);
		void* GetViews(void)
		{
			return descriptorHeap_.Get();
		}

		/// <summary>
		/// コピーコンストラクタ
		/// </summary>
		DepthStencilViews(const DepthStencilViews& views);
		
		/// <summary>
		/// コピー演算子
		/// </summary>
		DepthStencilViews& operator=(const DepthStencilViews& views);

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		DepthStencilViews(DepthStencilViews&& views) noexcept :
			descriptorHeap_{ std::move(views.descriptorHeap_) }, size_{ views.size_ }, isShaderVisible_{views.isShaderVisible_}
		{
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="views"></param>
		/// <returns></returns>
		DepthStencilViews& operator=(DepthStencilViews&& views) noexcept
		{
			descriptorHeap_ = std::move(views.descriptorHeap_);
			size_ = views.size_;
			isShaderVisible_ = views.isShaderVisible_;
			views.size_ = 0;
			return *this;
		}

		/// <summary>
		/// ビューの数を取得する(保持叶可能な数)
		/// </summary>
		/// <returns></returns>
		std::uint32_t GetSize() const
		{
			return size_;
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final() noexcept
		{
			descriptorHeap_.Reset();
			size_ = 0;
		}

	private:

		void Init(std::uint32_t size, bool isShaderVisible);

		DepthStencilViews(std::uint32_t size, bool isShaderVisible)
		{
			Init(size, isShaderVisible);
		}

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

		std::uint32_t size_;

		bool isShaderVisible_;

		friend class Graphics;
	};
}
