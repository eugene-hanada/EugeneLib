#pragma once
#include <filesystem>
#include <memory>
#include "SoundCommon.h"

namespace Eugene
{
	/// <summary>
	/// サウンドファイルのストリーム
	/// </summary>
	class SoundStreamFile
	{
	public:
		virtual ~SoundStreamFile();

		/// <summary>
		/// フォーマットを取得
		/// </summary>
		/// <returns></returns>
		const SoundFormat& GetFormat() const;

		/// <summary>
		/// フォーマットの拡張部分を取得
		/// </summary>
		/// <returns></returns>
		const SoundFormatEx& GetFormatEx() const;

		/// <summary>
		/// データの総サイズを取得する
		/// </summary>
		/// <returns></returns>
		const std::uint64_t GetDataSize() const;

		/// <summary>
		/// 読み取りを行ったサイズ
		/// </summary>
		/// <returns></returns>
		const std::uint64_t GetReadSize() const;

		/// <summary>
		/// 読み取り処理をする
		/// </summary>
		/// <param name="ptr"> 書き込み先ポインタ </param>
		/// <param name="size"> 読み込みサイズ </param>
		virtual void Read(std::uint8_t* ptr, std::uint64_t size) = 0;

		/// <summary>
		/// 先頭位置へシークする
		/// </summary>
		virtual void SeekStart() = 0;
	protected:
		
		/// <summary>
		/// コンストラクタ
		/// </summary>
		SoundStreamFile();

		/// <summary>
		/// フォーマット
		/// </summary>
		SoundFormat format_;

		/// <summary>
		/// フォーマットの拡張部
		/// </summary>
		SoundFormatEx formatEx_;

		/// <summary>
		/// データの総サイズ
		/// </summary>
		std::uint64_t dataSize_;

		/// <summary>
		/// 読み取ったサイズ
		/// </summary>
		std::uint64_t readSize_;
	};

	/// <summary>
	/// 指定のファイル
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	std::unique_ptr<SoundStreamFile> CreateSoundStreamFile(const std::filesystem::path& path);
}
