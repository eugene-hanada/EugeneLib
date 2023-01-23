#pragma once
#include <cstdint>

namespace Eugene
{
	/// <summary>
	/// �O���t�B�b�N�X���Ŏg�����\�[�X
	/// </summary>
	class GpuResource
	{
	public:

		/// <summary>
		/// �f�X�g���N�^
		/// </summary>
		virtual ~GpuResource();

		/// <summary>
		/// ���\�[�X���}�b�v����
		/// </summary>
		/// <param name=""></param>
		/// <returns> �|�C���^ </returns>
		virtual void* Map(void);

		/// <summary>
		/// ���\�[�X���A���}�b�v����
		/// </summary>
		/// <param name=""></param>
		virtual void UnMap(void);
		virtual void* GetResource(void) const= 0;
	protected:
		GpuResource();
	};

}
