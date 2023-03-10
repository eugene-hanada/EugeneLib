﻿#pragma once
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

		// R16B16
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
		//NON = 0,
		//R32G32B32A32_TYPELESS = 1,
		//R32G32B32A32_FLOAT = 2,
		//R32G32B32A32_UINT = 3,
		//R32G32B32A32_SINT = 4,
		//R32G32B32_TYPELESS = 5,
		//R32G32B32_FLOAT = 6,
		//R32G32B32_UINT = 7,
		//R32G32B32_SINT = 8,
		//R16G16B16A16_TYPELESS = 9,
		//R16G16B16A16_FLOAT = 10,
		//R16G16B16A16_UNORM = 11,
		//R16G16B16A16_UINT = 12,
		//R16G16B16A16_SNORM = 13,
		//R16G16B16A16_SINT = 14,
		//R32G32_TYPELESS = 15,
		//R32G32_FLOAT = 16,
		//R32G32_UINT = 17,
		//R32G32_SINT = 18,
		//R10G10B10A2_TYPELESS = 23,
		//R10G10B10A2_UNORM = 24,
		//R10G10B10A2_UINT = 25,
		//R11G11B10_FLOAT = 26,
		//R8G8B8A8_TYPELESS = 27,
		//R8G8B8A8_UNORM = 28,
		//R8G8B8A8_UNORM_SRGB = 29,
		//R8G8B8A8_UINT = 30,
		//R8G8B8A8_SNORM = 31,
		//R8G8B8A8_SINT = 32,
		//R16G16_TYPELESS = 33,
		//R16G16_FLOAT = 34,
		//R16G16_UNORM = 35,
		//R16G16_UINT = 36,
		//R16G16_SNORM = 37,
		//R16G16_SINT = 38,
		//R32_TYPELESS = 39,
		//D32_FLOAT = 40,
		//R32_FLOAT = 41,
		//R32_UINT = 42,
		//R32_SINT = 43,
	};

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
	};

	/// <summary>
	/// プリミティブタイプ
	/// </summary>
	enum class PrimitiveType
	{
		Point = 1,
		Line = 2,
		Triangle = 4,
		TriangleStrip = 5
	};

	constexpr auto maxSubResource = 64u;
}