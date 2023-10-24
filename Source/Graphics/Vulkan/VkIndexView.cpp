#include "VkIndexView.h"

Eugene::VkIndexView::VkIndexView(std::uint32_t size, std::uint32_t indexNum, vk::Buffer& buffer):
	IndexView{indexNum}
{
	buffer_ = buffer;
}

void* Eugene::VkIndexView::GetView(void)
{
	return &buffer_;
}
