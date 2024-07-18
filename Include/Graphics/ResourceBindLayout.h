#pragma once
#include <cstdint>
#include <bitset>

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

	enum class ResourceBindFlag :
		std::uint32_t
	{
		Non = 0,

		/// <summary>
		/// ����(�ǂݍ��݂̂�)
		/// </summary>
		Input = 1,

		/// <summary>
		/// �X�g���[���p�o��(����������)
		/// </summary>
		StreamOutput = 2,
	};

	using ResourceBindFlags = std::bitset<4>;

	constexpr ResourceBindFlags operator|(ResourceBindFlag rflag, ResourceBindFlag lflag)
	{
		return ResourceBindFlags{ std::to_underlying(rflag) | std::to_underlying(lflag) };
	}

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