﻿#pragma once
#include <cstdint>
#include <span>

namespace Eugene
{
	class SoundControl;

	/// <summary>
	/// サウンド関係のベースクラス
	/// </summary>
	class SoundBase
	{
	public:
		virtual ~SoundBase() = default;

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
		float GetVolume(void) const noexcept
		{
			return volume_;
		}

		/// <summary>
		/// 出力チャンネルを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		std::uint16_t GetOutChannel(void) const noexcept
		{
			return outChannel_;
		}

		/// <summary>
		/// 入力チャンネルを取得する
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		std::uint16_t GetInChannel(void) const noexcept
		{
			return inChannel_;
		}

		/// <summary>
		/// サウンドコントロールクラスに出力する(出力先でコントロールできる)
		/// </summary>
		/// <param name="control"> サウンドコントロールクラス </param>
		virtual void SetOutput(SoundControl& control);

	protected:
		SoundBase();

        SoundBase(std::uint16_t inChannel, std::uint16_t outChannel);

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