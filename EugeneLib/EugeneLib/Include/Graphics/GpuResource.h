#pragma once
#include <cstdint>

namespace EugeneLib
{
	class GpuResource
	{
	public:
		virtual ~GpuResource();
		virtual void* Map(void);
		virtual void UnMap(void);
		virtual void* GetResource(void) const= 0;
	protected:
		GpuResource();
	};

}
