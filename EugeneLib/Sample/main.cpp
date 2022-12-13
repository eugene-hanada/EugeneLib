#include <Windows.h>
#include <EugeneLib.h>
#include <memory>

// �V�X�e���n(Windows�֘A�̏����̃N���X)
std::unique_ptr<EugeneLib::System> libSys;

// �O���t�B�b�N�X�̃N���X(�f�o�C�X�Ƃ��̏���������)
std::unique_ptr <EugeneLib::Graphics> graphics;

// �R�}���h���X�g�����s����N���X
std::unique_ptr < EugeneLib::GpuEngine> gpuEngien;

// �R�}���h���X�g
std::unique_ptr<EugeneLib::CommandList> cmdList;

// �O���t�B�b�N�p�C�v���C��(�����̗���)
std::unique_ptr<EugeneLib::GraphicsPipeline> gpipeLine;

// ���_�f�[�^
std::unique_ptr <EugeneLib::GpuResource> upVertexBuffer;
std::unique_ptr <EugeneLib::GpuResource> vertexBuffer;
std::unique_ptr<EugeneLib::VertexView> vertexView;

// �e�N�X�`���f�[�^
std::unique_ptr <EugeneLib::GpuResource> upTextureBuffer;
std::unique_ptr <EugeneLib::GpuResource> textureBuffer;
std::unique_ptr < EugeneLib::ShaderResourceViews> textureView_;



void Init(void)
{
	// Windows�Ƃ��̋@�\���܂Ƃ߂��N���X���쐬
	libSys.reset(EugeneLib::CreateSystem({ 1280.0f, 720.0f }, u8"sample"));

	{
		// �O���t�B�b�N�̋@�\�̃N���X���쐬���R�}���h���X�g�����s����N���X���Z�b�g
		EugeneLib::GpuEngine* tmp;
		graphics.reset(EugeneLib::CreateGraphics(*libSys, tmp));
		gpuEngien.reset(tmp);
	}

	// �R�}���h���X�g���쐬
	cmdList.reset(EugeneLib::CreateCommandList(*graphics));
}

void InitGraphicsPipeline(void)
{
	// ���_�V�F�[�_�̓��͂̃��C�A�E�g
	std::vector<EugeneLib::ShaderInputLayout> layout
	{
		{"POSITION", 0, EugeneLib::Format::R32G32_FLOAT},
		{"TEXCOORD", 0, EugeneLib::Format::R32G32_FLOAT}
	};

	// �V�F�[�_�[
	std::vector<std::pair<EugeneLib::Shader, EugeneLib::ShaderType>> shaders
	{
		{EugeneLib::Shader{"VertexShader.vso"}, EugeneLib::ShaderType::Vertex},
		{EugeneLib::Shader{"PixelShader.pso"}, EugeneLib::ShaderType::Pixel}
	};

	// �����_�[�^�[�Q�b�g
	std::vector<EugeneLib::RendertargetLayout> rendertargets
	{
		{EugeneLib::Format::R8G8B8A8_UNORM, EugeneLib::BlendType::Non}
	};

	std::vector<std::vector<EugeneLib::ShaderLayout>> shaderLayout
	{
		{EugeneLib::ShaderLayout{EugeneLib::ViewType::Texture, 0,0}}
	};

	std::vector< EugeneLib::SamplerLayout> sampler
	{
		EugeneLib::SamplerLayout{}
	};

	gpipeLine.reset(EugeneLib::CreateGraphicsPipeline(
		*graphics,
		layout,
		shaders,
		rendertargets,
		EugeneLib::TopologyType::Triangle,
		false,
		shaderLayout
	));

}

struct Vertex
{
	EugeneLib::Vector2 pos;
	EugeneLib::Vector2 uv;
};


void InitVertex(void)
{
	Vertex vertex[3]
	{
		{{ -0.5f, -0.7f}, {0.5f, 0.0f}},
		{{0.0f,0.7f},{1.0f,1.0f} },
		{{0.5f, -0.7f}, {0.0f,1.0f}}
	};


	// CPU����A�N�Z�X�ł��郊�\�[�X���쐬

	upVertexBuffer.reset(EugeneLib::CreateUploadableResource(sizeof(vertex), *graphics));
	auto ptr = upVertexBuffer->Map();
	std::copy(std::begin(vertex), std::end(vertex), reinterpret_cast<Vertex*>(ptr));
	upVertexBuffer->UnMap();

	// GPU�����ł����g���Ȃ����\�[�X���쐬
	vertexBuffer.reset(EugeneLib::CreateDefaultResource(sizeof(vertex), *graphics));

	// ���_�r���[
	vertexView.reset(EugeneLib::CreateVertexView(sizeof(Vertex), 3ull, *vertexBuffer));
}

void InitTexture(void)
{
	EugeneLib::Texture tex("./Logo.png");
	upTextureBuffer.reset(EugeneLib::CreateUploadableResource(tex, *graphics));
	textureBuffer.reset(EugeneLib::CreateTextureResource(tex.GetInfo(), *graphics));
	textureView_.reset(EugeneLib::CreateShaderResourceViews(*graphics, 1));
	textureView_->CreateTexture(*textureBuffer, 0);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{

	Init();
	InitGraphicsPipeline();
	InitVertex();
	InitTexture();

	cmdList->Begin();
	cmdList->Copy(*vertexBuffer, *upVertexBuffer);
	cmdList->CopyTexture(*textureBuffer, *upTextureBuffer);
	cmdList->End();

	gpuEngien->Push(*cmdList);
	gpuEngien->Execute();
	gpuEngien->Wait();

	

	float color[4]{ 0.0f,0.0f,0.0f,1.0f };
	while (libSys->Update())
	{
		// �R�}���h�̊J�n
		cmdList->Begin();

		// �����_�[�^�[�Q�b�g�̃Z�b�g
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());

		// �����_�[�^�[�Q�b�g���N���A
		cmdList->ClearRenderTarget(graphics->GetViews(), color, graphics->GetNowBackBufferIndex());

		cmdList->SetGraphicsPipeline(*gpipeLine);

		cmdList->SetShaderResourceView(*textureView_, 0, 0);

		cmdList->SetScissorrect({ 0,0 }, { 1280, 720 });

		cmdList->SetViewPort({ 0.0f,0.0f }, { 1280.0f, 720.0f });

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