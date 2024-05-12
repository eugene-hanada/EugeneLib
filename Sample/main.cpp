#include <Windows.h>
#include <EugeneLib.h>
#include <memory>
#include <vector>


#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>
#include <ThirdParty/glm/glm/gtx/transform.hpp>
#include <ThirdParty/glm/glm/gtx/matrix_decompose.hpp>
#include <iostream>


#ifdef USE_EFFEKSEER
#include <Graphics/Graphics.h>
#include <Effekseer.h>

#endif

#ifdef USE_IMGUI
#include <ThirdParty/imgui/imgui.h>
#include <ThirdParty/imgui/imgui_internal.h>
#include <ImGuizmo.h>
#endif

#include <Debug/Debug.h>

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
	std::unique_ptr<Eugene::BufferResource> texVertexBuffer;
	std::unique_ptr<Eugene::VertexView> texVertexView;
	{
		struct Vertex2D
		{
			glm::vec2 pos;
			glm::vec2 uv;
		};

		Vertex2D vertex[4]
		{
			{{0.0f,0.0f},{0.0f,0.0f}},
			{{256.0f,0.0f},{1.0f,0.0f}},
			{{0.0f, 256.0f},{0.0f, 1.0f}},
			{{256.0f,256.0f},{1.0f, 1.0f}}
		};
		
		// アップロード用バッファ生成
		std::unique_ptr<Eugene::BufferResource> uploadBuffer;
		uploadBuffer.reset(graphics->CreateUploadableBufferResource(sizeof(vertex)));
		auto ptr = static_cast<Vertex2D*>(uploadBuffer->Map());
		std::copy(std::begin(vertex), std::end(vertex), ptr);
		uploadBuffer->UnMap();

		// 頂点バッファ生成
		texVertexBuffer.reset(graphics->CreateBufferResource(sizeof(Vertex2D) * 4));
		texVertexView.reset(graphics->CreateVertexView(sizeof(Vertex2D), 4ull, *texVertexBuffer));
		cmdList->Begin();
		cmdList->CopyBuffer(*texVertexBuffer, *uploadBuffer);
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

	// 画像と定数バッファ用のビュー
	std::unique_ptr<Eugene::ShaderResourceViews> texAndTransformView{ graphics->CreateShaderResourceViews({ Eugene::Bind{Eugene::ViewType::Texture,1},Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}}) };
	texAndTransformView->CreateTexture(*textureResource, 0);
	texAndTransformView->CreateConstantBuffer(*texMatrixBuffer, 1);

	// サンプラー生成
	std::unique_ptr<Eugene::Sampler> sampler;
	{
		Eugene::SamplerLayout layout;
		layout.filter_ = Eugene::SampleFilter::Linear;
		sampler.reset(graphics->CreateSampler(layout));
	}
	std::unique_ptr<Eugene::SamplerViews> samplerView{ graphics->CreateSamplerViews({ Eugene::Bind{Eugene::ViewType::Sampler,1} }) };
	samplerView->CreateSampler(*sampler, 0);



	// サウンド
	auto sound = Eugene::CreateSoundUnique();
	auto soundFile{ Eugene::SoundFile("./Walk.wav") };
	std::unique_ptr<Eugene::SoundControl> soundCtrl{ sound->CreateSoundControl(0,soundFile.GetFormat().sample, 2) };
	std::unique_ptr<Eugene::SoundSpeaker> speaker{sound->CreateSoundSpeaker(soundFile)};
	speaker->SetData(soundFile.GetDataPtr(), soundFile.GetDataSize());

	std::unique_ptr<Eugene::SoundStreamSpeaker> streamSound{ sound->CreateSoundStreamSpeaker("./test.wav")};
	streamSound->Play();

	speaker->SetVolume(0.5f);
	speaker->Play();
	//speaker->SetOutput(*soundCtrl);
	float panValue = 0.0f;
	

	float clearColor[]{ 1.0f,0.0f,0.0f,1.0f };

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::Enable(true);
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	
#ifdef USE_EFFEKSEER
	std::unique_ptr<Eugene::EffekseerWarpper> effekseer;
	effekseer.reset(graphics->CreateEffekseerWarpper(*gpuEngine, Eugene::Format::AUTO_BACKBUFFER, 2u));
	effekseer->SetCameraProjection(90.0f / 180.0f * 3.14f, 1280.f / 720.f, { 1.0f, 500.0f });
	effekseer->SetCameraPos({ 0.0f,0.0f,-30.0f }, { 0.0f, 0.0f, 0.0f }, Eugene::upVector3<float>);
	auto effect = Effekseer::Effect::Create(effekseer->GetManager(), u"Laser01.efkefc");
	auto effectHandle = effekseer->GetManager()->Play(effect, 0,0,0.0f);
	glm::vec3 effectPos = { 0,0,0 };
