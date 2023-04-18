#include <Windows.h>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <memory>
#include <vector>


//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
//{
//	// システム(osとかの)処理をするクラス
//	std::unique_ptr<Eugene::System> system;
//	system.reset(Eugene::CreateSystem({ 1280.0f,720.0f }, u8"Sample"));
//
//	// グラフィックの処理をするクラス
//	std::unique_ptr<Eugene::Graphics> graphics;
//	std::unique_ptr<Eugene::GpuEngine> gpuEngine;
//	{
//		auto [graphicsPtr, gpuPtr] = system->CreateGraphics();
//		graphics.reset(graphicsPtr);
//		gpuEngine.reset(gpuPtr);
//	}
//
//	while (system->Update() && !system->IsHitKey(Eugene::KeyID::ESCAPE))
//	{
//		graphics->Present();
//	}
//}

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
		Eugene::Image image{ "./LogoComp.dds" };
		//Eugene::Image image2{ "./Logo.png" };

		image.LoadInfo();
		image.LoadData();


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
	{
		Eugene::SamplerLayout layout;
		layout.filter_ = Eugene::SampleFilter::Linear;
		sampler.reset(graphics->CreateSampler(layout));
	}
	std::unique_ptr<Eugene::SamplerViews> samplerView;
	samplerView.reset(graphics->CreateSamplerViews(1));
	samplerView->CreateSampler(*sampler, 0);

	// カーソル表示用行列
	std::unique_ptr<Eugene::BufferResource> cursorMatrixBuffer;
	cursorMatrixBuffer.reset(graphics->CreateUploadableBufferResource(256));
	Eugene::Matrix4x4* cursorMatrix = static_cast<Eugene::Matrix4x4*>(cursorMatrixBuffer->Map());
	Eugene::Get2DTransformMatrix(*cursorMatrix, Eugene::zeroVector2<float>, 0.0f, {0.2f,0.2f });
	std::unique_ptr<Eugene::ShaderResourceViews> cursorView;
	cursorView.reset(graphics->CreateShaderResourceViews(2));
	cursorView->CreateTexture(*textureResource, 0);
	cursorView->CreateConstantBuffer(*cursorMatrixBuffer, 1);

	// サウンド
	std::unique_ptr<Eugene::Sound> sound;
	std::unique_ptr<Eugene::SoundSpeaker> speaker;
	sound.reset(Eugene::CreateSound());
	std::unique_ptr<Eugene::SoundFile> wave;;
	wave.reset(Eugene::OpenSoundFile("./exp.wav"));
	wave->LoadFormat();
	wave->LoadData();
	speaker.reset(sound->CreateSoundSpeaker(*wave));
	
	speaker->SetData(wave->GetDataPtr(), wave->GetDataSize());
	speaker->Play();

	// マウスの情報を受け取る構造体
	Eugene::System::Mouse mouse;

	// フレーム数
	std::uint32_t frameCnt = 0;

	float clearColor[]{ 1.0f,0.0f,0.0f,1.0f };
	while (system->Update())
	{
		// マウスの情報を取得
		system->GetMouse(mouse);

		// 再生チェック
		if (speaker->IsEnd())
		{
			// 再生終了していたら再度再生
			speaker->Play();
		}

		if (frameCnt % 3 == 0)
		{
			// 3フレームごとに処理する

			// 中心からの座標にする
			auto pos = mouse.pos - Eugene::Vector2{ 640.0f,360.0f };

			// 正規化して10倍
			pos.Normalize();
			pos *= 5.0f;

			// 位置更新
			/*ctrl3D->Set3DSound(
				Eugene::forwardVector3<float>,
				Eugene::upVector3<float>, 
				Eugene::zeroVector3<float>, 
				Eugene::zeroVector3<float>,
				Eugene::forwardVector3<float>,
				Eugene::upVector3<float>, 
				{ pos.x,0.0f, -pos.y},
				Eugene::zeroVector3<float>
			);*/
		}

		Eugene::Get2DTransformMatrix(*cursorMatrix, mouse.pos, 0.0f, { 0.2f,0.2f }, {128.0f,128.0f});


		// コマンド開始
		cmdList->Begin();

		// レンダーターゲットセット
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());
		cmdList->ClearRenderTarget(graphics->GetViews(), clearColor, graphics->GetNowBackBufferIndex());

		// グラフィックパイプラインセット
		cmdList->SetGraphicsPipeline(*pipeline);

		// シザーレクトセット
		cmdList->SetScissorrect({ 0,0 }, { 1280, 720 });

		// ビューポートセット
		cmdList->SetViewPort({ 0.0f,0.0f }, { 1280.0f, 720.0f });

		// プリミティブタイプセット
		cmdList->SetPrimitiveType(Eugene::PrimitiveType::TriangleStrip);

		// 頂点セット
		cmdList->SetVertexView(*vertexView);

		// テクスチャ、定数バッファ、サンプラーセット
		cmdList->SetShaderResourceView(*rtMatrixView, 0, 0);
		cmdList->SetShaderResourceView(*texAndMatrixView, 0, 1);
		cmdList->SetSamplerView(*samplerView, 0, 2);

		// 描画
		cmdList->Draw(4);

		// テクスチャ、定数バッファ、サンプラーセット
		cmdList->SetShaderResourceView(*cursorView, 0, 1);

		// 描画
		cmdList->Draw(4);

		cmdList->TransitionRenderTargetEnd(graphics->GetBackBufferResource());

		// コマンド終了
		cmdList->End();

		// コマンド実行
		gpuEngine->Push(*cmdList);
		gpuEngine->Execute();
		gpuEngine->Wait();
		graphics->Present();

		if (system->IsHitKey(Eugene::KeyID::ESCAPE))
		{
			break;
		}

		frameCnt++;
	}
	return 0;
}