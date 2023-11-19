#pragma once
#include "GpuResource.h"
#include "GraphicsCommon.h"
#include "../ThirdParty/glm/glm/vec2.hpp"

namespace Eugene
{
	/// <summary>
	/// テクスチャやレンダーターゲット等に使うリソース
	/// </summary>
	class ImageResource :
		public GpuResource
	{
	public:
		virtual ~ImageResource();

		/// <summary>
		/// フォーマットの取得
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		Format GetFormat(void) const { return format_; }

		/// <summary>
		/// 
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual glm::ivec2 GetSize(void) = 0;
	protected:
		ImageResource(Format format);

		/// <summary>
		/// フォーマット
		/// </summary>
		Format format_;
	private:
	};
}
