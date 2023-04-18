#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include "../../Include/Sound/SoundFile.h"


namespace Eugene
{
	// Waveクラス
	class Wave :
		public SoundFile
	{
	public:
		static constexpr int fmt = std::endian::native == std::endian::big ? 'fmt ' : ' tmf';
		static constexpr int data = std::endian::native == std::endian::big ? 'data' : 'atad';


		Wave(const std::filesystem::path& path);
		
		~Wave();

		///// <summary>
		///// fmtの部分を取得する
		///// </summary>
		///// <param name=""></param>
		///// <returns></returns>
		//const SoundFormat& GetFmt(void) const&
		//{
		//	return fmt_;
		//}

		///// <summary>
		///// データを取得する
		///// </summary>
		///// <param name=""></param>
		///// <returns></returns>
		//const std::vector<std::uint8_t>& GetData(void) const&
		//{
		//	return data_;
		//}

		//const SoundFormatEx& GetEx(void) const&
		//{
		//	return exData_;
		//}

		// RiFFチャンク
		struct RIFF
		{
			char id[4];
			unsigned int size;
			char type[4];
		};

	private:
		void LoadFormat(void) final;

		void LoadData(void) final;

		void Close(void) final;

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


		//// データ
		//std::vector<std::uint8_t> data_;

		//// fmt
		//SoundFormat fmt_;

		//SoundFormatEx exData_;

		std::ifstream file_;

		bool isFormatLoaded_;
	};
}
