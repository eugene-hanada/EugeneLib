#pragma once
#include <vulkan/vulkan.hpp>
#include "../../Utils//ArgsSpan.h"
#include "../GraphicsCommon.h"

namespace Eugene
{
	class Sampler;

	/// <summary>
	/// サンプラービュー
	/// </summary>
	class SamplerViews
	{
	public:
		struct Data
		{
			Data() = default;

			Data(Data&& data) noexcept :
				descriptorSet_{ std::move(data.descriptorSet_) }, layout_{ std::move(data.layout_) }
			{
			}

			Data& operator=(Data&& data) noexcept
			{
				descriptorSet_ = std::move(data.descriptorSet_);
				layout_ = std::move(data.layout_);
			}

			vk::UniqueDescriptorSet descriptorSet_;
			vk::UniqueDescriptorSetLayout layout_;
		};


		/// <summary>
		/// ビューを生成する
		/// </summary>
		/// <param name="sampler"> サンプラー </param>
		/// <param name="idx"> インデックス </param>
		void CreateSampler(Sampler& sampler, std::uint32_t idx);

		/// <summary>
		/// API側のビューを取得する
		/// </summary>
		/// <returns> ビューのポインタ </returns>
		void* GetViews() noexcept
		{
			return &data_;
		}

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="views"></param>
		SamplerViews(SamplerViews&& views) noexcept :
			descriptorPool_{std::move(views.descriptorPool_)}, data_{std::move(views.data_)},
			typeData_{std::move(views.typeData_)}
		{
		}

		/// <summary>
		/// ムーブ演算子
		/// </summary>
		/// <param name="views"></param>
		/// <returns></returns>
		SamplerViews& operator=(SamplerViews&& views) noexcept
		{
			descriptorPool_ = std::move(views.descriptorPool_);
			data_ = std::move(views.data_);
			typeData_ = std::move(views.typeData_);
		}

		/// <summary>
		/// 終了処理
		/// </summary>
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
