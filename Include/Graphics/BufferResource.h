#pragma once
#include "GpuResource.h"

namespace Eugene
{
	class BufferResource :
		public GpuResource
	{
	public:
		virtual ~BufferResource();
		virtual std::uint64_t GetSize(void) = 0;
	protected:
		BufferResource();
	private:

	};

}