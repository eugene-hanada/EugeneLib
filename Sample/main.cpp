#include <Windows.h>
//#define USE_IMGUI
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <memory>
#include <vector>
#include <initializer_list>

#include <Color.h>
#include <Common/ArgsSpan.h>

#include "Common/Debug.h"


int Func(const Eugene::ArgsSpan<int> value)
{
	auto begin = value.begin();
	int result = 0;
	for (std::uint64_t i = 0ull; i < value.size(); i++)
	{
		result += *(begin + i);
	}
	return result;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{
	
	auto a = std::make_shared<int>();

	Eugene::Vector2 v{1.0f, 2.0f};
	auto result = v.Normalized().Magnitude();
	// システム(osとかの)処理をするクラス
	auto system = Eugene::CreateSystemUnique({ 1280.0f,720.0f }, u8"Sample");

	// グラフィックの処理をするクラス
	auto [graphics, gpuEngine] = system->CreateGraphicsUnique();

	std::unique_ptr < Eugene::ResourceBindLayout> resourceBidLayout;
	resourceBidLayout.reset(graphics->CreateResourceBindLayout(
		{ 
			{Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}},
			{Eugene::Bind{Eugene::ViewType::Texture,1},Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}},
			{Eugene::Bind{Eugene::ViewType::Sampler,1}}
		}));

	
	// コマンドリスト生成
	std::unique_ptr<Eugene::CommandList> cmdList;
	cmdList.reset(graphics->CreateCommandList());

	// グラフィックパイプライン生成
	std::unique_ptr<Eugene::GraphicsPipeline> pipeline;
	{

		pipeline.reset(graphics->CreateGraphicsPipeline(
			*resourceBidLayout,
			{ Eugene::ShaderInputLayout{"POSITION", 0, Eugene::Format::R32G32_FLOAT},Eugene::ShaderInputLayout{"TEXCOORD", 0, Eugene::Format::R32G32_FLOAT} },
			{ Eugene::ShaderPair{{"VertexShader.spv"}, Eugene::ShaderType::Vertex}, Eugene::ShaderPair{Eugene::Shader{"PixelShader.spv"}, Eugene::ShaderType::Pixel} },
			Eugene::RendertargetLayout{ Eugene::Format::R8G8B8A8_UNORM, Eugene::BlendType::Non },
			Eugene::TopologyType::Triangle,
			false,
			true
			//,shaderLayout
		));

	}

	std::unique_ptr<Eugene::ImageResource> depthBuffer;
	std::unique_ptr<Eugene::DepthStencilViews> depthView;
	depthBuffer.reset(graphics->CreateDepthResource({ 1280, 720 }, 0.0f));
	depthView.reset(graphics->CreateDepthStencilViews(1));
	depthView->Create(*depthBuffer, 0);

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
		cmdList->CopyBuffer(*vertexBuffer, *uploadBuffer);
		cmdList->End();
		gpuEngine->Push(*cmdList);
		gpuEngine->Execute();
		gpuEngine->Wait();
	}


	// スクリーン座標からクリップ座標に変換する行列を生成
	std::unique_ptr < Eugene::ShaderResourceViews> rtMatrixView;
	rtMatrixView.reset(graphics->CreateShaderResourceViews({ Eugene::Bind{Eugene::ViewType::ConstantBuffer,1} }));
	std::unique_ptr<Eugene::BufferResource> rtMatrixBuffer;
	rtMatrixBuffer.reset(graphics->CreateUploadableBufferResource(256));
	rtMatrixView->CreateConstantBuffer(*rtMatrixBuffer, 0);
	Eugene::Matrix4x4* rtMatrix = static_cast<Eugene::Matrix4x4*>(rtMatrixBuffer->Map());
	Eugene::Get2DMatrix(*rtMatrix, { 1280.0f,720.0f });
	//rtMatrixBuffer->UnMap();
	

	// テクスチャ用リソース
	std::unique_ptr<Eugene::ImageResource> textureResource;
	{
		// 画像読み込み
		Eugene::Image image{ "./Logo.png" };
		//Eugene::Image image2{ "./Logo.png" };
		
		image.LoadInfo();
		image.LoadData();
		//auto tmp = std::move(image);

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
	texAndMatrixView.reset(graphics->CreateShaderResourceViews({ Eugene::Bind{Eugene::ViewType::Texture,1},Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}}));
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
	samplerView.reset(graphics->CreateSamplerViews({ Eugene::Bind{Eugene::ViewType::Sampler,1} }));
	samplerView->CreateSampler(*sampler, 0);

	// カーソル表示用行列
	std::unique_ptr<Eugene::BufferResource> cursorMatrixBuffer;
	cursorMatrixBuffer.reset(graphics->CreateUploadableBufferResource(256));
	Eugene::Matrix4x4* cursorMatrix = static_cast<Eugene::Matrix4x4*>(cursorMatrixBuffer->Map());
	Eugene::Get2DTransformMatrix(*cursorMatrix, Eugene::zeroVector2<float>, 0.0f, {0.2f,0.2f });
	std::unique_ptr<Eugene::ShaderResourceViews> cursorView;
	cursorView.reset(graphics->CreateShaderResourceViews({ Eugene::Bind{Eugene::ViewType::Texture,1},Eugene::Bind{Eugene::ViewType::ConstantBuffer,1} }));
	cursorView->CreateTexture(*textureResource, 0);
	cursorView->CreateConstantBuffer(*cursorMatrixBuffer, 1);

	// サウンド
	auto sound = Eugene::CreateSoundUnique();
	std::unique_ptr<Eugene::SoundControl> ctrl2;
	std::unique_ptr<Eugene::Sound3DControl> ctrl;
	
	std::unique_ptr<Eugene::SoundControl> ctrl3;
	std::unique_ptr<Eugene::SoundSpeaker> speaker;


	// マウスの情報を受け取る構造体
	Eugene::Mouse mouse;

	// フレーム数
	std::uint32_t frameCnt = 0;

	

	float clearColor[]{ 1.0f,0.0f,0.0f,1.0f };

	std::unique_ptr<Eugene::SoundStreamSpeaker> stream;
	//stream.reset(sound->CreateSoundStreamSpeaker("./BGM.wav"));
	//stream->SetOutput(*ctrl);
	//stream->Play(1);
	//stream->SetVolume(0.7f);
	//system->ResizeWindow({ 640.0f, 480.0f });
	
	/*graphics->GetImguiShaderResourceView().CreateTexture(*textureResource, 1);
	auto img = graphics->GetImguiImageID(1);*/
	//graphics->SetFullScreenFlag(true);
	bool flag = false;




	while (system->Update())
	{
		// マウスの情報を取得
		system->GetMouse(mouse);

		// 再生チェック
		if (mouse.CheckFlags(Eugene::Mouse::Flags::LeftButton))
		{
			if (!flag)
			{
				//stream->Play(0);
				flag = true;
			}
		}
		else
		{
			flag = false;
		}

		if (system->IsHitKey(Eugene::KeyID::SPACE))
		{
			mouse.flags.set(static_cast<size_t>(Eugene::Mouse::Flags::ShowCursor), !mouse.CheckFlags(Eugene::Mouse::Flags::ShowCursor));
			system->SetMouse(mouse);
		}


		if (frameCnt % 3 == 0)
		{
			// 3フレームごとに処理する

			// 中心からの座標にする
			auto pos = mouse.pos - Eugene::Vector2{ 640.0f,360.0f };

			// 正規化して10倍
			pos.Normalize();
			//pos *= 10.0f;
			
			//ctrl->Set3DSound(
			//	Eugene::forwardVector3<float>,Eugene::upVector3<float>,Eugene::zeroVector3<float>, Eugene::zeroVector3<float>,
			//	Eugene::forwardVector3<float>, Eugene::upVector3<float>, Eugene::Vector3{pos.x, 0.0f, pos.y}, Eugene::zeroVector3<float>
			//);
			
		}

		Eugene::Get2DTransformMatrix(*cursorMatrix, mouse.pos, 0.0f, { 0.2f,0.2f }, {128.0f,128.0f});
		Eugene::Get2DMatrix(*rtMatrix, system->GetWindowSize());

		//graphics->ImguiNewFrame();
		//system->ImguiNewFrame();
		//ImGui::NewFrame();

		//ImGui::Begin("window1");
		//
		//ImGui::Image((ImTextureID)img, { 256,256});
		//
		////ImGui::Text("text1");
		//
		//ImGui::End();
		//ImGui::Render();

		//ImGuiIO& io = ImGui::GetIO();
		//if (!system->IsEnd())
		//{
		//	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//	{
		//		ImGui::UpdatePlatformWindows();
		//		ImGui::RenderPlatformWindowsDefault();
		//	}
		//}



		// コマンド開始
		cmdList->Begin();

		// レンダーターゲットセット
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());
		cmdList->TransitionDepthBegin(*depthBuffer);
		//cmdList->ClearRenderTarget(graphics->GetViews(), clearColor, graphics->GetNowBackBufferIndex());
		
		cmdList->SetRenderTarget(graphics->GetViews(), clearColor, {static_cast<std::uint32_t>(graphics->GetNowBackBufferIndex()),1u});
		//cmdList->ClearDepth(*depthView);

		// グラフィックパイプラインセット
		cmdList->SetGraphicsPipeline(*pipeline);

		const auto& size = system->GetWindowSize();

		// シザーレクトセット
		cmdList->SetScissorrect({ 0,0 }, { static_cast<int>(size.x), static_cast<int>(size.y) });

		// ビューポートセット
		cmdList->SetViewPort({ 0.0f,0.0f }, size);

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
		cmdList->TransitionDepthEnd(*depthBuffer);

		//cmdList->SetImguiCommand(ImGui::GetDrawData(), *graphics);

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