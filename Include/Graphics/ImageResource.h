#pragma once
#include "GpuResource.h"
#include "GraphicsCommon.h"
#include "../ThirdParty/glm/glm/vec2.hpp"

namespace Eugene
{
	/// <summary>
	/// �e�N�X�`���⃌���_�[�^�[�Q�b�g���Ɏg�����\�[�X
	/// </summary>
	class ImageResource :
		public GpuResource
	{
	public:
		virtual ~ImageResource();

		/// <summary>
		/// �t�H�[�}�b�g�̎擾
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		Format GetFormat(void) const { return format_; }

		/// <summary>
		/// 
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual glm::ivec2 GetSize(void) = 0;
	protected:
		ImageResource(Format format);

		/// <summary>
		/// �t�H�[�}�b�g
		/// </summary>
		Format format_;
	private:
	};
}
