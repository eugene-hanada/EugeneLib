#pragma once

namespace Eugene
{
	class Graphics;
	class CommandList;

	/// <summary>
	/// CommandList�N���X�����s�Ȃǂ̏������s���N���X
	/// </summary>
	class GpuEngine
	{
	public:

		/// <summary>
		/// �f�X�g���N�^
		/// </summary>
		virtual ~GpuEngine();

		/// <summary>
		/// Push����CommandList�����s����
		/// </summary>
		/// <param name=""></param>
		virtual void Execute(void) = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name=""></param>
		virtual void Wait(void) = 0;

		/// <summary>
		/// ���s����R�}���h���X�g��ǉ�����
		/// </summary>
		/// <param name="commandList"> �R�}���h���X�g </param>
		virtual void Push(CommandList& commandList) = 0;

		virtual void* GetQueue(void) const = 0;
	protected:

		/// <summary>
		/// �R�}���h���X�g
		/// </summary>
		GpuEngine();
	};
}