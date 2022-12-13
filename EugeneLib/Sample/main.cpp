#include <Windows.h>
#include <EugeneLib.h>
#include <memory>

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



void Init(void)
{
	// Windowsとかの機能をまとめたクラスを作成
	libSys.reset(EugeneLib::CreateSystem({ 1280.0f, 720.0f }, u8"sample"));

	{
		// グラフィックの機能のクラスを作成しコマンドリストを実行するクラスをセット
		EugeneLib::GpuEngine* tmp;
		graphics.reset(EugeneLib::CreateGraphics(*libSys, tmp));
		gpuEngien.reset(tmp);
	}

	// コマンドリストを作成
	cmdList.reset(EugeneLib::CreateCommandList(*graphics));
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


	// CPUからアクセスできるリソースを作成

	upVertexBuffer.reset(EugeneLib::CreateUploadableResource(sizeof(vertex), *graphics));
	auto ptr = upVertexBuffer->Map();
	std::copy(std::begin(vertex), std::end(vertex), reinterpret_cast<Vertex*>(ptr));
	upVertexBuffer->UnMap();

	// GPUだけでしか使えないリソースを作成
	vertexBuffer.reset(EugeneLib::CreateDefaultResource(sizeof(vertex), *graphics));

	// 頂点ビュー
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
		// コマンドの開始
		cmdList->Begin();

		// レンダーターゲットのセット
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());

		// レンダーターゲットをクリア
		cmdList->ClearRenderTarget(graphics->GetViews(), color, graphics->GetNowBackBufferIndex());

		cmdList->SetGraphicsPipeline(*gpipeLine);

		cmdList->SetShaderResourceView(*textureView_, 0, 0);

		cmdList->SetScissorrect({ 0,0 }, { 1280, 720 });

		cmdList->SetViewPort({ 0.0f,0.0f }, { 1280.0f, 720.0f });

		cmdList->SetPrimitiveType(EugeneLib::PrimitiveType::Triangle);

		cmdList->SetVertexView(*vertexView);

		cmdList->Draw(3);


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
	}

	
	return 0;
}