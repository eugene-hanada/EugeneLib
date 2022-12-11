#include "../../Include/Graphics/ShaderResourceViews.h"
#include "DirectX12/Dx12ShaderResourceViews.h"

EugeneLib::ShaderResourceViews::~ShaderResourceViews()
{
}

const size_t EugeneLib::ShaderResourceViews::GetSize(void)
{
	return  size_;
}

EugeneLib::ShaderResourceViews::ShaderResourceViews(size_t size) :
	size_{size}
{
}

EugeneLib::ShaderResourceViews* EugeneLib::CreateShaderResourceViews(Graphics& graphics, size_t size)
{
	return new Dx12ShaderResourceViews{graphics,size};
}
