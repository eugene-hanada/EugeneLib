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
		virtual Format GetFormat(void) const = 0;
		virtual Vector2I GetSize(void) = 0;
	protected:
		ImageResource();
	private:
	};
}
