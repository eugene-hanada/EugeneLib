#pragma once
#include <cstdint>
#include <span>
#include <vector>
#include "GraphicsCommon.h"
#include "ResourceBindLayout.h"
#include "Sampler.h"

namespace Eugene
{
	/// <summary>
	/// シェーダの入力レイアウト用構造体
	/// </summary>
	struct ShaderInputLayout
	{
		ShaderInputLayout();
		ShaderInputLayout(const char* semanticName, std::uint32_t semanticIdx, Format format, std::uint32_t slot = 0);
		const char* semanticName_;
		std::uint32_t semanticIdx_;
		Format format_;
		std::uint32_t slot_;
	};

	/// <summary>
	/// シェーダタイプ
	/// </summary>
	enum class ShaderType
	{
		/// <summary>
		/// 頂点
		/// </summary>
		Vertex,

		/// <summary>
		/// ピクセル
		/// </summary>
		Pixel,

		/// <summary>
		/// 
		/// </summary>
		Frag = Pixel,

		/// <summary>
		/// ジオメトリ
		/// </summary>
		Geometry,

		/// <summary>
		/// ドメイン
		/// </summary>
		Domain,

		/// <summary>
		/// ハル
		/// </summary>
		Hull,
	};

	/// <summary>
	/// ブレンドタイプ
	/// </summary>
	enum class BlendType
	{
		/// <summary>
		/// 無し
		/// </summary>
		Non,

		/// <summary>
		/// アルファ
		/// </summary>
		Alpha,

		/// <summary>
		/// 加算
		/// </summary>
		Add,

		/// <summary>
		/// 減算
		/// </summary>
		Sub,

		/// <summary>
		/// 逆
		/// </summary>
		Inv
	};

	/// <summary>
	/// レンダーターゲットのレイアウト
	/// </summary>
	struct RendertargetLayout
	{
		/// <summary>
		/// フォーマット
		/// </summary>
		Format format_;

		/// <summary>
		/// ブレンドタイプ
		/// </summary>
		BlendType blendType_;
	};

	/// <summary>
	/// シェーダーレイアウト用構造体
	/// </summary>
	struct ShaderLayout
	{
		/// <summary>
		/// ビューのタイプ
		/// </summary>
		ViewType viewType_;

		/// <summary>
		/// ビューの数
		/// </summary>
		std::uint32_t numView_;

		/// <summary>
		/// ベースレジスタ
		/// </summary>
		std::uint32_t baseRegister_;
	};

	/// <summary>
	/// トポロジータイプ
	/// </summary>
	enum class TopologyType
	{
		Point = 1,
		Line = 2,
		Triangle = 3,
		Patch = 4
	};

	class Shader;
	class Graphics;
	using ShaderInputSpan = std::span<ShaderInputLayout>;
	using ShaderPair = std::pair<Shader, ShaderType>;
	using ShaderTypePaisrSpan = std::span<ShaderPair>;
	using ShaderLayoutSpan = std::span<std::vector<ShaderLayout>>;
	using SamplerSpan = std::span<SamplerLayout>;
	using RenderTargetSpan = std::span <RendertargetLayout>;

	/// <summary>
	/// グラフィックスパイプラインを設定しているクラス
	/// </summary>
	class GraphicsPipeline
	{
	public:
		virtual ~GraphicsPipeline();
		virtual void* GetPipeline(void) = 0;
	protected:
		GraphicsPipeline();

	};
}
