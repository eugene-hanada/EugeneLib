#pragma once
#include <cstdint>

namespace Eugene
{
	/// <summary>
	/// フォーマット
	/// </summary>
	enum class Format
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

		// 圧縮テクスチャ
		BC1_UNORM,
		BC2_UNORM,
		BC3_UNORM,
		BC5_UNORM,
		BC7_UNORM
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
	constexpr auto FormatMax = 48ull;

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
	};

	/// <summary>
	/// プリミティブタイプ
	/// </summary>
	enum class PrimitiveType
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
}