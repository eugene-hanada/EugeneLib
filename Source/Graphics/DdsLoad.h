#pragma once
#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <functional>
#include "../../Include/Graphics/Image.h"

namespace Eugene
{

	struct DdsHeader
	{
		/// <summary>
		/// ヘッダーサイズ
		/// </summary>
		std::int32_t size;

		/// <summary>
		/// ヘッダ内の情報のフラグ
		/// </summary>
		std::int32_t flags;

		/// <summary>
		/// 高さ
		/// </summary>
		std::int32_t height;

		/// <summary>
		/// 幅
		/// </summary>
		std::int32_t width;

		/// <summary>
		/// 横1ラインのバイト数または1面分のバイト数
		/// </summary>
		std::int32_t pitchOrLinearSize;

		/// <summary>
		/// 奥行サイズ
		/// </summary>
		std::int32_t depth;

		/// <summary>
		/// ミップマップ数
		/// </summary>
		std::int32_t mipMapCount;

		/// <summary>
		/// 予約領域
		/// </summary>
		std::int32_t reserved1[11];

		/// <summary>
		/// ピクセルフォーマット情報のバイト数
		/// </summary>
		std::int32_t pixelFormatSize;

		/// <summary>
		/// ピクセルフォーマットのフラグ
		/// </summary>
		std::int32_t pixelFormatFlags;

		/// <summary>
		/// フォーマットの定義
		/// </summary>
		std::int32_t fourCC;

		/// <summary>
		/// 1ピクセル当たりのビット数
		/// </summary>
		std::int32_t rgbBitCount;

		std::int32_t rBitMask;
		std::int32_t gBitMask;
		std::int32_t bBitMask;
		std::int32_t aBitMask;

		/// <summary>
		/// ミップマップやキューブマップとかの情報
		/// </summary>
		std::int32_t caps;

		/// <summary>
		/// 上の詳細な奴
		/// </summary>
		std::int32_t caps2;

		/// <summary>
		/// 予約領域
		/// </summary>
		std::int32_t reservedCaps[2];

		/// <summary>
		/// 予約領域
		/// </summary>
		std::int32_t reserved2;
	};

	struct DdsExtensionHeader
	{
		/// <summary>
		/// フォーマット
		/// </summary>
		std::uint32_t format = 0u;

		/// <summary>
		/// 1Dとか2Dとかの情報
		/// </summary>
		std::uint32_t dimension = 0u;

		/// <summary>
		/// 
		/// </summary>
		std::uint32_t miscFlag = 0u;

		/// <summary>
		/// 配列サイズ
		/// </summary>
		std::uint32_t arraySize = 0u;

		/// <summary>
		/// 
		/// </summary>
		std::uint32_t miscFlag2 = 0u;
	};

	struct ColcDxt1Size 
	{
		constexpr ColcDxt1Size() = default;
		std::int32_t operator()(std::int32_t x, std::int32_t y, std::int32_t pixelSize)
		{
			return std::max(1, ((x + 3) / 4)) * std::max(1, ((y + 3) / 4)) * 8;
		}
	};

	struct ColcSize
	{
		constexpr ColcSize() = default;
		std::int32_t operator()(std::int32_t x, std::int32_t y, std::int32_t pixelSize)
		{
			return x * y * pixelSize;
		}
	};

	struct ColcDxt3Size
	{
		constexpr ColcDxt3Size() = default;
		std::int32_t operator()(std::int32_t x, std::int32_t y, std::int32_t pixelSize)
		{
			return std::max(1, ((x + 3) / 4)) * std::max(1, ((y + 3) / 4)) * 16;
		}
	};

	const std::unordered_map<Format, std::function<std::int32_t(std::int32_t, std::int32_t, std::int32_t)>> colcMap
	{
		{Format::R8G8B8A8_UNORM,ColcSize{}},
		{Format::BC1_UNORM,ColcDxt1Size{}},
		{Format::BC2_UNORM,ColcDxt3Size{}},
		{Format::BC3_UNORM,ColcDxt3Size{}}
	};

	void LoadDdsExtension(std::ifstream& file, TextureInfo& info)
	{
		// 追加情報を読み込む
		DdsExtensionHeader ext;
		file.read(reinterpret_cast<char*>(&ext), sizeof(ext));
		info.arraySize = std::max(1u,ext.arraySize);
		
		// フォーマットをセットする
		switch (ext.format)
		{
		case 98:
			info.format = Format::BC7_UNORM;
			return;
		case 70:
			info.format = Format::BC1_UNORM;
			return;
		default:
			info.format = Format::R8G8B8A8_SNORM;
			return;
		}
		
	}
}
