#include <Windows.h>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <memory>
#include <Common/Debug.h>

// システム系(Windows関連の処理のクラス)
std::unique_ptr<Eugene::System> libSys;

// グラフィックスのクラス(デバイスとかの処理をする)
std::unique_ptr <Eugene::Graphics> graphics;

// コマンドリストを実行するクラス
std::unique_ptr < Eugene::GpuEngine> gpuEngien;

// コマンドリスト
std::unique_ptr<Eugene::CommandList> cmdList;

// グラフィックパイプライン(処理の流れ)
std::unique_ptr<Eugene::GraphicsPipeline> gpipeLine;

// 頂点データ
std::unique_ptr <Eugene::GpuResource> upVertexBuffer;
std::unique_ptr <Eugene::GpuResource> vertexBuffer;
std::unique_ptr<Eugene::VertexView> vertexView;

// テクスチャデータ
std::unique_ptr <Eugene::GpuResource> upTextureBuffer;
std::unique_ptr <Eugene::GpuResource> textureBuffer;
std::unique_ptr < Eugene::ShaderResourceViews> textureView_;

// 行列データ
std::unique_ptr <Eugene::GpuResource> upMatrixBuffer;
std::unique_ptr <Eugene::GpuResource> matrixBuffer;
std::unique_ptr < Eugene::ShaderResourceViews> matrixView_;

// サウンド系
std::unique_ptr<Eugene::Wave> wave;
std::unique_ptr<Eugene::Sound> sound;
std::unique_ptr < Eugene::SoundControl> soundCtrl;
std::unique_ptr<Eugene::Sound3DControl> sound3DCtrl;
std::unique_ptr<Eugene::SoundSpeaker> soundSpeaker;

void Init(void)
{
	// Windowsとかの機能をまとめたクラスを作成
	libSys.reset(Eugene::CreateSystem({ 1280.0f, 720.0f }, u8"sample"));

	{
		// グラフィックの機能のクラスを作成しコマンドリストを実行するクラスをセット
		Eugene::GpuEngine* tmp;
		graphics.reset(libSys->CreateGraphics(tmp,3));
		gpuEngien.reset(tmp);
	}

	// コマンドリストを作成
	cmdList.reset(graphics->CreateCommandList());
}

void InitGraphicsPipeline(void)
{
	// 頂点シェーダの入力のレイアウト
	std::vector<Eugene::ShaderInputLayout> layout
	{
		{"POSITION", 0, Eugene::Format::R32G32_FLOAT},
		{"TEXCOORD", 0, Eugene::Format::R32G32_FLOAT}
	};

	// シェーダー
	std::vector<std::pair<Eugene::Shader, Eugene::ShaderType>> shaders
	{
		{Eugene::Shader{"VertexShader.vso"}, Eugene::ShaderType::Vertex},
		{Eugene::Shader{"PixelShader.pso"}, Eugene::ShaderType::Pixel}
	};

	// レンダーターゲット
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


	// CPUからアクセスできるリソースを作成

	upVertexBuffer.reset(graphics->CreateUploadableResource(sizeof(vertex)));
	auto ptr = upVertexBuffer->Map();
	std::copy(std::begin(vertex), std::end(vertex), reinterpret_cast<Vertex*>(ptr));
	upVertexBuffer->UnMap();

	// GPUだけでしか使えないリソースを作成
	vertexBuffer.reset(graphics->CreateDefaultResource(sizeof(vertex)));

	// 頂点ビュー
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

		// コマンドの開始
		cmdList->Begin();

		// レンダーターゲットのセット
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());

		// レンダーターゲットをクリア
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

		// コマンド終了
		cmdList->End();

		// 実行するコマンドを追加
		gpuEngien->Push(*cmdList);

		// コマンドを実行
		gpuEngien->Execute();

		// コマンド実行を待つ
		gpuEngien->Wait();

		// スクリーンをバックバッファに入れ替えする
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