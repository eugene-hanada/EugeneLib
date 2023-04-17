#pragma once
#include <filesystem>
#include <vector>
#include <unordered_map>
#include "GraphicsCommon.h"
#include "../Math/Vector2.h"

namespace Eugene
{
	/// <summary>
	/// テクスチャ
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

		void LoadData(void);

		class BinaryReader
		{
		public:
			BinaryReader(const std::filesystem::path& path);
			~BinaryReader();
			void Read(void* ptr, std::uint64_t size);
			bool IsOpen(void) const;
			FILE* GetFilePtr(void);
		private:
			FILE* file_;
		};
	private:

		using LoadFunc = bool(Image::*)(const std::filesystem::path&);

		using LoadFuncMap = std::unordered_map<std::string, LoadFunc>;

		

		/// <summary>
		/// stbライブラリを使用して読み込む
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		bool LoadStb(const std::filesystem::path& path);

		/// <summary>
		/// ddsファイルの読み込み
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		bool LoadDds(const std::filesystem::path& path);

		/// <summary>
		/// テクスチャデータ
		/// </summary>
		std::vector<std::vector<std::uint8_t>> data_;

		/// <summary>
		/// テクスチャ情報
		/// </summary>
		TextureInfo info_;

		BinaryReader br_;

		/// <summary>
		/// ロード用関数のmap
		/// </summary>
		static const LoadFuncMap loadFuncMap_;
	};
}
