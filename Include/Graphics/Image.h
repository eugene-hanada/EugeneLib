#pragma once
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <span>
#include <functional>
#include "GraphicsCommon.h"

namespace Eugene
{
	/// <summary>
	/// 画像データクラス(RAM側にデータがある)、対応フォーマット[jpeg, png, tag, bmp, psd, gif, hdr, pic, pnm, dds]
	/// </summary>
	class Image
	{
	public:
		
		/// <summary>
		/// コンストラクタ(指定したテクスチャファイルを読み込む)
		/// </summary>
		/// <param name="path"> テクスチャファイルのパス </param>
		Image(const std::filesystem::path& path);

		/// <summary>
		/// コンストラクタ(指定されたバイナリデータから読み込む)
		/// </summary>
		/// <param name="data"></param>
		Image(std::span<std::uint8_t> data, const std::string_view& ext);

		/// <summary>
		/// テクスチャの情報を取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const TextureInfo& GetInfo(void) const&;

		/// <summary>
		/// テクスチャデータを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> テクスチャデータ </returns>
		std::uint8_t* GetData(std::uint32_t arrayIndex = 0u,std::uint16_t mipMapLevel = 0u);

		/// <summary>
		/// ムーブコンストラクタ
		/// </summary>
		/// <param name="img"></param>
		Image(Image&& img) noexcept;

		/// <summary>
		/// move演算子
		/// </summary>
		/// <param name="img"></param>
		/// <returns></returns>
		Image& operator=(Image&& img) noexcept;
	private:

		using LoadFromFileFunc = bool(Image::*)(const std::filesystem::path&);
		using LoadFromMemoryFunc = bool(Image::*)(const std::span<std::uint8_t>&);
		using LoadFromFileFuncMap = std::unordered_map<std::uint64_t, LoadFromFileFunc>;
		using LoadFromMemoryFuncMap = std::unordered_map<std::uint64_t, LoadFromMemoryFunc>;

		/// <summary>
		/// stbライブラリを使用して読み込む
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		bool LoadStbFromFile(const std::filesystem::path& path);

		/// <summary>
		/// stbライブラリを使用してメモリ上のデータを読み込む
		/// </summary>
		/// <param name="data"></param>
		/// <returns></returns>
		bool LoadStbFromMemory(const std::span<std::uint8_t>& data);

		/// <summary>
		/// ddsファイルを読み込む
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		bool LoadDdsFromFile(const std::filesystem::path& path);

		/// <summary>
		/// ddsファイルのデータメモリ上から読み込む
		/// </summary>
		/// <param name="data"></param>
		/// <returns></returns>
		bool LoadDdsFromMemory(const std::span<std::uint8_t>& data);

		/// <summary>
		/// テクスチャデータのアクセス用(ミップマップや配列ごとにspanに分けてる)
		/// </summary>
		std::vector<std::span<std::uint8_t>> data_;

		/// <summary>
		/// テクスチャデータ(ひとまとまりにしている)
		/// </summary>
		std::vector<std::uint8_t> baseData_;

		/// <summary>
		/// テクスチャ情報
		/// </summary>
		TextureInfo info_;

		/// <summary>
		/// ロード用関数のmap
		/// </summary>
		static const LoadFromFileFuncMap loadFromFileFuncMap_;

		/// <summary>
		/// メモリからのロード用の関数map
		/// </summary>
		static const LoadFromMemoryFuncMap loadFromMemoryMap_;
	};

	/// <summary>
	/// フォーマットごとにサイズを計算するfunctionを返すunordered_map
	/// </summary>
	extern const std::unordered_map<Format, std::function<std::int32_t(std::int32_t, std::int32_t, std::int32_t)>> calcSizeMap;
}
