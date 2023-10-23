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

		/// <summary>
		/// �o�b�t�@
		/// </summary>
		vk::Buffer buffer_;
	};
}

