#include <Windows.h>
#include <EugeneLib.h>
#include <Graphics/Graphics.h>
#include <Graphics/GpuEngine.h>
#include <Graphics/GpuResource.h>
#include <Graphics/CommandList.h>
#include <Graphics/GraphicsPipeline.h>
#include <Math/Vector2.h>
#include <Graphics/Shader.h>
#include <Graphics/VertexView.h>
#include <memory>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{
	// Windows�Ƃ��̋@�\���܂Ƃ߂��N���X���쐬
	std::unique_ptr<EugeneLib::System>system;
	system.reset(EugeneLib::CreateSystem({ 1280.0f, 720.0f }, u8"sample"));

	std::unique_ptr < EugeneLib::GpuEngine> gpuEngien;
	std::unique_ptr <EugeneLib::Graphics> graphics;

	{
		// �O���t�B�b�N�̋@�\�̃N���X���쐬���R�}���h���X�g�����s����N���X���Z�b�g
		EugeneLib::GpuEngine* tmp;
		graphics.reset(EugeneLib::CreateGraphics(*system, tmp));
		gpuEngien.reset(tmp);
	}

	struct Vertex
	{
		EugeneLib::Vector2 pos;
		EugeneLib::Vector2 uv;
	};

	Vertex vertex[3]
	{
		{{ -0.5f, -0.7f}, {0.5f, 0.0f}},
		{{0.0f,0.7f},{1.0f,1.0f} },
		{{0.5f, -0.7f}, {0.0f,1.0f}}
	};

	// ���_�V�F�[�_�̓��͂̃��C�A�E�g
	std::vector<EugeneLib::ShaderInputLayout> layout
	{
		{"POSITION", 0, EugeneLib::Format::R32G32_FLOAT},
		{"TEXCOORD", 0, EugeneLib::Format::R32G32_FLOAT}
	};

	// �V�F�[�_�[
	std::vector<std::pair<EugeneLib::Shader,EugeneLib::ShaderType>> shaders
	{
		{EugeneLib::Shader{"VertexShader.vso"}, EugeneLib::ShaderType::Vertex},
		{EugeneLib::Shader{"PixelShader.pso"}, EugeneLib::ShaderType::Pixel}
	};

	// �����_�[�^�[�Q�b�g
	std::vector<EugeneLib::RendertargetLayout> rendertargets
	{
		{EugeneLib::Format::R8G8B8A8_UNORM, EugeneLib::BlendType::Non}
	};

	std::unique_ptr<EugeneLib::GraphicsPipeline> gpipeLine;
	gpipeLine.reset(EugeneLib::CreateGraphicsPipeline(*graphics, layout, shaders, rendertargets));


	// CPU����A�N�Z�X�ł��郊�\�[�X���쐬
	std::unique_ptr <EugeneLib::GpuResource> resource;
	resource.reset(EugeneLib::CreateUploadableResource(sizeof(vertex), *graphics));
	auto ptr = resource->Map();
	std::copy(std::begin(vertex), std::end(vertex), reinterpret_cast<Vertex*>(ptr));
	resource->UnMap();

	// GPU�����ł����g���Ȃ����\�[�X���쐬
	std::unique_ptr <EugeneLib::GpuResource> defResource;
	defResource.reset(EugeneLib::CreateDefaultResource(sizeof(vertex), *graphics));

	// ���_�r���[
	std::unique_ptr<EugeneLib::VertexView> vertexView;
	vertexView.reset(EugeneLib::CreateVertexView(sizeof(Vertex), 3ull, *defResource));

	// �R�}���h���X�g���쐬
	std::unique_ptr<EugeneLib::CommandList> cmdList;
	cmdList.reset(EugeneLib::CreateCommandList(*graphics));

	cmdList->Begin();
	cmdList->Copy(*defResource, *resource);
	cmdList->End();

	gpuEngien->Push(*cmdList);
	gpuEngien->Execute();
	gpuEngien->Wait();

	float color[4]{ 0.0f,0.0f,0.0f,1.0f };
	while (system->Update())
	{
		// �R�}���h�̊J�n
		cmdList->Begin();

		// �����_�[�^�[�Q�b�g�̃Z�b�g
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());

		// �����_�[�^�[�Q�b�g���N���A
		cmdList->ClearRenderTarget(graphics->GetViews(), color, graphics->GetNowBackBufferIndex());

		cmdList->SetGraphicsPipeline(*gpipeLine);

		cmdList->SetScissorrect({ 0,0 }, { 1280, 720 });

		cmdList->SetPrimitiveType(EugeneLib::PrimitiveType::Triangle);

		cmdList->SetVertexView(*vertexView);

		cmdList->Draw(3);


		cmdList->TransitionRenderTargetEnd(graphics->GetBackBufferResource());

		// �R�}���h�I��
		cmdList->End();

		// ���s����R�}���h��ǉ�
		gpuEngien->Push(*cmdList);

		// �R�}���h�����s
		gpuEngien->Execute();

		// �R�}���h���s��҂�
		gpuEngien->Wait();

		// �X�N���[�����o�b�N�o�b�t�@�ɓ���ւ�����
		graphics->Present();
	}

	
	return 0;
}