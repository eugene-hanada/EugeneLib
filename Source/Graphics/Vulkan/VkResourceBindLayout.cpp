#include "VkResourceBindLayout.h"

Eugene::VkResourceBindLayout::VkResourceBindLayout(const vk::Device& device, const ArgsSpan<ArgsSpan<Bind>>& viewTypes)
{
	vk::DescriptorSetLayoutCreateInfo layoutInfo{};
	std::vector<std::vector<vk::DescriptorSetLayoutBinding>> bindingVector(viewTypes.size());
	for (std::size_t i = 0ull; i < bindingVector.size(); i++)
	{
		auto now = (i + viewTypes.begin());
		bindingVector[i].resize(now->size());
		for (std::size_t j = 0ull; j < now->size(); j++)
		{
			
		}
	}
}
