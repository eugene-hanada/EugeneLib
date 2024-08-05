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

		ResourceBindLayout(const ResourceBindLayout& bindLayout) noexcept :
			rootSignature_{bindLayout.rootSignature_}
		{
		}

		ResourceBindLayout& operator=(const ResourceBindLayout& bindLayout) noexcept
		{
			rootSignature_ = bindLayout.rootSignature_;
		}

		ResourceBindLayout() = default;
	private:

		ResourceBindLayout(const ArgsSpan<ArgsSpan<Bind>>& viewTypes, ResourceBindFlags flags);

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

		friend class Graphics;
		friend class Pipeline;
	};
}
