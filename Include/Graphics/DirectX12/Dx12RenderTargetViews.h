#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <utility>

namespace Eugene
{
	class ImageResource;

	/// <summary>
	/// レンダーターゲット用のビュー
	/// </summary>
	class RenderTargetViews
	{
	public:
		RenderTargetViews() noexcept :
			size_{0}, isShaderVisible_{false}
		{
		}

		/// <summary>
		/// ビューを生成する
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		void Create(ImageResource& resource, std::uint32_t idx);

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
		/// ビューの数(保持できる数)
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

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="views"></param>
		RenderTargetViews(RenderTargetViews&& views) noexcept :
			descriptorHeap_{std::move(views.descriptorHeap_)}, size_{std::move(views.size_)}, isShaderVisible_{views.isShaderVisible_}
		{
			views.size_ = 0;
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="views"></param>
		/// <returns></returns>
		RenderTargetViews& operator=(RenderTargetViews&& views) noexcept
		{
			descriptorHeap_ = std::move(views.descriptorHeap_);
			size_ = views.size_;
			views.size_ = 0;
			isShaderVisible_ = views.isShaderVisible_;
		}

		/// <summary>
		/// コピーコンストラクタ
		/// </summary>
		/// <param name="views"></param>
		RenderTargetViews(const RenderTargetViews& views) :
			isShaderVisible_{ views.isShaderVisible_ }
		{
			*this = views;
		}

		/// <summary>
		///コピー演算子
		/// </summary>
		/// <param name="views"></param>
		/// <returns></returns>
		RenderTargetViews& operator=(const RenderTargetViews& views);
	private:

		void Init(std::uint32_t size, bool isShaderVisible);

		RenderTargetViews(std::uint32_t size, bool isShaderVisible)
		{
			Init(size, isShaderVisible);
		}

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

		std::uint32_t size_;

		bool isShaderVisible_;

		friend class Graphics;
	};
}
