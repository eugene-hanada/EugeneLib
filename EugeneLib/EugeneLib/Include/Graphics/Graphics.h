#pragma once
#include "GraphicsPipeline.h"
#include "GraphicsCommon.h"
#include "../Math/Vector2.h"

namespace Eugene
{
	class System;
	class GpuEngine;
	class RenderTargetViews;
	class CommandList;
	class GpuResource;
	class Texture;
	class ShaderResourceViews;
	class DepthStencilViews;
	class VertexView;

	/// <summary>
	/// �O���t�B�b�N�X�̏������s���N���X
	/// </summary>
	class Graphics
	{
	public:
		virtual ~Graphics();

		/// <summary>
		/// GpuEngine�̐���
		/// </summary>
		/// <param name="maxSize"> �����ł���R�}���h���X�g�̍ő吔 </param>
		/// <returns> GpuEngine�̃|�C���^ </returns>
		virtual GpuEngine* CreateGpuEngine(size_t maxSize) const = 0;

		/// <summary>
		/// CommandList�̐���
		/// </summary>
		/// <param name=""></param>
		/// <returns> CommandList�̃|�C���^ </returns>
		virtual CommandList* CreateCommandList(void) const = 0;

		/// <summary>
		/// GraphicsPipeline�̐���
		/// </summary>
		/// <param name="layout"> �V�F�[�_�[�̓��̓��C�A�E�g </param>
		/// <param name="shaders"> �V�F�[�_�[ </param>
		/// <param name="rendertarges"> �����_�[�^�[�Q�b�g�̐ݒ� </param>
		/// <param name="topologyType"> �g�|���W�[�ݒ� </param>
		/// <param name="isCulling"> �J�����O���s����(�f�t�H���g�ł͍s��Ȃ�) </param>
		/// <param name="shaderLayout"> �V�F�[�_�[�Ŏg���萔�o�b�t�@���̏�� </param>
		/// <param name="samplerLayout"> �T���v���[�̏�� </param>
		/// <returns></returns>
		virtual GraphicsPipeline* CreateGraphicsPipeline(
			ShaderInputSpan layout,
			ShaderTypePaisrSpan  shaders,
			RenderTargetSpan rendertarges,
			TopologyType topologyType = TopologyType::Triangle,
			bool isCulling = false,
			ShaderLayoutSpan shaderLayout = ShaderLayoutSpan{},
			SamplerSpan samplerLayout = SamplerSpan{}
		) const = 0;
		
		/// <summary>
		/// �A�b�v���[�h�p��GpuResource�̐���
		/// </summary>
		/// <param name="size"> ���\�[�X�̃T�C�Y </param>
		/// <returns> GpuResource�̃|�C���^ </returns>
		virtual GpuResource* CreateUploadableResource(size_t size) const = 0;

		/// <summary>
		/// �e�N�X�`���̃A�b�v���[�h�pGpuResource�̐���
		/// </summary>
		/// <param name="texture"> �e�N�X�`�� </param>
		/// <returns> GpuResource�̃|�C���^ </returns>
		virtual GpuResource* CreateUploadableTextureResource(Texture& texture) const = 0;

		/// <summary>
		/// GpuResource�̐���
		/// </summary>
		/// <param name="size"> ���\�[�X�̃T�C�Y </param>
		/// <returns> GpuResource�̃|�C���^ </returns>
		virtual GpuResource* CreateDefaultResource(size_t size) const = 0;

		/// <summary>
		/// �e�N�X�`���pGpuResource�̐���
		/// </summary>
		/// <param name="formatData"> �e�N�X�`���̃t�H�[�}�b�g </param>
		/// <returns> GpuResource�̃|�C���^ </returns>
		virtual GpuResource* CreateTextureResource(const TextureInfo& formatData) const = 0;

		/// <summary>
		/// �����_�[�^�[�Q�b�g�pGpuResource�̐���
		/// </summary>
		/// <param name="size"> �����_�[�^�[�Q�b�g�̃T�C�Y </param>
		/// <param name="format"> �t�H�[�}�b�g </param>
		/// <param name="clearColor"> �������J���[ </param>
		/// <returns> GpuResource�̃|�C���^ </returns>
		virtual GpuResource* CreateRenderTargetResource(const Vector2& size, Format format, const std::span<float, 4>& clearColor) const = 0;

		/// <summary>
		/// �[�x�o�b�t�@�pGpuResource�̐���
		/// </summary>
		/// <param name="size"> �T�C�Y </param>
		/// <param name="format"> �t�H�[�}�b�g </param>
		/// <param name="clearValue"> �������̒l </param>
		/// <returns> GpuResource�̃|�C���^ </returns>
		virtual GpuResource* CreateDepthResource(const Vector2& size, Format format, float clearValue = 1.0f) const = 0;

		/// <summary>
		/// ShaderResourceViews�̐���
		/// </summary>
		/// <param name="size"> View�̐� </param>
		/// <returns> ShaderResourceViews�̃|�C���^ </returns>
		virtual ShaderResourceViews* CreateShaderResourceViews(size_t size) const = 0;

		/// <summary>
		/// RenderTargetViews�̐���
		/// </summary>
		/// <param name="size"> View�̐� </param>
		/// <param name="isShaderVisible"> �V�F�[�_�[�Ɍ������Ԃɂ��邩? </param>
		/// <returns> RenderTargetViews�̃|�C���^ </returns>
		virtual RenderTargetViews* CreateRenderTargetViews(size_t size, bool isShaderVisible) const = 0;

		/// <summary>
		/// DepthStencilViews�̐���
		/// </summary>
		/// <param name="size"> View�̐� </param>
		/// <returns> DepthStencilViews�̃|�C���^ </returns>
		virtual DepthStencilViews* CreateDepthStencilViews(size_t size) const = 0;

		/// <summary>
		/// VertexView�̐���
		/// </summary>
		/// <param name="size"> 1���_������̃T�C�Y </param>
		/// <param name="vertexNum"> ���_�� </param>
		/// <param name="resource"> ���_��GpuResource </param>
		/// <returns> VertexView�̃|�C���^ </returns>
		virtual VertexView* CreateVertexView(size_t size, size_t vertexNum, GpuResource& resource) const = 0;

		/// <summary>
		/// �o�b�N�o�b�t�@�̃����_�[�^�[�Q�b�g��GpuResource���擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> GpuResource�̎Q�� </returns>
		virtual GpuResource& GetBackBufferResource(void) = 0;

		/// <summary>
		/// �o�b�N�o�b�t�@�Ŏg�p����RenderTargetViews�̎擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> RenderTargetViews </returns>
		virtual RenderTargetViews& GetViews(void) = 0;

		/// <summary>
		/// ���݂̃o�b�N�o�b�t�@�̃C���f�b�N�X���擾����
		/// </summary>
		/// <param name=""></param>
		/// <returns> �C���f�b�N�X </returns>
		virtual size_t GetNowBackBufferIndex(void) = 0;
		virtual void Present(void) = 0;
	protected:
		Graphics();

		/// <summary>
		/// �X���b�v�`�F�C���p��GpuResource�̐���
		/// </summary>
		/// <param name="idx"> �o�b�N�o�b�t�@�̃C���f�b�N�X </param>
		/// <returns> GpuResource�̃|�C���^ </returns>
		virtual GpuResource* CreateSwapChainResource(std::uint32_t idx) const = 0;
	};
}
