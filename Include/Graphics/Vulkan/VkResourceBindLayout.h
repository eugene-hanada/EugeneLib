#pragma once
#include "../GraphicsCommon.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include "../../Utils/ArgsSpan.h"
#include <mimalloc.h>

namespace Eugene
{
	/// <summary>
	/// シェーダへのリソースバインドのレイアウトを定義したクラス
	/// </summary>
	class ResourceBindLayout
	{
	public:
		ResourceBindLayout() = default;
		~ResourceBindLayout() noexcept
		{
			descriptorLayoutArray_.clear();
			pipelineLayout_.reset();
			pushConstantSize_ = 0;
		}

		/// <summary>
		/// 終了処理
		/// </summary>
		void Final() noexcept
		{
			for (auto& setLayout : descriptorLayoutArray_)
			{
				pipelineLayout_->getOwner().destroy(setLayout);
			}
			descriptorLayoutArray_.clear();
			pipelineLayout_.reset();
			pushConstantSize_ = 0;
		}

		ResourceBindLayout(ResourceBindLayout&& resourceBind) noexcept :
			descriptorLayoutArray_{ std::move(resourceBind.descriptorLayoutArray_) },
			pipelineLayout_{std::move(resourceBind.pipelineLayout_)}, 
			pushConstantSize_{ resourceBind.pushConstantSize_ }
		{
		}

		ResourceBindLayout& operator=(ResourceBindLayout&& resourceBind) noexcept
		{
			descriptorLayoutArray_ = std::move(resourceBind.descriptorLayoutArray_);
			pipelineLayout_ = std::move(resourceBind.pipelineLayout_);
			pushConstantSize_ = resourceBind.pushConstantSize_;
			resourceBind.pushConstantSize_ = 0;
			return *this;
		}

		ResourceBindLayout(const ResourceBindLayout&) = delete;
		ResourceBindLayout& operator=(const ResourceBindLayout) = delete;
	private:


		ResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes);

		/// <summary>
		/// ディスクリプタセットのレイアウト
		/// </summary>
		std::vector<vk::DescriptorSetLayout, mi_stl_allocator<vk::DescriptorSetLayout>> descriptorLayoutArray_;

		/// <summary>
		/// パイプラインのレイアウト
		/// </summary>
		std::shared_ptr<vk::UniquePipelineLayout> pipelineLayout_;

		std::uint32_t pushConstantSize_{ 0 };

		friend class Graphics;
		friend class Pipeline;
	};
}
