#pragma once
#include <filesystem>
#include <vector>
#include <unordered_map>
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
		/// データ部分を読み込む
		/// </summary>
		/// <param name=""></param>
		void LoadData(void);

		/// <summary>
		/// 情報を読み込む
		/// </summary>
		/// <param name=""></param>
		void LoadInfo(void);

		/// <summary>
		/// すべて読み込む
		/// </summary>
		/// <param name=""></param>
		void Load(void);

		class BinaryReader
		{
		public:
			BinaryReader(BinaryReader&& br) noexcept;
			BinaryReader(const std::filesystem::path& path);
			~BinaryReader();
			BinaryReader& operator=(BinaryReader&& br) noexcept;
			void Read(void* ptr, std::uint64_t size);
			bool IsOpen(void) const;
			void Close(void);
			FILE* GetFilePtr(void);
		private:
			FILE* file_;
		};

		Image(Image&& img) noexcept;

		/// <summary>
		/// move演算子
		/// </summary>
		/// <param name="img"></param>
		/// <returns></returns>
		Image& operator=(Image&& img) noexcept;
	private:

		using LoadFunc = bool(Image::*)(BinaryReader&);
		using LoadFuncMap = std::unordered_map<std::uint64_t, std::pair<LoadFunc, LoadFunc>>;

		
		/// <summary>
		/// stbライブラリを使用して画像の情報を読み込む
		/// </summary>
		/// <param name="br"></param>
		/// <returns></returns>
		bool LoadStbInfo(BinaryReader& br);

		/// <summary>
		/// stbライブラリを使用して画像のデータを読み込む
		/// </summary>
		/// <param name="br"></param>
		/// <returns></returns>
		bool LoadStbData(BinaryReader& br);

		/// <summary>
		/// ddsファイルの情報を読み込む
		/// </summary>
		/// <param name="br"></param>
		/// <returns></returns>
		bool LoadDdsInfo(BinaryReader& br);

		/// <summary>
		/// ddsファイルのデータを読み込む
		/// </summary>
		/// <param name="br"></param>
		/// <returns></returns>
		bool LoadDdsData(BinaryReader& br);

		/// <summary>
		/// テクスチャデータ
		/// </summary>
		std::vector<std::vector<std::uint8_t>> data_;

		/// <summary>
		/// テクスチャ情報
		/// </summary>
		TextureInfo info_;

		/// <summary>
		/// バイナリリーダー
		/// </summary>
		BinaryReader br_;

		/// <summary>
		/// 拡張しタイプ
		/// </summary>
		std::uint64_t ext_;

		/// <summary>
		/// 情報をロードしたか？
		/// </summary>
		bool isInfoLoaded_;

		/// <summary>
		/// ロード用関数のmap
		/// </summary>
		static const LoadFuncMap loadFuncMap_;
	};
}