#endif

	auto cameraView{ glm::lookAtRH({0.0f,0.0f,-10.0f},{0.0f,0.0f,0.0f}, Eugene::upVector3<float>) };
	auto cameraProjection{ glm::perspectiveRH(glm::radians(90.0f), 1280.0f / 720.0f, 1.0f, 500.0f) };
	auto objectTransform{ glm::scale(glm::identity<glm::mat4>(),{1.0f,1.0f,1.0f})};
	auto identity = glm::identity<glm::mat4>();
	//DebugIO.OpenConsole();
	auto a = fmt::format("Log{}", 114514);
	DebugIO.Error("Log{0:}",114514);
	std::chrono::system_clock::time_point startTime, endTime;
	float delta = 0.0f;
	float pan[] = {0.5f,0.5f};
	speaker->SetPan(pan);
	while (system->Update())
	{
		startTime = std::chrono::system_clock::now();
		auto nowWindowSize{ system->GetWindowSize() };
		*rtMatrix = glm::ortho(0.0f, nowWindowSize.x, nowWindowSize.y, 0.0f);

#ifdef USE_IMGUI
		graphics->ImguiNewFrame();
		system->ImguiNewFrame();
		ImGui::NewFrame();
		
		ImGuizmo::BeginFrame();
		ImGuizmo::SetOrthographic(false);
		ImGui::Begin("Texture");
		{
			float pos[]{ (*texMatrix)[3][0],(*texMatrix)[3][1] };
			bool dirty = false;
			if (ImGui::DragFloat2("Position", pos))
			{
				*texMatrix = { glm::translate(identity,{pos[0],pos[1], 0.0f}) };
			}
		}
		ImGui::End();

		ImGui::Begin("Log");
		{
			ImGui::Text("Delta=%f", delta);
			ImGui::Text("FPS=%f", 1.0f / delta);

			const auto info = graphics->GetGpuMemoryInfo();

			ImGui::Text("LocalMemory=%f/%f",info.first.usage / static_cast<float>(std::giga::num), info.first.budget / static_cast<float>(std::giga::num));
			ImGui::Text("SharedMemory=%f/%f", info.second.usage / static_cast<float>(std::giga::num), info.second.budget / static_cast<float>(std::giga::num));
			auto size = ImVec2{ ImGui::GetWindowWidth() - 50, ImGui::GetWindowHeight() };
			
			if (ImGui::Button(reinterpret_cast<const char*>(u8"クリア")))
			{
				DebugIO.ClearBuffer();
			}
			if (ImGui::Button(reinterpret_cast<const char*>(u8"デルタタイムをログ出力")))
			{
				DebugIO.LogDebug("Delta={}", delta);
			}
			auto text = DebugIO.GetBuffer_View();
			ImGui::BeginChild(ImGui::GetID((void*)0), size);
			if (text.length() > 0)
			{
				ImGui::Text(text.data());
			}
			ImGui::EndChild();
		}
		ImGui::End();


#ifdef USE_EFFEKSEER
		ImGui::Begin("Effect");
		{
			float pos[]{ effectPos.x,effectPos.y, effectPos.z};
			if (ImGui::Button("Play"))
			{
				effectHandle = effekseer->GetManager()->Play(effect, { 0.0f,0.0f,0.0f });
				effectPos = Eugene::zeroVector3<float>;
				objectTransform = glm::identity<glm::mat4>();
			}
			if (ImGui::DragFloat3("Position", pos))
			{
				effekseer->GetManager()->AddLocation(effectHandle,{ pos[0] - effectPos.x, pos[1] - effectPos.y, pos[2] - effectPos.z });
				effectPos = { pos[0],pos[1],pos[2] };
			}
		}
		ImGui::End();
#endif
		
		ImGui::Begin("Sound");
		if (ImGui::Button("Play"))
		{
			speaker->Play();
		}

		{
			float volume = speaker->GetVolume();
			if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f))
			{
				speaker->SetVolume(volume);
			}

			
			//if (ImGui::SliderFloat("Pan", &panValue, -1.0f, 1.0f))
			//{
			//	//auto rate = (panValue + 1.0f) /2.0f;
			//	//pan[0] = 1.0f - rate; 
			//	//pan[1] = rate;
			//	//speaker->SetPan(pan);
			//}
		}
		ImGui::End();

		auto rectPos{ (system->GetMaxWindowSize() - nowWindowSize) / 2.0f };
		ImGuizmo::SetRect(rectPos.x, rectPos.y, nowWindowSize.x, nowWindowSize.y);
		ImGuizmo::DrawGrid(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(identity), 100.0f);
		
		if (ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, glm::value_ptr(objectTransform)))
		{
			glm::vec3 decomposeTr;
			glm::vec3 decomposeSc;
			glm::quat decomposeRt;
			glm::vec3 decomposeSkew;
			glm::vec4 decomposePer;
#ifdef USE_EFFEKSEER
			glm::decompose(objectTransform, decomposeSc, decomposeRt, decomposeTr, decomposeSkew, decomposePer);
			auto deff = decomposeTr - effectPos;

			effekseer->GetManager()->AddLocation(effectHandle, {deff.x, deff.y, deff.z});
			effectPos += deff;
#endif
		}
		ImGuizmo::ViewManipulate(glm::value_ptr(cameraView), 8.0f, ImVec2(rectPos.x, rectPos.y), ImVec2(128, 128), 0x10101010);
