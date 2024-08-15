#pragma once
#include <vulkan/vulkan.hpp>
#include "../../Utils//ArgsSpan.h"
#include "../GraphicsCommon.h"

namespace Eugene
{
	class Sampler;
	class SamplerViews
	{
		struct Data
		{
			Data() = default;

			Data(Data&& data) :
			descriptorSet_{ std::move(data.descriptorSet_) }, layout_{ std::move(data.layout_) }
			{
			}

			Data& operator=(Data&& data)
			{
				descriptorSet_ = std::move(data.descriptorSet_);
				layout_ = std::move(data.layout_);
			}

			vk::UniqueDescriptorSet descriptorSet_;
			vk::UniqueDescriptorSetLayout layout_;
		};

	public:
		using ViewsType = Data;

		// SamplerViews を介して継承されました
		void CreateSampler(Sampler& sampler, std::uint32_t idx);
		ViewsType& GetViews() noexcept
		{
			return data_;
		}

		SamplerViews(SamplerViews&& views) noexcept :
			descriptorPool_{std::move(views.descriptorPool_)}, data_{std::move(views.data_)},
			typeData_{std::move(views.typeData_)}
		{
		}
		SamplerViews& operator=(SamplerViews&& views)
		{
			descriptorPool_ = std::move(views.descriptorPool_);
			data_ = std::move(views.data_);
			typeData_ = std::move(views.typeData_);
		}

		void Final() noexcept
		{
			typeData_.clear();
			data_.layout_.reset();
			data_.descriptorSet_.reset();
			descriptorPool_.reset();
		}

		SamplerViews(const SamplerViews&) = delete;
		SamplerViews& operator=(const SamplerViews&) = delete;
	private:

		SamplerViews(const ArgsSpan<Bind>& viewTypes);
		
		vk::UniqueDescriptorPool descriptorPool_;
		Data data_;
		std::vector<std::pair<std::uint32_t, std::uint32_t>> typeData_;

		friend class Graphics;
	};
}
