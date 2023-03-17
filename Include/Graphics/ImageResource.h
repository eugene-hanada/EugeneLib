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
		virtual Format GetFormat(void) const = 0;

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
