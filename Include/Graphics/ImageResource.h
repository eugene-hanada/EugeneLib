#pragma once
#include "GpuResource.h"
#include "GraphicsCommon.h"
#include "../Math/Vector2.h"

namespace Eugene
{
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
		virtual Vector2I GetSize(void) = 0;
	protected:
		ImageResource(Format format);

		/// <summary>
		/// フォーマット
		/// </summary>
		Format format_;
	private:
	};
}
