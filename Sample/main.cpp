#include <Windows.h>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <memory>
#include <vector>
#include <initializer_list>
#include <Common/Debug.h>
#include <Color.h>
#include <Common/ArgsSpan.h>



#ifdef USE_EFFEKSEER
#include <Graphics/Graphics.h>
#include <Effekseer.h>
#endif

#ifdef USE_IMGUI
#include <ThirdParty/imgui/imgui.h>
#endif

#include "Common/Debug.h"


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{
	// システム(osとかの)処理をするクラス
	auto system = Eugene::CreateSystemUnique({ 1280.0f,720.0f }, u8"Sample");

	// グラフィックの処理をするクラス
	auto [graphics, gpuEngine] = system->CreateGraphicsUnique();

	std::unique_ptr < Eugene::ResourceBindLayout> resourceBidLayout{ graphics->CreateResourceBindLayout(
		{
			{Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}},
			{Eugene::Bind{Eugene::ViewType::Texture,1},Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}},
			{Eugene::Bind{Eugene::ViewType::Sampler,1}}
		}) };

	
	// コマンドリスト生成
	std::unique_ptr<Eugene::CommandList> cmdList{ graphics->CreateCommandList() };
	
	// グラフィックパイプライン生成
	std::unique_ptr<Eugene::GraphicsPipeline> pipeline{ graphics->CreateGraphicsPipeline(
			*resourceBidLayout,
			{ Eugene::ShaderInputLayout{"POSITION", 0, Eugene::Format::R32G32_FLOAT},Eugene::ShaderInputLayout{"TEXCOORD", 0, Eugene::Format::R32G32_FLOAT} },
			{ Eugene::ShaderPair{{"VertexShader.vso"}, Eugene::ShaderType::Vertex}, Eugene::ShaderPair{Eugene::Shader{"PixelShader.pso"}, Eugene::ShaderType::Pixel} },
			Eugene::RendertargetLayout{ Eugene::Format::AUTO_BACKBUFFER, Eugene::BlendType::Alpha },
			Eugene::TopologyType::Triangle,
			true,
			false
		) };

	std::unique_ptr<Eugene::ImageResource> depthBuffer{ graphics->CreateDepthResource({ 1280, 720 }, 0.0f) };
	std::unique_ptr<Eugene::DepthStencilViews> depthView{ graphics->CreateDepthStencilViews(1) };
	depthView->Create(*depthBuffer, 0);

	// 頂点情報を生成
	std::unique_ptr<Eugene::BufferResource> vertexBuffer;
	std::unique_ptr<Eugene::VertexView> vertexView;
	{
		struct Vertex
		{
			glm::vec2 pos;
			glm::vec2 uv;
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
	std::unique_ptr < Eugene::ShaderResourceViews> rtMatrixView{ graphics->CreateShaderResourceViews({ Eugene::Bind{Eugene::ViewType::ConstantBuffer,1} }) };
	std::unique_ptr<Eugene::BufferResource> rtMatrixBuffer{ graphics->CreateUploadableBufferResource(256) };
	rtMatrixView->CreateConstantBuffer(*rtMatrixBuffer, 0);
	
	auto rtMatrix = static_cast<glm::mat4*>(rtMatrixBuffer->Map());
	*rtMatrix = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f);

	// テクスチャ用リソース
	std::unique_ptr<Eugene::ImageResource> textureResource;
	{
		// 画像読み込み
		Eugene::Image image{ "./Logo.png" };
		
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
	auto texMatrix = static_cast<glm::mat4*>(texMatrixBuffer->Map());
	*texMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3{ (1280.0f / 2.0f) - 128.0f,(720.0f / 2.0f) - 128.0f , 0.0f});
	texMatrixBuffer->UnMap();

	// 画像と定数バッファ用のビュー
	std::unique_ptr<Eugene::ShaderResourceViews> texAndMatrixView{ graphics->CreateShaderResourceViews({ Eugene::Bind{Eugene::ViewType::Texture,1},Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}}) };
	texAndMatrixView->CreateTexture(*textureResource, 0);
	texAndMatrixView->CreateConstantBuffer(*texMatrixBuffer, 1);

	// サンプラー生成
	std::unique_ptr<Eugene::Sampler> sampler;
	{
		Eugene::SamplerLayout layout;
		layout.filter_ = Eugene::SampleFilter::Linear;
		sampler.reset(graphics->CreateSampler(layout));
	}
	std::unique_ptr<Eugene::SamplerViews> samplerView{ graphics->CreateSamplerViews({ Eugene::Bind{Eugene::ViewType::Sampler,1} }) };
	samplerView->CreateSampler(*sampler, 0);

	// カーソル表示用行列
	std::unique_ptr<Eugene::BufferResource> cursorMatrixBuffer{ graphics->CreateUploadableBufferResource(256) };
	auto cursorMatrix = static_cast<glm::mat4*>(cursorMatrixBuffer->Map());
	*cursorMatrix = glm::identity<glm::mat4>();
	std::unique_ptr<Eugene::ShaderResourceViews> cursorView{ graphics->CreateShaderResourceViews({ Eugene::Bind{Eugene::ViewType::Texture,1},Eugene::Bind{Eugene::ViewType::ConstantBuffer,1} }) };
	cursorView->CreateTexture(*textureResource, 0);
	cursorView->CreateConstantBuffer(*cursorMatrixBuffer, 1);

	std::unique_ptr<Eugene::BufferResource> index{ graphics->CreateBufferResource(256) };

	std::unique_ptr<Eugene::IndexView> indexView_{ graphics->CreateIndexView(4u, 4u,Eugene::Format::R16_FLOAT,*index) };

	//// サウンド
	//auto sound = Eugene::CreateSoundUnique();
	//std::unique_ptr<Eugene::SoundControl> ctrl2;
	//std::unique_ptr<Eugene::Sound3DControl> ctrl;
	//
	//std::unique_ptr<Eugene::SoundControl> ctrl3;
	//std::unique_ptr<Eugene::SoundSpeaker> speaker;


	// マウスの情報を受け取る構造体
	Eugene::Mouse mouse;

	// フレーム数
	std::uint32_t frameCnt = 0;

	

	float clearColor[]{ 1.0f,0.0f,0.0f,1.0f };

	//std::unique_ptr<Eugene::SoundStreamSpeaker> stream;
	//stream.reset(sound->CreateSoundStreamSpeaker("./BGM.wav"));
	//stream->SetOutput(*ctrl);
	//stream->Play(1);
	//stream->SetVolume(0.7f);
	//system->ResizeWindow({ 640.0f, 480.0f });
	
	/*graphics->GetImguiShaderResourceView().CreateTexture(*textureResource, 1);
	*/
	graphics->SetImguiImage(*textureResource);
	auto img = graphics->GetImguiImageID(0);
	bool flag = false;
	ImGuiIO& io = ImGui::GetIO();

	graphics->SetFullScreenFlag(true);

