#pragma once
#include <cstdint>

namespace Eugene
{
	/// <summary>
/// �r���[�̃^�C�v
/// </summary>
	enum class ViewType
	{
		/// <summary>
		/// �e�N�X�`��
		/// </summary>
		Texture,

		/// <summary>
		/// UA
		/// </summary>
		UnoderedAccess,

		/// <summary>
		/// �萔�o�b�t�@
		/// </summary>
		ConstantBuffer,

		/// <summary>
		/// �T���v���[
		/// </summary>
		Sampler
	};

	struct Bind
	{
		ViewType viewType_{ViewType::ConstantBuffer};
		std::uint32_t viewNum_{1u};
	};

	/// <summary>
	/// �V�F�[�_�Ƀ��\�[�X���o�C���h����ۂ̃��C�A�E�g�������߂�N���X(GraphicsPipeline�����ɕK�v)
	/// </summary>
	class ResourceBindLayout
	{
	public:
		virtual ~ResourceBindLayout();
	private:
	};
}