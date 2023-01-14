#pragma once
#include <string>
#include <string_view>
#include <array>
#include <span>
#include "../Math/Vector2.h"
#include "KeyID.h"
#include "GamePad.h"

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

		/// <summary>
		/// �L�[�{�[�h�̏��()
		/// </summary>
		using KeyDataSpan = std::span<bool, KEYID_MAX>;

		/// <summary>
		/// �L�[�R�[�h�̃e�[�u��
		/// </summary>
		using KeyCodeTable = std::array<int, KEYID_MAX>;

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
		virtual bool GetKeyData(KeyDataSpan& keyData) const;

		/// <summary>
		/// �L�[�R�[�h�̃f�[�^�e�[�u��
		/// </summary>
		/// <param name="keyCodeTable"> �Z�b�g����L�[�R�[�h </param>
		/// <returns> ������true�A���s��false </returns>
		virtual bool SetKeyCodeTable(KeyCodeTable& keyCodeTable);

		/// <summary>
		/// �Q�[���p�b�h���擾����
		/// </summary>
		/// <param name="pad"> �Q�[���p�b�h </param>
		/// <param name="idx"></param>
		/// <returns></returns>
		virtual bool GetGamePad(GamePad& pad, std::uint32_t idx) const;
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
