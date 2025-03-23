#pragma once
#include <string>
#include <string_view>
#include <array>
#include <span>
#include <bitset>
#include <filesystem>
#include "../ThirdParty/glm/glm/vec2.hpp"
#include "KeyID.h"
#include "GamePad.h"

struct ImGuiContext;

#ifdef EUGENE_WINDOWS
#include "Windows/WindowsSystem.h"
#elif EUGENE_ANDROID
#include "Android/AndroidSystem.h"
#endif

namespace Eugene
{
	class Graphics;
	class GpuEngine;
	class DynamicLibrary;

	/// <summary>
	/// マウスの構造体
	/// </summary>
	struct Mouse
	{
	public:
		/// <summary>
		/// マウス用フラグ
		/// </summary>
		enum class Flags
		{
			/// <summary>
			/// 左クリック
			/// </summary>
			LeftButton,

			/// <summary>
			/// 右クリック
			/// </summary>
			RightButton,

			/// <summary>
			/// ホイール
			/// </summary>
			CenterButton,

			/// <summary>
			/// 追加ボタン1
			/// </summary>
			Other1Button,

			/// <summary>
			/// 追加ボタン2
			/// </summary>
			Other2Button,

			/// <summary>
			/// カーソル表示フラグ
			/// </summary>
			ShowCursor,
			Max
		};

		Mouse();

		/// <summary>
		/// 指定のフラグがtrueかチェックする
		/// </summary>
		/// <param name="flag"> チェックしたいフラグ </param>
		/// <returns> 結果 </returns>
		bool CheckFlags(Flags flag) const
		{
			return flags.test(static_cast<size_t>(flag));
		}

		/// <summary>
		/// マウス座標
		/// </summary>
		glm::vec2 pos;

		/// <summary>
		/// ホイール
		/// </summary>
		float wheel = 0.0f;

		/// <summary>
		/// フラグ
		/// </summary>
		std::bitset<static_cast<size_t>(Flags::Max)> flags;
	private:
	};

	struct TouchData
	{
	public:
		struct Touch
		{
			Touch();
			glm::vec2 pos;
			float nowTime;
			float downTime;
		};

		TouchData();

		std::uint32_t touchCount_;
		std::array<Touch, 8> touchList_;
	private:
	};
}
