#pragma once
#include <string>
#include <string_view>
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
		virtual Graphics* CreateGraphics(GpuEngine*& gpuEngine) const& = 0;

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
