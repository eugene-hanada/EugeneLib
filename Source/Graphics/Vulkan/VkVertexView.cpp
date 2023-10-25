#include "VkVertexView.h"

Eugene::VkVertexView::VkVertexView(size_t size, size_t vertexNum, vk::Buffer& buffer) :
	VertexView{static_cast<std::uint32_t>(vertexNum)}
{
	buffer_ = buffer;
}

void* Eugene::VkVertexView::GetView(void) 
{
	return reinterpret_cast<void*>(&buffer_);
}
