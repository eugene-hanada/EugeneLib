#pragma once
#include "../../../Include/Graphics/VertexView.h"
#include <vulkan/vulkan.hpp>

namespace Eugene
{
	class VkVertexView :
		public VertexView
	{
	public:
		VkVertexView(size_t size, size_t vertexNum, vk::Buffer& buffer);
	private:
		// VertexView ����Čp������܂���
		void* GetView(void);

		vk::Buffer buffer_;
	};
}

