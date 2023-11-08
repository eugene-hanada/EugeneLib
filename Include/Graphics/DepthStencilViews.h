#pragma once
#include "GraphicsCommon.h"

namespace Eugene
{
	class Graphics;
	class ImageResource;

	/// <summary>
	/// デブスステンシル用ビュー
	/// </summary>
	class DepthStencilViews
	{
	public:
		virtual ~DepthStencilViews();

		/// <summary>
		/// ビューを生成する
		/// </summary>
		/// <param name="resource"></param>
		/// <param name="idx"></param>
		virtual void Create(ImageResource& resource, std::uint64_t idx) = 0;
		virtual void* GetViews(void) = 0;
	protected:
		DepthStencilViews(std::uint64_t size);
		std::uint64_t size_;
	};
}