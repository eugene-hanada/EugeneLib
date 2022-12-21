#pragma once
#include <cstdint>
#include <span>
#include "GraphicsCommon.h"
#include "../Math/Vector2.h"

namespace EugeneLib
{
	class Graphics;
	class RenderTargetViews;
	class DepthStencilViews;
	class VertexView;
	class GpuResource;
	class GraphicsPipeline;
	class ShaderResourceViews;

	/// <summary>
	/// �R�}���h���X�g�̏������s���N���X
	/// </summary>
	class CommandList
	{
	public:

		/// <summary>
		/// �f�X�g���N�^
		/// </summary>
		virtual ~CommandList();

		/// <summary>
		/// �R�}���h���X�g���J�n����
		/// </summary>
		/// <param name=""></param>
		virtual void Begin(void) = 0;

		/// <summary>
		/// �R�}���h���X�g���I������
		/// </summary>
		/// <param name=""></param>
		virtual void End(void) = 0;

		/// <summary>
		/// �O���t�B�b�N�X�p�C�v���C�����Z�b�g����
		/// </summary>
		/// <param name="gpipeline"> �O���t�B�b�N�X�p�C�v���C�� </param>
		virtual void SetGraphicsPipeline(GraphicsPipeline& gpipeline) = 0;

		/// <summary>
		/// �v���~�e�B�u�^�C�v���Z�b�g����
		/// </summary>
		/// <param name="type"> �v���~�e�B�u�^�C�v </param>
		virtual void SetPrimitiveType(PrimitiveType type) = 0;

		/// <summary>
		/// �V�U�[���N�g���Z�b�g����
		/// </summary>
		/// <param name="leftTop"> ���� </param>
		/// <param name="rightBottom"> �E�� </param>
		virtual void SetScissorrect(const Vector2I& leftTop, const Vector2I& rightBottom) = 0;

		/// <summary>
		/// �r���[�|�[�g���Z�b�g����
		/// </summary>
		/// <param name="leftTop"> ���� </param>
		/// <param name="size"> �T�C�Y </param>
		/// <param name="depthMin"> �[�x�̍ŏ��l(�f�t�H���g0.0f) </param>
		/// <param name="depthMax"> �[�x�̍ő�l(�f�t�H���g1.0f) </param>
		virtual void SetViewPort(const Vector2& leftTop, const Vector2& size, float depthMin = 0.0f, float depthMax = 1.0f) = 0;
		
		/// <summary>
		/// ���_�r���[�̃Z�b�g
		/// </summary>
		/// <param name="view"> ���_�r���[ </param>
		virtual void SetVertexView(VertexView& view) = 0;

		/// <summary>
		/// �V�F�[�_�[���\�[�X���Z�b�g����
		/// </summary>
		/// <param name="views"> �V�F�[�_�[���\�[�X�̃r���[ </param>
		/// <param name="viewsIdx"> �r���[�̃C���f�b�N�X </param>
		/// <param name="paramIdx"> �Z�b�g����C���f�b�N�X </param>
		virtual void SetShaderResourceView(ShaderResourceViews& views, size_t viewsIdx, size_t paramIdx) = 0;

		/// <summary>
		/// �`�悷��
		/// </summary>
		/// <param name="vertexCount"> ���_�� </param>
		/// <param name="instanceCount"> �C���X�^���X�� </param>
		virtual void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount = 1) = 0;

		/// <summary>
		/// �����_�[�^�[�Q�b�g���Z�b�g����
		/// </summary>
		/// <param name="views"> �����_�[�^�[�Q�b�g�̃r���[ </param>
		/// <param name="idx"> �r���[�̃C���f�b�N�X </param>
		virtual void SetRenderTarget(RenderTargetViews& views, size_t idx = 0) = 0;

		/// <summary>
		/// �����_�[�^�[�Q�b�g���Z�b�g����(RenderTargetViews�ɂ�����̂��ׂĂ��Z�b�g����)
		/// </summary>
		/// <param name="views"> �����_�[�^�[�Q�b�g�r���[ </param>
		virtual void SetRenderTarget(RenderTargetViews& views) = 0;

		/// <summary>
		/// �����_�[�^�[�Q�b�g���Z�b�g����(�[�x�o�b�t�@���Z�b�g����)
		/// </summary>
		/// <param name="renderTargetViews"> �����_�[�^�[�Q�b�g�r���[ </param>
		/// <param name="depthViews"> �f�v�X�X�e���V���r���[ </param>
		/// <param name="rtViewsIdx"> �����_�[�^�[�Q�b�g�r���[�̃C���f�b�N�X </param>
		/// <param name="dsViewsIdx"> �f�v�X�X�e���V���r���[ �̃C���f�b�N�X </param>
		virtual void SetRenderTarget(RenderTargetViews& renderTargetViews, DepthStencilViews& depthViews, size_t rtViewsIdx = 0, size_t dsViewsIdx = 0) = 0;

		/// <summary>
		/// �����_�[�^�[�Q�b�g���N���A����
		/// </summary>
		/// <param name="views"> �����_�[�^�[�Q�b�g�r���[ </param>
		/// <param name="color"> �N���A����J���[ </param>
		/// <param name="idx"> �����_�[�^�[�Q�b�g�r���[�̃C���f�b�N�X </param>
		virtual void ClearRenderTarget(RenderTargetViews& views, std::span<float,4> color, size_t idx = 0) = 0;

		/// <summary>
		///  �����_�[�^�[�Q�b�g���N���A����(RenderTargetViews�ɂ�����̂��ׂăN���A����)
		/// </summary>
		/// <param name="views"> �����_�[�^�[�Q�b�g�r���[ </param>
		/// <param name="color"> �N���A����J���[ </param>
		virtual void ClearRenderTarget(RenderTargetViews& views, std::span<float, 4> color) = 0;

		/// <summary>
		/// �����_�[�^�[�Q�b�g�̎g�p���J�n�ł����Ԃɂ���
		/// </summary>
		/// <param name="resource"> �����_�[�^�[�Q�b�g </param>
		virtual void TransitionRenderTargetBegin(GpuResource& resource) = 0;

		/// <summary>
		/// �����_�[�^�[�Q�b�g�̎g�p���I���̏�Ԃɂ���
		/// </summary>
		/// <param name="resource"> �����_�[�^�[�Q�b�g </param>
		virtual void TransitionRenderTargetEnd(GpuResource& resource) = 0;

		/// <summary>
		/// �[�x�o�b�t�@���N���A����
		/// </summary>
		/// <param name="views"> �f�v�X�X�e���V���r���[ </param>
		/// <param name="clearValue"> �N���A����l </param>
		/// <param name="idx"> �f�v�X�X�e���V���r���[�̃C���f�b�N�X </param>
		virtual void ClearDepth(DepthStencilViews& views, float clearValue = 1.0f, size_t idx = 0) = 0;

		/// <summary>
		/// ���\�[�X���R�s�[����
		/// </summary>
		/// <param name="destination"> �R�s�[�� </param>
		/// <param name="source"> �R�s�[�� </param>
		virtual void Copy(GpuResource& destination, GpuResource& source) = 0;

		/// <summary>
		/// �e�N�X�`���̃R�s�[������
		/// </summary>
		/// <param name="destination"> �R�s�[�� </param>
		/// <param name="source"> �R�s�[�� </param>
		virtual void CopyTexture(GpuResource& destination, GpuResource& source) = 0;

		virtual void* GetCommandList(void) const = 0;
	};
}
