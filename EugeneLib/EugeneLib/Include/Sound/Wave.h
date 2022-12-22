#pragma once
#include <filesystem>
#include <vector>
#include "SoundCommon.h"


namespace EugeneLib
{
	// Waveクラス
	class Wave
	{
	public:
		Wave(const std::filesystem::path& path);

		/// <summary>
		/// fmtの部分を取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const SoundFormat& GetFmt(void) const&
		{
			return fmt_;
		}

		/// <summary>
		/// データを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		const std::vector<unsigned char>& GetData(void) const&
		{
			return data_;
		}

	private:

		/// <summary>
		/// Fmtチャンクの読み込み
		/// </summary>
		/// <param name="file"></param>
		void LoadFmt(std::ifstream& file);

		/// <summary>
		/// データチャンクの読み込み
		/// </summary>
		/// <param name="file"></param>
		void LoadData(std::ifstream& file);


		// データ
		std::vector<unsigned char> data_;

		// fmt
		SoundFormat fmt_;

		// RiFFチャンク
		struct RIFF
		{
			char id[4];
			unsigned int size;
			char type[4];
		};

	};
}
