#include "../../Include/Graphics/ShaderResourceViews.h"

Eugene::ShaderResourceViews::~ShaderResourceViews()
{
}

const size_t Eugene::ShaderResourceViews::GetSize(void)
{
	return  size_;
}

Eugene::ShaderResourceViews::ShaderResourceViews(size_t size) :
	size_{size}
{
}
