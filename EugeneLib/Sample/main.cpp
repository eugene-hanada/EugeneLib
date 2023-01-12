#include <Windows.h>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <memory>
#include <Common/Debug.h>

// システム系(Windows関連の処理のクラス)
std::unique_ptr<EugeneLib::System> libSys;

// グラフィックスのクラス(デバイスとかの処理をする)
std::unique_ptr <EugeneLib::Graphics> graphics;

// コマンドリストを実行するクラス
std::unique_ptr < EugeneLib::GpuEngine> gpuEngien;

// コマンドリスト
std::unique_ptr<EugeneLib::CommandList> cmdList;

// グラフィックパイプライン(処理の流れ)
std::unique_ptr<EugeneLib::GraphicsPipeline> gpipeLine;

// 頂点データ
std::unique_ptr <EugeneLib::GpuResource> upVertexBuffer;
std::unique_ptr <EugeneLib::GpuResource> vertexBuffer;
std::unique_ptr<EugeneLib::VertexView> vertexView;

// テクスチャデータ
std::unique_ptr <EugeneLib::GpuResource> upTextureBuffer;
std::unique_ptr <EugeneLib::GpuResource> textureBuffer;
std::unique_ptr < EugeneLib::ShaderResourceViews> textureView_;

// 行列データ
std::unique_ptr <EugeneLib::GpuResource> upMatrixBuffer;
std::unique_ptr <EugeneLib::GpuResource> matrixBuffer;
std::unique_ptr < EugeneLib::ShaderResourceViews> matrixView_;

// サウンド系
std::unique_ptr<EugeneLib::Wave> wave;
std::unique_ptr<EugeneLib::Sound> sound;
std::unique_ptr < EugeneLib::SoundControl> soundCtrl;
std::unique_ptr<EugeneLib::SoundSpeaker> soundSpeaker;

void Init(void)
{
	// Windowsとかの機能をまとめたクラスを作成
	libSys.reset(EugeneLib::CreateSystem({ 1280.0f, 720.0f }, u8"sample"));

	{
		// グラフィックの機能のクラスを作成しコマンドリストを実行するクラスをセット
		EugeneLib::GpuEngine* tmp;
		graphics.reset(libSys->CreateGraphics(tmp,3));
		gpuEngien.reset(tmp);
	}

	// コマンドリストを作成
	cmdList.reset(graphics->CreateCommandList());
}

void InitGraphicsPipeline(void)
{
	// 頂点シェーダの入力のレイアウト
	std::vector<EugeneLib::ShaderInputLayout> layout
	{
		{"POSITION", 0, EugeneLib::Format::R32G32_FLOAT},
		{"TEXCOORD", 0, EugeneLib::Format::R32G32_FLOAT}
	};

	// シェーダー
	std::vector<std::pair<EugeneLib::Shader, EugeneLib::ShaderType>> shaders
	{
		{EugeneLib::Shader{"VertexShader.vso"}, EugeneLib::ShaderType::Vertex},
		{EugeneLib::Shader{"PixelShader.pso"}, EugeneLib::ShaderType::Pixel}
	};

	// レンダーターゲット
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

		cmdList->SetPrimitiveType(EugeneLib::PrimitiveType::TriangleStrip);

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


		if (GetKeyboardState(key))
		{
			for (int i = 0; i < 256; i++)
			{
				if ((key[i] & 0x80))
				{
					DebugLog(i, u8"押されました");
				}
			}
		}
	}

	
	return 0;
}