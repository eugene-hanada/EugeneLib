#pragma once
#include "../../../Include/Graphics/ShaderResourceViews.h"
#include <vulkan/vulkan.hpp>
#include "../../../Include/Common/ArgsSpan.h"
#include "../../../Include/Graphics/ResourceBindLayout.h"

namespace Eugene
{
	class VkShaderResourceViews :
		public ShaderResourceViews
	{
	public:
		VkShaderResourceViews(const vk::Device& device, const ArgsSpan<Bind>& ViewTypes);
	private:

		// ShaderResourceViews ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		void CreateTexture(ImageResource& resource, std::uint64_t idx) final;
		void CreateConstantBuffer(BufferResource& resource, std::uint64_t idx) final;
		void CreateCubeMap(ImageResource& resource, std::uint64_t idx) final;
		void* GetViews(void) const final;
		std::uint64_t GetImg(void) final;
		vk::UniqueDescriptorSet descriptorSet_;
		vk::UniqueDescriptorPool descriptorPool_;
		vk::UniqueDescriptorSetLayout layout_;
		std::vector<std::tuple<ViewType, std::uint32_t, std::uint32_t>> typeData_;
	};
}