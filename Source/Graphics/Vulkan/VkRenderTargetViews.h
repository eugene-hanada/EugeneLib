#pragma once
#include "../../../Include/Graphics/RenderTargetViews.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/Math/Geometry.h"
#include <optional>

namespace Eugene
{
	class VkRenderTargetViews :
		public RenderTargetViews
	{
	public:
		
		VkRenderTargetViews(std::uint64_t size);

		/// <summary>
		/// �f�[�^
		/// </summary>
		struct Data
		{
			/// <summary>
			/// ImageView
			/// </summary>
			vk::UniqueImageView imageView;

			/// <summary>
			/// �摜�T�C�Y
			/// </summary>
			glm::ivec2 size;
		};
		using ViewsType = std::vector<Data>;
	private:
		// RenderTargetViews ����Čp������܂���
		void Create(ImageResource& resource, std::uint64_t idx) final;
		void* GetViews(void) final;

		/// <summary>
		/// �r���[�̃f�[�^
		/// </summary>
		ViewsType imageViews_;
	};
}