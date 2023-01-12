#include <Windows.h>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <memory>
#include <Common/Debug.h>

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

// �s��f�[�^
std::unique_ptr <EugeneLib::GpuResource> upMatrixBuffer;
std::unique_ptr <EugeneLib::GpuResource> matrixBuffer;
std::unique_ptr < EugeneLib::ShaderResourceViews> matrixView_;

// �T�E���h�n
std::unique_ptr<EugeneLib::Wave> wave;
std::unique_ptr<EugeneLib::Sound> sound;
std::unique_ptr < EugeneLib::SoundControl> soundCtrl;
std::unique_ptr<EugeneLib::SoundSpeaker> soundSpeaker;

void Init(void)
{
	// Windows�Ƃ��̋@�\���܂Ƃ߂��N���X���쐬
	libSys.reset(EugeneLib::CreateSystem({ 1280.0f, 720.0f }, u8"sample"));

	{
		// �O���t�B�b�N�̋@�\�̃N���X���쐬���R�}���h���X�g�����s����N���X���Z�b�g
		EugeneLib::GpuEngine* tmp;
		graphics.reset(libSys->CreateGraphics(tmp,3));
		gpuEngien.reset(tmp);
	}

	// �R�}���h���X�g���쐬
	cmdList.reset(graphics->CreateCommandList());
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
		{EugeneLib::ShaderLayout{EugeneLib::ViewType::ConstantBuffer, 1,0}},
		{EugeneLib::ShaderLayout{EugeneLib::ViewType::Texture, 1,0}}
	};

	std::vector< EugeneLib::SamplerLayout> sampler
	{
		EugeneLib::SamplerLayout{}
	};

	gpipeLine.reset(graphics->CreateGraphicsPipeline(
		layout,
		shaders,
		rendertargets,
		EugeneLib::TopologyType::Triangle,
		false,
		shaderLayout,
		sampler
	));

}

struct Vertex
{
	EugeneLib::Vector2 pos;
	EugeneLib::Vector2 uv;
};


void InitVertex(void)
{
	Vertex vertex[4]
	{
		{{0.0f,0.0f},{0.0f,0.0f}},
		{{246.0f,0.0f},{1.0f,0.0f}},
		{{0.0f, 246.0f},{0.0f, 1.0f}},
		{{246.0f,246.0f},{1.0f, 1.0f}}
	};


	// CPU����A�N�Z�X�ł��郊�\�[�X���쐬

	upVertexBuffer.reset(graphics->CreateUploadableResource(sizeof(vertex)));
	auto ptr = upVertexBuffer->Map();
	std::copy(std::begin(vertex), std::end(vertex), reinterpret_cast<Vertex*>(ptr));
	upVertexBuffer->UnMap();

	// GPU�����ł����g���Ȃ����\�[�X���쐬
	vertexBuffer.reset(graphics->CreateDefaultResource(sizeof(vertex)));

	// ���_�r���[
	vertexView.reset(graphics->CreateVertexView(sizeof(Vertex), 4ull, *vertexBuffer));
}

void InitTexture(void)
{
	EugeneLib::Texture tex("./Logo.png");
	upTextureBuffer.reset(graphics->CreateUploadableTextureResource(tex));
	textureBuffer.reset(graphics->CreateTextureResource(tex.GetInfo()));
	textureView_.reset(graphics->CreateShaderResourceViews(1));
	textureView_->CreateTexture(*textureBuffer, 0);
}

void InitConstantBuffer(void)
{
	EugeneLib::Matrix4x4 matrix;
	EugeneLib::Get2DMatrix(matrix, { 1280.0f, 720.0f });
	upMatrixBuffer.reset(graphics->CreateUploadableResource(256));
	*static_cast<EugeneLib::Matrix4x4*>(upMatrixBuffer->Map()) = matrix;
	upMatrixBuffer->UnMap();

	matrixBuffer.reset(graphics->CreateDefaultResource(256));
	matrixView_.reset(graphics->CreateShaderResourceViews(1));
	matrixView_->CreateConstantBuffer(*matrixBuffer, 0);
	
}

void InitSound(void)
{
	sound.reset(EugeneLib::CreateSound());
	wave = std::make_unique<EugeneLib::Wave>(L"./exp.wav");
	soundSpeaker.reset(sound->CreateSoundSpeaker(*wave));
	soundCtrl.reset(sound->CreateSoundControl(48000, 2, 2));
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{

	Init();
	InitGraphicsPipeline();
	InitVertex();
	InitTexture();
	InitConstantBuffer();
	InitSound();

	cmdList->Begin();
	cmdList->Copy(*matrixBuffer, *upMatrixBuffer);
	cmdList->Copy(*vertexBuffer, *upVertexBuffer);
	cmdList->CopyTexture(*textureBuffer, *upTextureBuffer);
	cmdList->End();

	gpuEngien->Push(*cmdList);
	gpuEngien->Execute();
	gpuEngien->Wait();

	soundSpeaker->SetOutput(*soundCtrl);
	soundSpeaker->Play();
	std::vector<float> volumes{ 1.0f,0.0f };
	soundCtrl->SetPan(volumes);

	//soundCtrl->SetPan(volumes);
	float color[4]{ 0.0f,0.0f,0.0f,1.0f };
	std::uint8_t key[256];
	while (libSys->Update())
	{
		EugeneLib::System::Mouse mouse;
		libSys->GetMouse(mouse);
		//DebugLog(mouse.pos);
		if (soundSpeaker->IsEnd())
		{
			soundSpeaker->Play();
		}

		// �R�}���h�̊J�n
		cmdList->Begin();

		// �����_�[�^�[�Q�b�g�̃Z�b�g
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());

		// �����_�[�^�[�Q�b�g���N���A
		cmdList->ClearRenderTarget(graphics->GetViews(), color, graphics->GetNowBackBufferIndex());

		cmdList->SetGraphicsPipeline(*gpipeLine);

		cmdList->SetShaderResourceView(*matrixView_, 0, 0);

		cmdList->SetShaderResourceView(*textureView_, 0, 1);

		cmdList->SetScissorrect({ 0,0 }, { 1280, 720 });

		cmdList->SetViewPort({ 0.0f,0.0f }, { 1280.0f, 720.0f });

		cmdList->SetPrimitiveType(EugeneLib::PrimitiveType::TriangleStrip);

		cmdList->SetVertexView(*vertexView);

		cmdList->Draw(4);


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


		if (GetKeyboardState(key))
		{
			for (int i = 0; i < 256; i++)
			{
				if ((key[i] & 0x80))
				{
					DebugLog(i, u8"������܂���");
				}
			}
		}
	}

	
	return 0;
}