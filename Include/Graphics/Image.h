#pragma once
#include <filesystem>
#include <vector>
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


		const std::vector<std::uint8_t>& GetData(void) const&;

		/// <summary>
		/// テクスチャデータを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> テクスチャデータ </returns>
		std::uint8_t* GetData(void)&;
	private:

		/// <summary>
		/// テクスチャデータ
		/// </summary>
		std::vector<std::uint8_t> data_;

		/// <summary>
		/// テクスチャ情報
		/// </summary>
		TextureInfo info_;
	};
}
