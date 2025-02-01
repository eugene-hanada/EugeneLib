#pragma once
#include "../Utils/Array.h"
#include <filesystem>
#include <span>

namespace Eugene
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
		/// バイトコードから直接構築
		/// </summary>
		/// <param name="byteCode"></param>
		Shader(std::span<const std::uint8_t> byteCode);

		/// <summary>
		/// 読み込んだシェーダーの先頭ポインタを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> 先頭ポインタ </returns>
		const char* GetPtr(void) const noexcept;

		/// <summary>
		/// シェーダーのサイズを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns> サイズ </returns>
		std::uint64_t GetSize(void) const noexcept;
	private:

		/// <summary>
		/// シェーダーコード
		/// </summary>
		Array<std::uint8_t> byteCode_;
	};
}
