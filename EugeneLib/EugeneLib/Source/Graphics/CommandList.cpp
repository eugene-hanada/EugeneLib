#include "../../Include/Graphics/CommandList.h"
#include "DirectX12/Dx12CommandList.h"

EugeneLib::CommandList::~CommandList()
{
}

EugeneLib::CommandList* EugeneLib::CreateCommandList(Graphics& graphics)
{
	return new Dx12CommandList{ graphics };
}