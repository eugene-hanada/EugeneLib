#include <Windows.h>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <memory>
#include <vector>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{
	// システム(osとかの)処理をするクラス
	std::unique_ptr<Eugene::System> system;
	system.reset(Eugene::CreateSystem({ 1280.0f,720.0f }, u8"Sample"));

	// グラフィックの処理をするクラス
	std::unique_ptr<Eugene::Graphics> graphics;
	std::unique_ptr<Eugene::GpuEngine> gpuEngine;
	{
		auto [graphicsPtr, gpuPtr] = system->CreateGraphics();
		graphics.reset(graphicsPtr);
		gpuEngine.reset(gpuPtr);
	}

	// コマンドリスト生成
	std::unique_ptr<Eugene::CommandList> cmdList;
	cmdList.reset(graphics->CreateCommandList());

	// グラフィックパイプライン生成
	std::unique_ptr<Eugene::GraphicsPipeline> pipeline;
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

		// 定数バッファとかの設定
		std::vector<std::vector<Eugene::ShaderLayout>> shaderLayout
		{
			{Eugene::ShaderLayout{Eugene::ViewType::ConstantBuffer, 1,0}},
			{Eugene::ShaderLayout{Eugene::ViewType::Texture, 1,0},Eugene::ShaderLayout{Eugene::ViewType::ConstantBuffer,1,1}},
			{Eugene::ShaderLayout{Eugene::ViewType::Sampler, 1,0}}
		};

		pipeline.reset(graphics->CreateGraphicsPipeline(
			layout,
			shaders,
			rendertargets,
			Eugene::TopologyType::Triangle,
			false,
			shaderLayout
		));

	}

	// 頂点情報を生成
	std::unique_ptr<Eugene::BufferResource> vertexBuffer;
	std::unique_ptr<Eugene::VertexView> vertexView;
	{
		struct Vertex
		{
			Eugene::Vector2 pos;
			Eugene::Vector2 uv;
		};
		Vertex vertex[4]
		{
			{{0.0f,0.0f},{0.0f,0.0f}},
			{{256.0f,0.0f},{1.0f,0.0f}},
			{{0.0f, 256.0f},{0.0f, 1.0f}},
			{{256.0f,256.0f},{1.0f, 1.0f}}
		};
		
		// アップロード用バッファ生成
		std::unique_ptr<Eugene::BufferResource> uploadBuffer;
		uploadBuffer.reset(graphics->CreateUploadableBufferResource(sizeof(vertex)));
		auto ptr = static_cast<Vertex*>(uploadBuffer->Map());
		std::copy(std::begin(vertex), std::end(vertex), ptr);
		uploadBuffer->UnMap();

		// 頂点バッファ生成
		vertexBuffer.reset(graphics->CreateBufferResource(sizeof(Vertex) * 4));
		vertexView.reset(graphics->CreateVertexView(sizeof(Vertex), 4ull, *vertexBuffer));
		cmdList->Begin();
		cmdList->Copy(*vertexBuffer, *uploadBuffer);
		cmdList->End();
		gpuEngine->Push(*cmdList);
		gpuEngine->Execute();
		gpuEngine->Wait();
	}

	// スクリーン座標からクリップ座標に変換する行列を生成
	std::unique_ptr < Eugene::ShaderResourceViews> rtMatrixView;
	rtMatrixView.reset(graphics->CreateShaderResourceViews(1));
	std::unique_ptr<Eugene::BufferResource> rtMatrixBuffer;
	rtMatrixBuffer.reset(graphics->CreateUploadableBufferResource(256));
	rtMatrixView->CreateConstantBuffer(*rtMatrixBuffer, 0);
	Eugene::Matrix4x4* rtMatrix = static_cast<Eugene::Matrix4x4*>(rtMatrixBuffer->Map());
	Eugene::Get2DMatrix(*rtMatrix, { 1280.0f,720.0f });
	rtMatrixBuffer->UnMap();


	// テクスチャ用リソース
	std::unique_ptr<Eugene::ImageResource> textureResource;
	{
		// 画像読み込み
		Eugene::Image image{ "./Logo.png" };

		// リソース生成
		std::unique_ptr<Eugene::BufferResource> uploadBuffer;
		uploadBuffer.reset(graphics->CreateBufferResource(image));
		textureResource.reset(graphics->CreateImageResource(image.GetInfo()));

		cmdList->Begin();
		cmdList->CopyTexture(*textureResource, *uploadBuffer);
		cmdList->End();
		gpuEngine->Push(*cmdList);
		gpuEngine->Execute();
		gpuEngine->Wait();
	}

	// 画像用の行列
	std::unique_ptr<Eugene::BufferResource> texMatrixBuffer;
	texMatrixBuffer.reset(graphics->CreateUploadableBufferResource(256));
	Eugene::Matrix4x4* texMatrix = static_cast<Eugene::Matrix4x4*>(texMatrixBuffer->Map());
	Eugene::Get2DTranslateMatrix(*texMatrix, { (1280.0f / 2.0f) - 128.0f,(720.0f / 2.0f)-128.0f });
	texMatrixBuffer->UnMap();

	// 画像と定数バッファ用のビュー
	std::unique_ptr<Eugene::ShaderResourceViews> texAndMatrixView;
	texAndMatrixView.reset(graphics->CreateShaderResourceViews(2));
	texAndMatrixView->CreateTexture(*textureResource, 0);
	texAndMatrixView->CreateConstantBuffer(*texMatrixBuffer, 1);

	// サンプラー生成
	std::unique_ptr<Eugene::Sampler> sampler;
	sampler.reset(graphics->CreateSampler(Eugene::SamplerLayout{}));
	std::unique_ptr<Eugene::SamplerViews> samplerView;
	samplerView.reset(graphics->CreateSamplerViews(1));
	samplerView->CreateSampler(*sampler, 0);

	std::unique_ptr<Eugene::BufferResource> cursorMatrixBuffer;
	cursorMatrixBuffer.reset(graphics->CreateUploadableBufferResource(256));



	float clearColor[]{ 1.0f,0.0f,0.0f,1.0f };
	while (system->Update())
	{
		cmdList->Begin();
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());
		cmdList->ClearRenderTarget(graphics->GetViews(), clearColor, graphics->GetNowBackBufferIndex());

		cmdList->SetGraphicsPipeline(*pipeline);
		cmdList->SetShaderResourceView(*rtMatrixView, 0, 0);
		cmdList->SetShaderResourceView(*texAndMatrixView, 0, 1);
		cmdList->SetSamplerView(*samplerView, 0, 2);
		cmdList->SetScissorrect({ 0,0 }, { 1280, 720 });
		cmdList->SetViewPort({ 0.0f,0.0f }, { 1280.0f, 720.0f });
		cmdList->SetPrimitiveType(Eugene::PrimitiveType::TriangleStrip);
		cmdList->SetVertexView(*vertexView);
		cmdList->Draw(4);

		cmdList->TransitionRenderTargetEnd(graphics->GetBackBufferResource());
		cmdList->End();

		gpuEngine->Push(*cmdList);
		gpuEngine->Execute();
		gpuEngine->Wait();
		graphics->Present();

		if (system->IsHitKey(Eugene::KeyID::ESCAPE))
		{
			break;
		}
	}
	return 0;
}