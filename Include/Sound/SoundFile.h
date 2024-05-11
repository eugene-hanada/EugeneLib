#pragma once
#include <cstdint>
#include <filesystem>
#include <vector>
#include <bit>
#include "SoundCommon.h"

namespace Eugene
{
	/// <summary>
	/// サウンドファイル
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
		SoundFile(SoundFile&& soundFile) noexcept;

		SoundFile& operator=(SoundFile&& soundFile);

		/// <summary>
		/// データのポインタを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::uint8_t* GetDataPtr(void) const noexcept;

		/// <summary>
		/// データのサイズを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::uint64_t GetDataSize(void) const noexcept;

		/// <summary>
		/// フォーマットを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const SoundFormat& GetFormat(void) const noexcept;

		/// <summary>
		/// フォーマットの拡張部分を取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const SoundFormatEx& GetFormatExt(void) const noexcept;
	protected:

		/// <summary>
		/// waveファイルを読み込む
		/// </summary>
		/// <param name="path"> パス </param>
		/// <returns> 成功時true、失敗時false </returns>
		bool LoadWave(const std::filesystem::path& path);

		/// <summary>
		/// ogg vorbisファイルを読み込む
		/// </summary>
		/// <param name="path"> パス </param>
		/// <returns> 成功時true、失敗時false </returns>
		bool LoadOggVorbis(const std::filesystem::path& path);

		/// <summary>
		/// データ
		/// </summary>
		std::vector<std::uint8_t> data_;

		/// <summary>
		/// フォーマット
		/// </summary>
		SoundFormat format_;

		/// <summary>
		/// フォーマットの拡張部分
		/// </summary>
		SoundFormatEx ex_;
	};
}