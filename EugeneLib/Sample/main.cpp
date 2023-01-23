#include <Windows.h>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <memory>
#include <Common/Debug.h>

// �V�X�e���n(Windows�֘A�̏����̃N���X)
std::unique_ptr<Eugene::System> libSys;

// �O���t�B�b�N�X�̃N���X(�f�o�C�X�Ƃ��̏���������)
std::unique_ptr <Eugene::Graphics> graphics;

// �R�}���h���X�g�����s����N���X
std::unique_ptr < Eugene::GpuEngine> gpuEngien;

// �R�}���h���X�g
std::unique_ptr<Eugene::CommandList> cmdList;

// �O���t�B�b�N�p�C�v���C��(�����̗���)
std::unique_ptr<Eugene::GraphicsPipeline> gpipeLine;

// ���_�f�[�^
std::unique_ptr <Eugene::GpuResource> upVertexBuffer;
std::unique_ptr <Eugene::GpuResource> vertexBuffer;
std::unique_ptr<Eugene::VertexView> vertexView;

// �e�N�X�`���f�[�^
std::unique_ptr <Eugene::GpuResource> upTextureBuffer;
std::unique_ptr <Eugene::GpuResource> textureBuffer;
std::unique_ptr < Eugene::ShaderResourceViews> textureView_;

// �s��f�[�^
std::unique_ptr <Eugene::GpuResource> upMatrixBuffer;
std::unique_ptr <Eugene::GpuResource> matrixBuffer;
std::unique_ptr < Eugene::ShaderResourceViews> matrixView_;

// �T�E���h�n
std::unique_ptr<Eugene::Wave> wave;
std::unique_ptr<Eugene::Sound> sound;
std::unique_ptr < Eugene::SoundControl> soundCtrl;
std::unique_ptr<Eugene::Sound3DControl> sound3DCtrl;
std::unique_ptr<Eugene::SoundSpeaker> soundSpeaker;

void Init(void)
{
	// Windows�Ƃ��̋@�\���܂Ƃ߂��N���X���쐬
	libSys.reset(Eugene::CreateSystem({ 1280.0f, 720.0f }, u8"sample"));

	{
		// �O���t�B�b�N�̋@�\�̃N���X���쐬���R�}���h���X�g�����s����N���X���Z�b�g
		Eugene::GpuEngine* tmp;
		graphics.reset(libSys->CreateGraphics(tmp,3));
		gpuEngien.reset(tmp);
	}

	// �R�}���h���X�g���쐬
	cmdList.reset(graphics->CreateCommandList());
}

void InitGraphicsPipeline(void)
{
	// ���_�V�F�[�_�̓��͂̃��C�A�E�g
	std::vector<Eugene::ShaderInputLayout> layout
	{
		{"POSITION", 0, Eugene::Format::R32G32_FLOAT},
		{"TEXCOORD", 0, Eugene::Format::R32G32_FLOAT}
	};

	// �V�F�[�_�[
	std::vector<std::pair<Eugene::Shader, Eugene::ShaderType>> shaders
	{
		{Eugene::Shader{"VertexShader.vso"}, Eugene::ShaderType::Vertex},
		{Eugene::Shader{"PixelShader.pso"}, Eugene::ShaderType::Pixel}
	};

	// �����_�[�^�[�Q�b�g
	std::vector<Eugene::RendertargetLayout> rendertargets
	{
		{Eugene::Format::R8G8B8A8_UNORM, Eugene::BlendType::Non}
	};

	std::vector<std::vector<Eugene::ShaderLayout>> shaderLayout
	{
		{Eugene::ShaderLayout{Eugene::ViewType::ConstantBuffer, 1,0}},
		{Eugene::ShaderLayout{Eugene::ViewType::Texture, 1,0}}
	};

	std::vector< Eugene::SamplerLayout> sampler
	{
		Eugene::SamplerLayout{}
	};

	gpipeLine.reset(graphics->CreateGraphicsPipeline(
		layout,
		shaders,
		rendertargets,
		Eugene::TopologyType::Triangle,
		false,
		shaderLayout,
		sampler
	));

}

struct Vertex
{
	Eugene::Vector2 pos;
	Eugene::Vector2 uv;
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
	Eugene::Texture tex("./Logo.png");
	upTextureBuffer.reset(graphics->CreateUploadableTextureResource(tex));
	textureBuffer.reset(graphics->CreateTextureResource(tex.GetInfo()));
	textureView_.reset(graphics->CreateShaderResourceViews(1));
	textureView_->CreateTexture(*textureBuffer, 0);
}

void InitConstantBuffer(void)
{
	Eugene::Matrix4x4 matrix;
	Eugene::Get2DMatrix(matrix, { 1280.0f, 720.0f });
	upMatrixBuffer.reset(graphics->CreateUploadableResource(256));
	*static_cast<Eugene::Matrix4x4*>(upMatrixBuffer->Map()) = matrix;
	upMatrixBuffer->UnMap();

	matrixBuffer.reset(graphics->CreateDefaultResource(256));
	matrixView_.reset(graphics->CreateShaderResourceViews(1));
	matrixView_->CreateConstantBuffer(*matrixBuffer, 0);
	
}

void InitSound(void)
{
	sound.reset(Eugene::CreateSound());
	//wave = std::make_unique<Eugene::Wave>(L"./MusicSurround.wav");
	wave = std::make_unique<Eugene::Wave>(L"./exp.wav");
	soundSpeaker.reset(sound->CreateSoundSpeaker(*wave));
	soundCtrl.reset(sound->CreateSoundControl(wave->GetFmt().sample, wave->GetFmt().channel, 2));
	sound3DCtrl.reset(sound->CreateSound3DControl(wave->GetFmt().sample, wave->GetFmt().channel, 2));

}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{
	auto c = u8"test";
	Eugene::Debug::GetInstance().LogOut(u8"test", 1);
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

	soundSpeaker->SetOutput(*sound3DCtrl);
	sound3DCtrl->Update3DSound(
		Eugene::forwardVector3<float>, Eugene::upVector3<float>, Eugene::zeroVector3<float>, Eugene::zeroVector3<float>,
		Eugene::forwardVector3<float>, Eugene::upVector3<float>, {0.5f,-0.5f, 1.0f}, Eugene::zeroVector3<float>
	);
	soundSpeaker->Play();
	


	float color[4]{ 0.0f,0.0f,0.0f,1.0f };
	Eugene::GamePad pad;
	while (libSys->Update())
	{
		Eugene::System::Mouse mouse;
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

		cmdList->SetPrimitiveType(Eugene::PrimitiveType::TriangleStrip);

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

		auto r = libSys->GetGamePad(pad, 0);
		if (libSys->IsHitKey(Eugene::KeyID::SPACE))
		{
			break;
		}
		if (pad.IsHit(Eugene::PadID::A))
		{
			break;
		}
	
	}

	
	return 0;
}