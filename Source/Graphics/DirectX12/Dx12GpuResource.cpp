#include "Dx12GpuResource.h"
#include <d3d12.h>

Eugene::Dx12GpuResource::~Dx12GpuResource()
{
}

void* Eugene::Dx12GpuResource::GetResource(void) const
{
	return resource_.Get();
}
