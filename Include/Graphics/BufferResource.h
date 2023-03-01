#pragma once
#include "GpuResource.h"

namespace Eugene
{
	/// <summary>
	/// �o�b�t�@�p���\�[�X
	/// </summary>
	class BufferResource :
		public GpuResource
	{
	public:
		virtual ~BufferResource();

		/// <summary>
		/// �T�C�Y���擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> �T�C�Y(�o�C�g��) </returns>
		virtual std::uint64_t GetSize(void) = 0;
	protected:
		BufferResource();
	private:

	};

}