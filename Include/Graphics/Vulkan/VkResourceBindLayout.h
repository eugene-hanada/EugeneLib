#pragma once
#include "../GraphicsCommon.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include "../../Utils/ArgsSpan.h"

namespace Eugene
{
	class ResourceBindLayout
	{
	public:
		ResourceBindLayout() = default;
		~ResourceBindLayout() noexcept
		{
			descriptorLayoutArray_.clear();
			pipelineLayout_ = vk::PipelineLayout{};
		}

		void Init(const ArgsSpan<ArgsSpan<Bind>>& viewTypes);

		void Final() noexcept
		{
			descriptorLayoutArray_.clear();
			pipelineLayout_ = vk::PipelineLayout{};
		}

		ResourceBindLayout(ResourceBindLayout&& resourceBind) noexcept :
			descriptorLayoutArray_{ std::move(resourceBind.descriptorLayoutArray_) },
			pipelineLayout_{std::move(resourceBind.pipelineLayout_)}
		{
		}

		ResourceBindLayout& operator=(ResourceBindLayout&& resourceBind) noexcept
		{
			descriptorLayoutArray_ = std::move(resourceBind.descriptorLayoutArray_);
			pipelineLayout_ = std::move(resourceBind.pipelineLayout_);
		}

	private:
		ResourceBindLayout(const ResourceBindLayout&) = delete;
		ResourceBindLayout& operator=(const ResourceBindLayout) = delete;

		ResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes)
		{
			Init(viewTypes);
		}

		/// <summary>
		/// ディスクリプタセットのレイアウト
		/// </summary>
		std::vector<vk::DescriptorSetLayout> descriptorLayoutArray_;

		/// <summary>
		/// パイプラインのレイアウト
		/// </summary>
		vk::PipelineLayout pipelineLayout_;

		friend class Graphics;
		friend class Pipeline;
	};
}
