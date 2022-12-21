#pragma once
#include <vector>
#include <filesystem>

namespace EugeneLib
{
	/// <summary>
	/// シェーダー
	/// </summary>
	class Shader
	{
	public:

		/// <summary>
		/// コンストラクタ(指定したパスのシェーダーファイルを読み込む)
		/// </summary>
		/// <param name="path"> シェーダーファイルのパス </param>
		Shader(const std::filesystem::path& path);

		/// <summary>
		/// 読み込んだシェーダーの先頭ポインタを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> 先頭ポインタ </returns>
		const char* GetPtr(void) const;

		/// <summary>
		/// シェーダーのサイズを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> サイズ </returns>
		size_t GetSize(void) const;
	private:

		/// <summary>
		/// シェーダーコード
		/// </summary>
		std::vector<char> code_;
	};
}
