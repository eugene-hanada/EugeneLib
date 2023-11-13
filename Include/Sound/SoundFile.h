#pragma once
#include <cstdint>
#include <filesystem>
#include "SoundCommon.h"

namespace Eugene
{
	/// <summary>
	/// サウンドファイルを読み込むためのクラス
	/// </summary>
	class SoundFile
	{
	public:
		static constexpr int fmt = std::endian::native == std::endian::big ? 'fmt ' : ' tmf';
		static constexpr int data = std::endian::native == std::endian::big ? 'data' : 'atad';

		struct RIFF
		{
			char id[4];
			unsigned int size;
			char type[4];
		};

		SoundFile(const std::filesystem::path& path);
		SoundFile(SoundFile&& soundFile);

		SoundFile& operator=(SoundFile&& soundFile);

		/// <summary>
		/// データへのポインタを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::uint8_t* GetDataPtr(void) const noexcept;

		/// <summary>
		/// データのバイト数を取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::uint64_t GetDataSize(void) const noexcept;

		/// <summary>
		/// フォーマットの取得
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const SoundFormat& GetFormat(void) const noexcept;

		/// <summary>
		/// フォーマットの拡張データを取得
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const SoundFormatEx& GetFormatExt(void) const noexcept;
	protected:

		bool LoadWave(const std::filesystem::path& path);

		bool LoadOggVorbis(const std::filesystem::path& path);

		/// <summary>
		/// データ部
		/// </summary>
		std::vector<std::uint8_t> data_;

		/// <summary>
		/// フォーマット
		/// </summary>
		SoundFormat format_;

		/// <summary>
		/// 拡張データ部分
		/// </summary>
		SoundFormatEx ex_;
	};
}