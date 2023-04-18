#pragma once
#include <cstdint>
#include <filesystem>
#include "SoundCommon.h"

namespace Eugene
{
	class SoundFile
	{
	public:
		virtual ~SoundFile();
		/// <summary>
		/// フォーマット部分を読み込みます
		/// </summary>
		/// <param name=""></param>
		virtual void LoadFormat(void) = 0;

		/// <summary>
		/// データ部分を読み込みます
		/// </summary>
		/// <param name=""></param>
		virtual void LoadData(void) = 0;

		/// <summary>
		/// ファイルを閉じる
		/// </summary>
		/// <param name=""></param>
		virtual void Close(void) = 0;

		/// <summary>
		/// データへのポインタを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::uint8_t* GetDataPtr(void) const;

		/// <summary>
		/// データのバイト数を取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::uint64_t GetDataSize(void) const;

		const SoundFormat& GetFormat(void) const;

		const SoundFormatEx& GetFormatExt(void) const;
	protected:
		SoundFile();
		std::vector<std::uint8_t> data_;
		SoundFormat format_;
		SoundFormatEx ex_;
	};

	/// <summary>
	/// サウンドファイルを開きインターフェースクラスを生成する
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	SoundFile* OpenSoundFile(const std::filesystem::path& path);
}