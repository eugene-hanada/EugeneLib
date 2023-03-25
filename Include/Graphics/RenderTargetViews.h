#pragma once
#include <cstdint>
#include "GraphicsCommon.h"

namespace Eugene
{
	class Graphics;
	class ImageResource;

	/// <summary>
	/// レンダーターゲット用のビュー
	/// </summary>
	class RenderTargetViews
	{
	public:
		virtual ~RenderTargetViews();

		/// <summary>
		/// レンダーターゲット用ビューの作成
		/// </summary>
		/// <param name="resource"> リソース </param>
		/// <param name="idx"> インデックス </param>
		/// <param name="format"> フォーマット </param>
		virtual void Create(ImageResource& resource,std::uint64_t idx, const Format& format) = 0;
		virtual void* GetViews(void) const = 0;

		/// <summary>
		/// サイズの取得
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::uint64_t GetSize(void);
	protected:
		RenderTargetViews(std::uint64_t size);
		std::uint64_t size_;
	};

}