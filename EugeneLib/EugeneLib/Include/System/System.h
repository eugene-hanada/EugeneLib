#pragma once
#include <string>
#include <string_view>
#include <array>
#include "../Math/Vector2.h"

namespace EugeneLib
{
	class Graphics;
	class GpuEngine;

	/// <summary>
	/// OS���̏���������N���X
	/// </summary>
	class System
	{
	public:
		using KeyData = std::array<bool, 256>;

		/// <summary>
		/// �}�E�X�̍\����
		/// </summary>
		struct Mouse
		{
			Vector2 pos;
			bool left = false;
			bool right = false;
			float wheel = 0.0f;
		};

		/// <summary>
		/// �L�[�{�[�h��ID
		/// </summary>
		enum class KeyID
		{
			TAB = 9,
			CAPS_LOCK = 240,
			LEFT_SHIFT = 160,
			LEFT_CTRL = 162,
			ESCAPE = 27,
			SPACE = 32,
			LEFT_ALT = 164,
			RIGHT_ALT = 165,
			BACK_SPACE = 8,
			RETURN = 13,
			RIGHT_SHIFT = 161,
			RIGHT_CTRL = 163,
			UP = 38,
			DOWN = 40,
			LEFT = 37,
			RIGHT = 39,
			NUM_LOCK = 144,
			F1 = 112,
			F2 = 113,
			F3 = 114,
			F4 = 115,
			F5 = 116,
			F6 = 117,
			F7 = 118,
			F8 = 119,
			F9 = 120,
			F10 = 121,
			F11 = 122,
			F12 = 123,
			NUMPAD_0 = 96,
			NUMPAD_1 = 97,
			NUMPAD_2 = 98,
			NUMPAD_3 = 99,
			NUMPAD_4 = 100,
			NUMPAD_5 = 101,
			NUMPAD_6 = 102,
			NUMPAD_7 = 103,
			NUMPAD_8 = 104,
			NUMPAD_9 = 105,
			A = 'A',
			B = 'B',
			C = 'C',
			D = 'D',
			E = 'E',
			F = 'F',
			G = 'G',
			H = 'H',
			I = 'I',
			J = 'J',
			K = 'K',
			L = 'L',
			M = 'M',
			N = 'N',
			O = 'O',
			P = 'P',
			Q = 'Q',
			R = 'R',
			S = 'S',
			T = 'T',
			U = 'U',
			V = 'V',
			W = 'W',
			X = 'X',
			Y = 'Y',
			Z = 'Z',
		};

		/// <summary>
		/// �X�V����(���t���[���ĂԕK�v������Ԓl�ŏI�����ׂ������A���Ă���)
		/// </summary>
		/// <param name=""></param>
		/// <returns> �p�����ׂ���true�A�I�����ׂ���false </returns>
		virtual bool Update(void) = 0;

		/// <summary>
		/// �f�X�g���N�^
		/// </summary>
		virtual ~System();

		/// <summary>
		/// �E�B���h�E�T�C�Y���擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> �E�B���h�E�T�C�Y </returns>
		const Vector2& GetWindowSize(void) const&;

		/// <summary>
		/// Graphics���𐶐�����
		/// </summary>
		/// <param name="gpuEngine"> GpuEngine�̃|�C���^�̎Q�� </param>
		/// <returns> Graphics�̃|�C���^ </returns>
		virtual Graphics* CreateGraphics(GpuEngine*& gpuEngine, size_t bufferNum = 2) const& = 0;

		/// <summary>
		/// �}�E�X�̏����擾����
		/// </summary>
		/// <param name="outMouse"></param>
		/// <returns></returns>
		virtual void GetMouse(Mouse& outMouse) const&;

		/// <summary>
		/// �L�[��������Ă��邩
		/// </summary>
		/// <param name="keyID"> ������Ă���L�[��ID </param>
		/// <returns> ������Ă���Ƃ�true�A������Ă��Ȃ��Ƃ�false </returns>
		virtual bool IsHitKey(KeyID keyID) const;

		/// <summary>
		/// �L�[���͏����擾����
		/// </summary>
		/// <param name="keySpan"> �L�[��� </param>
		/// <returns> �L�[��񂪂���ꍇ��true�A�Ȃ��ꍇ��false </returns>
		virtual bool GetKeyData(KeyData& keyData) const;
	protected:

		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		/// <param name="size"> �E�B���h�E�T�C�Y </param>
		/// <param name="title"> �E�B���h�E�^�C�g�� </param>
		System(const Vector2& size, const std::u8string& title);
		
		/// <summary>
		/// �E�B���h�E�T�C�Y
		/// </summary>
		Vector2 windowSize_;

		/// <summary>
		/// �^�C�g��
		/// </summary>
		std::u8string title_;
	private:
		System(const System&) = delete;
		System& operator=(const System&) = delete;

	};

	/// <summary>
	/// System�̍쐬
	/// </summary>
	/// <param name="size"> �E�B���h�E�T�C�Y </param>
	/// <param name="title"> �^�C�g�� </param>
	/// <returns></returns>
	System* CreateSystem(const Vector2& size, const std::u8string& title);
}
