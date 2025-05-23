#pragma once
#include <cstdint>
#include <limits>
#include <bitset>
#include <span>
#include <vector>
#include <type_traits>

namespace Eugene
{
	/// <summary>
	/// フォーマット
	/// </summary>
	enum class Format :
		std::uint8_t
	{
		NON,

		// R32G32B32A32
		R32G32B32A32_TYPELESS,
		R32G32B32A32_FLOAT,
		R32G32B32A32_UINT,
		R32G32B32A32_SINT,

		// R32G32B32
		R32G32B32_TYPELESS,
		R32G32B32_FLOAT,
		R32G32B32_UINT,
		R32G32B32_SINT,

		// R32G32
		R32G32_TYPELESS,
		R32G32_FLOAT,
		R32G32_UINT,
		R32G32_SINT,

		// R32
		R32_TYPELESS,
		D32_FLOAT,
		R32_FLOAT,
		R32_UINT,
		R32_SINT,

		// R16G16B16A16
		R16G16B16A16_TYPELESS,
		R16G16B16A16_FLOAT,
		R16G16B16A16_UNORM,
		R16G16B16A16_UINT ,
		R16G16B16A16_SNORM,
		R16G16B16A16_SINT ,

		// R16G16
		R16G16_TYPELESS,
		R16G16_FLOAT,
		R16G16_UNORM,
		R16G16_UINT,
		R16G16_SNORM,
		R16G16_SINT,

		// R16
		R16_TYPELESS,
		R16_FLOAT,
		D16_UNORM,
		R16_UNORM,
		R16_UINT,
		R16_SNORM,
		R16_SINT,

		// R8G8B8A8
		R8G8B8A8_TYPELESS,
		R8G8B8A8_UNORM,
		R8G8B8A8_UNORM_SRGB,
		R8G8B8A8_UINT,
		R8G8B8A8_SNORM,
		R8G8B8A8_SINT,

		// B8G8R8A8
		B8G8R8A8_TYPELESS,
		B8G8R8A8_UNORM,
		B8G8R8A8_UNORM_SRGB,
		B8G8R8A8_UINT,
		B8G8R8A8_SNORM,
		B8G8R8A8_SINT,

		// 圧縮テクスチャ
		BC1_UNORM,
		BC2_UNORM,
		BC3_UNORM,
		BC5_UNORM,
		BC7_UNORM,

		/// <summary>
		/// バックバッファと同じフォーマットにする
		/// </summary>
		AUTO_BACKBUFFER
	};

	constexpr size_t FormatSize[]
	{
		// NON
		0ull,

		// R32G32B32A32
		16ull,
		16ull,
		16ull,
		16ull,

		// R32G32B32
		12ull,
		12ull,
		12ull,
		12ull,

		// R32G32
		8ull,
		8ull,
		8ull,
		8ull,

		// R32
		4ull,
		4ull,
		4ull,
		4ull,
		4ull,

		// R16G16B16A16
		8ull,
		8ull,
		8ull,
		8ull,
		8ull,
		8ull,

		// R16G16
		4ull,
		4ull,
		4ull,
		4ull,
		4ull,
		4ull,

		// R16
		2ull,
		2ull,
		2ull,
		2ull,
		2ull,
		2ull,
		2ull,

		// R8G8B8A8
		4ull,
		4ull,
		4ull,
		4ull,
		4ull,
		4ull,
	};

	/// <summary>
	/// フォーマットの最大数
	/// </summary>
	constexpr auto FormatMax = 54ull;

	enum class IndexType :
		std::uint8_t
	{
		UINT32,
		UINT16,
		UINT8
	};

	/// <summary>
	/// テクスチャ用画像情報
	/// </summary>
	struct TextureInfo
	{
		std::uint32_t width;			// 横幅
		std::uint32_t height;			// 縦幅
		Format format;			// フォーマット
		std::uint32_t arraySize;		// 配列サイズ
		std::uint16_t mipLevels;			// ミップマップレベル
		std::int32_t pixelPerBite;			// ピクセル当たりのバイト数
		std::uint32_t totalSize_;			// トータルサイズ
	};

	/// <summary>
	/// プリミティブタイプ
	/// </summary>
	enum class PrimitiveType :
		std::uint8_t
	{
		Point = 1,
		Line = 2,
		LineStrip = 3,
		Triangle = 4,
		TriangleStrip = 5
	};

	/// <summary>
	/// サブリソースの最大数
	/// </summary>
	constexpr auto maxSubResource = 64u;

	/// <summary>
	/// Gpuメモリの情報
	/// </summary>
	struct GpuMemoryInfo
	{
		/// <summary>
		/// 使用中のバイト数
		/// </summary>
		std::uint64_t usage;

