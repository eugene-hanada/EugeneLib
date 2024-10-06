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

		friend class Graphics;
		friend class Pipeline;
	};
}
