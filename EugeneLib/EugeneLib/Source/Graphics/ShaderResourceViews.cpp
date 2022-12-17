#include "../../Include/Graphics/ShaderResourceViews.h"

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
