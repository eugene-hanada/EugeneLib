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
	// Windowsとかの機能をまとめたクラスを作成
	std::unique_ptr<EugeneLib::System>system;
	system.reset(EugeneLib::CreateSystem({ 1280.0f, 720.0f }, u8"sample"));

	std::unique_ptr < EugeneLib::GpuEngine> gpuEngien;
	std::unique_ptr <EugeneLib::Graphics> graphics;

	{
		// グラフィックの機能のクラスを作成しコマンドリストを実行するクラスをセット
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

	// 頂点シェーダの入力のレイアウト
	std::vector<EugeneLib::ShaderInputLayout> layout
	{
		{"POSITION", 0, EugeneLib::Format::R32G32_FLOAT},
		{"TEXCOORD", 0, EugeneLib::Format::R32G32_FLOAT}
	};

	// シェーダー
	std::vector<std::pair<EugeneLib::Shader,EugeneLib::ShaderType>> shaders
	{
		{EugeneLib::Shader{"VertexShader.vso"}, EugeneLib::ShaderType::Vertex},
		{EugeneLib::Shader{"PixelShader.pso"}, EugeneLib::ShaderType::Pixel}
	};

	// レンダーターゲット
	std::vector<EugeneLib::RendertargetLayout> rendertargets
	{
		{EugeneLib::Format::R8G8B8A8_UNORM, EugeneLib::BlendType::Non}
	};

	std::unique_ptr<EugeneLib::GraphicsPipeline> gpipeLine;
	gpipeLine.reset(EugeneLib::CreateGraphicsPipeline(*graphics, layout, shaders, rendertargets));


	// CPUからアクセスできるリソースを作成
	std::unique_ptr <EugeneLib::GpuResource> resource;
	resource.reset(EugeneLib::CreateUploadableResource(sizeof(vertex), *graphics));
	auto ptr = resource->Map();
	std::copy(std::begin(vertex), std::end(vertex), reinterpret_cast<Vertex*>(ptr));
	resource->UnMap();

	// GPUだけでしか使えないリソースを作成
	std::unique_ptr <EugeneLib::GpuResource> defResource;
	defResource.reset(EugeneLib::CreateDefaultResource(sizeof(vertex), *graphics));

	// 頂点ビュー
	std::unique_ptr<EugeneLib::VertexView> vertexView;
	vertexView.reset(EugeneLib::CreateVertexView(sizeof(Vertex), 3ull, *defResource));

	// コマンドリストを作成
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
		// コマンドの開始
		cmdList->Begin();

		// レンダーターゲットのセット
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());

		// レンダーターゲットをクリア
		cmdList->ClearRenderTarget(graphics->GetViews(), color, graphics->GetNowBackBufferIndex());

		cmdList->SetGraphicsPipeline(*gpipeLine);

		cmdList->SetScissorrect({ 0,0 }, { 1280, 720 });

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