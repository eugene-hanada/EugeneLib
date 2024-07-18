#pragma once
#include <cstdint>
#include <bitset>

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
		UnoderedAccess,

		/// <summary>
		/// 定数バッファ
		/// </summary>
		ConstantBuffer,

		/// <summary>
		/// サンプラー
		/// </summary>
		Sampler
	};

	enum class ResourceBindFlag :
		std::uint32_t
	{
		Non = 0,

		/// <summary>
		/// 入力(読み込みのみ)
		/// </summary>
		Input = 1,

		/// <summary>
		/// ストリーム用出力(書き換え可)
		/// </summary>
		StreamOutput = 2,
	};

	using ResourceBindFlags = std::bitset<4>;

	constexpr ResourceBindFlags operator|(ResourceBindFlag rflag, ResourceBindFlag lflag)
	{
		return ResourceBindFlags{ std::to_underlying(rflag) | std::to_underlying(lflag) };
	}

	struct Bind
	{
		ViewType viewType_{ViewType::ConstantBuffer};
		std::uint32_t viewNum_{1u};
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