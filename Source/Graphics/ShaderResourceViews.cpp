#include "../../Include/Graphics/ShaderResourceViews.h"

Eugene::ShaderResourceViews::~ShaderResourceViews()
{
}

const std::uint64_t Eugene::ShaderResourceViews::GetSize(void)
{
	return  size_;
}

Eugene::ShaderResourceViews::ShaderResourceViews(std::uint64_t size) :
	size_{size}
{
}