#endif
#ifdef USE_EFFEKSEER
		{
			glm::vec3 decomposeTr;
			glm::vec3 decomposeSc;
			glm::quat decomposeRt;
			glm::vec3 decomposeSkew;
			glm::vec4 decomposePer;
			glm::decompose(cameraView, decomposeSc, decomposeRt, decomposeTr, decomposeSkew, decomposePer);
			decomposeTr.z *= -1.0f;
			glm::mat4 tmpLH = glm::recompose(decomposeSc, decomposeRt, decomposeTr, decomposeSkew, decomposePer);
			effekseer->SetCameraPos(tmpLH);
		}
#endif
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
		effekseer->Update(1.0f / 60.0f);
#endif

		// コマンド開始
		cmdList->Begin();

		// レンダーターゲットセット
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());
		cmdList->TransitionDepthBegin(*depthBuffer);
		
		cmdList->SetRenderTarget(graphics->GetViews(), clearColor, {static_cast<std::uint32_t>(graphics->GetNowBackBufferIndex()),1u});
		
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
		cmdList->SetVertexView(*texVertexView);

		// テクスチャ、定数バッファ、サンプラーセット
		cmdList->SetShaderResourceView(*rtMatrixView, 0);
		cmdList->SetShaderResourceView(*texAndTransformView, 1);
		cmdList->SetSamplerView(*samplerView, 2);

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

		endTime = std::chrono::system_clock::now();
		delta = std::chrono::duration<float, std::chrono::seconds::period>(endTime - startTime).count();
	}

	//texMatrixBuffer->UnMap();
	//rtMatrixBuffer->UnMap();
	ImGuizmo::Enable(true);
	return 0;
}