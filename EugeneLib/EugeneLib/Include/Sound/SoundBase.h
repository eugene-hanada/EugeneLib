#pragma once
#include <cstdint>
#include <span>

namespace EugeneLib
{
	class SoundBase
	{
	public:
		virtual ~SoundBase();

		/// <summary>
		/// 全体のボリュームをセットする
		/// </summary>
		/// <param name="volume"> ボリューム </param>
		virtual void SetVolume(float volume) = 0;

		/// <summary>
		/// パンニング用にボリュームをセットする
		/// </summary>
		/// <param name="volumes"></param>
		virtual void SetPan(std::span<float> volumes) = 0;

		/// <summary>
		/// 現在のボリュームを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		float GetVolume(void) const;

		/// <summary>
		/// 出力チャンネルを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		std::uint16_t GetOututChannel(void) const;

	protected:
		SoundBase();

		/// <summary>
		/// 現在のボリューム
		/// </summary>
		float volume_;

		/// <summary>
		/// 出力チャンネル
		/// </summary>
		std::uint16_t outChannel_;

		/// <summary>
		/// 入力チャンネル
		/// </summary>
		std::uint16_t inChannel_;
	};

}