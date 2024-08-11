#pragma once
#include "../GraphicsCommon.h"
#include "../../Utils/ArgsSpan.h"
#include <wrl.h>
#include <d3d12.h>

namespace Eugene
{
	class ResourceBindLayout
	{
	public:
		ResourceBindLayout(ResourceBindLayout&& bindLayout) noexcept :
			rootSignature_{std::move(bindLayout.rootSignature_)}
		{
		}
		ResourceBindLayout& operator=(ResourceBindLayout&& bindLayout) noexcept
		{
			rootSignature_ = std::move(bindLayout.rootSignature_);
		}

		void Final() noexcept
		{
			rootSignature_.Reset();
		}

		ResourceBindLayout() = default;
	private:

		ResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlags flags);

		ResourceBindLayout(const ResourceBindLayout& bindLayout) = delete;

		ResourceBindLayout& operator=(const ResourceBindLayout& bindLayout) = delete;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

		friend class Graphics;
		friend class Pipeline;
	};
}
