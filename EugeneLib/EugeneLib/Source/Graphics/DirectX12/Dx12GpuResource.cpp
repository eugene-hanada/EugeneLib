#include "Dx12GpuResource.h"
#include <d3d12.h>

EugeneLib::Dx12GpuResource::~Dx12GpuResource()
{
}

void* EugeneLib::Dx12GpuResource::GetResource(void) const
{
	return resource_.Get();
}
