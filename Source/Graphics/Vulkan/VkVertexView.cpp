#include "VkVertexView.h"

Eugene::VkVertexView::VkVertexView(size_t size, size_t vertexNum, vk::Buffer& buffer) :
	VertexView{vertexNum}
{
	buffer_ = buffer;
}

void* Eugene::VkVertexView::GetView(void) 
{
	return reinterpret_cast<void*>(&buffer_);
}