		/// <summary>
		/// プログラムで使用可能なバイト数
		/// </summary>
		std::uint64_t budget;
	};

	/// <summary>
	/// Gpuリソースの種類
	/// </summary>
	enum class GpuResourceType :
		std::uint8_t
	{
		/// <summary>
		/// デフォルト
		/// </summary>
		Default,

		/// <summary>
		/// アップロード用
		/// </summary>
		Upload,

		/// <summary>
		/// 読み取り用
		/// </summary>
		ReadBack
	};

	/// <summary>
/// ビューのタイプ
/// </summary>
	enum class ViewType : 
		std::uint8_t
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
		Sampler,

		/// <summary>
		/// 32ビットの定数
		/// </summary>
		Constans
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
		using UnderlyingType = std::underlying_type<ResourceBindFlag>::type;
		return ResourceBindFlags{ static_cast<UnderlyingType>(rflag) | static_cast<UnderlyingType>(lflag) };
	}

	struct Bind
	{
		ViewType viewType_{ ViewType::ConstantBuffer };
		std::uint32_t viewNum_{ 1u };
	};

	/// <summary>
/// シェーダの入力レイアウト用構造体
/// </summary>
	struct ShaderInputLayout
	{
		constexpr ShaderInputLayout() :
			format_{ Format::NON }, semanticIdx_{ 0u }, semanticName_{ nullptr }, slot_{ 0u }
		{

		}
		constexpr ShaderInputLayout(const char* semanticName, std::uint32_t semanticIdx, Format format, std::uint32_t slot = 0) :
			semanticName_{ semanticName }, semanticIdx_{ semanticIdx }, format_{ format }, slot_{ slot }
		{

		}
		std::uint32_t semanticIdx_;
		Format format_;
		std::uint32_t slot_;
		const char* semanticName_;
	};

	/// <summary>
	/// シェーダタイプ
	/// </summary>
	enum class ShaderType : 
		std::uint8_t
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
	enum class BlendType : 
		std::uint8_t
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
		constexpr RendertargetLayout() :
			format_{ Format::NON }, blendType_{ BlendType::Non }
		{

		}
		constexpr RendertargetLayout(Format format, BlendType blendType = BlendType::Non) :
			format_{ format }, blendType_{ blendType }
		{

		}
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
	enum class TopologyType :
		std::uint8_t
	{
		Non = 0,
		Point = 1,
		Line = 2,
		Triangle = 3,
		Patch = 4
	};


	using ShaderInputSpan = std::span<ShaderInputLayout>;
	using ShaderPair = std::pair<std::span<std::uint8_t>, ShaderType>;
	using ShaderTypePaisrSpan = std::span<ShaderPair>;
	struct SamplerLayout;
	using ShaderLayoutSpan = std::span<std::vector<ShaderLayout>>;
	using SamplerSpan = std::span<SamplerLayout>;
	using RenderTargetSpan = std::span <RendertargetLayout>;

	/// <summary>
/// テクスチャアドレッシングモード
/// </summary>
	enum class TextureAddressMode :
		std::uint8_t
	{
		Wrap = 1,
		Mirror = 2,
		Clamp = 3,
		Border = 4,
		MirrorOnce = 5
	};

	/// <summary>
	/// サンプラーのフィルター
	/// </summary>
	enum class SampleFilter :
		std::uint8_t
	{
		Point = 0,
		Linear = 0x15,
		Anisotropic = 0x55
	};

	/// <summary>
	/// サンプラーの比較演算用
	/// </summary>
	enum class SamplerComparison :
		std::uint8_t
	{
		Non,
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always
	};

	/// <summary>
	/// サンプラーのレイアウト用
	/// </summary>
	struct SamplerLayout
	{
		constexpr SamplerLayout(
			TextureAddressMode u = TextureAddressMode::Wrap,
			TextureAddressMode v = TextureAddressMode::Wrap,
			TextureAddressMode w = TextureAddressMode::Wrap,
			SampleFilter filter = SampleFilter::Point
		) : u_{u}, v_{v}, w_{w}, filter_{filter}
		{
			maxAnisotropy_ = 16;
			comparison_ = SamplerComparison::Non;
			minLod_ = 0.0f;
			maxLod_ = std::numeric_limits<float>::max();
		}
		TextureAddressMode u_;
		TextureAddressMode v_;
		TextureAddressMode w_;
		SampleFilter filter_;
		std::uint32_t maxAnisotropy_;
		SamplerComparison comparison_;
		float maxLod_;
		float minLod_;
	};
}