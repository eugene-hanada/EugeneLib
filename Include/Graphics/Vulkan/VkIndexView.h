#pragma once
#include <vulkan/vulkan.hpp>
#include  "VkBufferResource.h"

namespace Eugene
{
	class GpuResource;
	class IndexView
	{
	public:
		using ViewType = vk::Buffer;



		std::uint32_t GetIndexNum() const noexcept
		{
			return indexNum_;
		}

		ViewType& GetView() noexcept
		{
			return buffer_;
		}

		IndexView() noexcept :
			indexNum_{ 0 }
		{
		}

		IndexView(std::uint32_t size, std::uint32_t indexNum, BufferResource& resource) noexcept :
			buffer_{resource.GetResource()}, indexNum_{indexNum}
		{
		}

		void Init(std::uint32_t size, std::uint32_t indexNum, BufferResource& resource)noexcept
		{
			buffer_ = resource.GetResource();
			indexNum_ = indexNum;
		}

		IndexView(const IndexView& view) noexcept :
			buffer_{ view.buffer_ }, indexNum_{ view.indexNum_ }
		{
		}

		IndexView& operator=(const IndexView& view) noexcept
		{
			buffer_ = view.buffer_;
			indexNum_ = view.indexNum_;
		}
	private:
	
		vk::Buffer buffer_;
		std::uint32_t indexNum_;
	};
}