#pragma once
#include <vulkan/vulkan.hpp>
#include "VkBufferResource.h"

namespace Eugene
{
	class VertexView
	{
	public:
		using ViewType = vk::Buffer;
		
		std::uint32_t GetVertexNum() const noexcept
		{
			return vertexNum_;
		}

		ViewType& GetView()
		{
			return buffer_;
		}

		VertexView() noexcept :
			vertexNum_{0}
		{
		}

		VertexView(std::uint32_t size, std::uint32_t vertexNum, BufferResource& resource) noexcept :
			buffer_{ resource.GetResource() }, vertexNum_{ vertexNum }
		{
		}

		void Init(std::uint32_t size, std::uint32_t vertexNum, BufferResource& resource)
		{
			buffer_ = resource.GetResource();
			vertexNum_ = vertexNum;
		}
	private:
		
	
		/// <summary>
		/// バッファ
		/// </summary>
		vk::Buffer buffer_;

		/// <summary>
		/// 
		/// </summary>
		std::uint32_t vertexNum_;
	};
}

