#pragma once
#include <cstdint>

namespace Eugene
{
	/// <summary>
/// ビューのタイプ
/// </summary>
	enum class ViewType
	{
		/// <summary>
		/// テクスチャ
		/// </summary>
		Texture,

		/// <summary>
		/// UA
		/// </summary>
		UnoderedAccsec,

		/// <summary>
		/// 定数バッファ
		/// </summary>
		ConstantBuffer,

		/// <summary>
		/// サンプラー
		/// </summary>
		Sampler
	};

	struct Bind
	{
		ViewType viewType_;
		std::uint32_t viewNum_;
	};

	/// <summary>
	/// シェーダにリソースをバインドする際のレイアウト等を決めるクラス(GraphicsPipeline生成に必要)
	/// </summary>
	class ResourceBindLayout
	{
	public:
		virtual ~ResourceBindLayout();
	private:
	};
}