#ifdef USE_EFFEKSEER
	std::unique_ptr<Eugene::EffekseerWarpper> effekseer;
	effekseer.reset(graphics->CreateEffekseerWarpper(*gpuEngine, Eugene::Format::AUTO_BACKBUFFER, 2u));
	effekseer->SetCameraProjection(90.0f / 180.0f * 3.14f, 1280.f / 720.f, { 1.0f, 500.0f });
	effekseer->SetCameraPos({ 0.0f,0.0f,-30.0f }, { 0.0f, 0.0f, 0.0f }, Eugene::upVector3<float>);
	auto effect = Effekseer::Effect::Create(effekseer->GetManager(), u"Laser01.efkefc");
	auto h = effekseer->GetManager()->Play(effect, 0,0,0.0f);
	effekseer->GetManager()->SetRotation(h, Eugene::Deg2Rad(45.0f), Eugene::Deg2Rad(90.0f), 0.0f);
#endif

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
			auto pos = mouse.pos - (system->GetWindowSize() / 2.0f);

			// 正規化して10倍
			pos = glm::normalize(pos);
			//pos *= 10.0f;
			
			//ctrl->Set3DSound(
			//	Eugene::forwardVector3<float>,Eugene::upVector3<float>,Eugene::zeroVector3<float>, Eugene::zeroVector3<float>,
			//	Eugene::forwardVector3<float>, Eugene::upVector3<float>, Eugene::Vector3{pos.x, 0.0f, pos.y}, Eugene::zeroVector3<float>
			//);
			
		}

		//Eugene::Get2DTransformMatrix(*cursorMatrix, mouse.pos, 0.0f, { 0.2f,0.2f }, {128.0f,128.0f});
		*cursorMatrix = glm::translate(glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3{ mouse.pos.x,mouse.pos.y ,0.0f }), glm::vec3{ 0.2f }), glm::vec3{-128.0f, -128.0f, 0.0f});
		//Eugene::Get2DMatrix(*rtMatrix, system->GetWindowSize());
		auto nowWindowSize{ system->GetWindowSize() };
		*rtMatrix = glm::ortho(0.0f, nowWindowSize.x, nowWindowSize.y, 0.0f);

		graphics->ImguiNewFrame();
		system->ImguiNewFrame();
		ImGui::NewFrame();

		ImGui::Begin("window1");
		auto imgSize{ textureResource->GetSize() };
		ImGui::Image(img, ImVec2{static_cast<float>(imgSize.x),static_cast<float>(imgSize.y)});
		ImGui::End();

		ImGui::Begin("window2");

		ImGui::End();
		ImGui::Render();

		
		if (!system->IsEnd())
		{
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}

		
#ifdef USE_EFFEKSEER
		effekseer->Update(1.0f / 75.0f);
#endif

		// コマンド開始
		cmdList->Begin();

		// レンダーターゲットセット
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());
		cmdList->TransitionDepthBegin(*depthBuffer);
		
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
		cmdList->SetShaderResourceView(*rtMatrixView, 0);
		cmdList->SetShaderResourceView(*texAndMatrixView, 1);
		cmdList->SetSamplerView(*samplerView, 2);

		

		// 描画
		cmdList->Draw(4);

		// テクスチャ、定数バッファ、サンプラーセット
		cmdList->SetShaderResourceView(*cursorView, 1);

		// 描画
		cmdList->Draw(4);

#ifdef USE_EFFEKSEER
		effekseer->Draw(*cmdList);
#endif

		cmdList->TransitionRenderTargetEnd(graphics->GetBackBufferResource());
		cmdList->TransitionDepthEnd(*depthBuffer);
		
#ifdef USE_IMGUI
		cmdList->SetImguiCommand(ImGui::GetDrawData(), *graphics);
#endif


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