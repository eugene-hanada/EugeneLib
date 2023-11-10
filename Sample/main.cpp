﻿#include <Windows.h>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <memory>
#include <vector>
#include <initializer_list>
#include <Common/ArgsSpan.h>

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

#ifdef USE_IMGUI
void UpdateImgui()
{

}
#endif
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

		struct Vertex3D
		{
			glm::vec3 pos;
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



	//// サウンド
	//auto sound = Eugene::CreateSoundUnique();
	//std::unique_ptr<Eugene::SoundControl> ctrl2;
	//std::unique_ptr<Eugene::Sound3DControl> ctrl;
	//
	//std::unique_ptr<Eugene::SoundControl> ctrl3;
	//std::unique_ptr<Eugene::SoundSpeaker> speaker;

	//std::unique_ptr<Eugene::SoundStreamSpeaker> stream;
	//stream.reset(sound->CreateSoundStreamSpeaker("./BGM.wav"));
	//stream->SetOutput(*ctrl);
	//stream->Play(1);
	//stream->SetVolume(0.7f);
	//system->ResizeWindow({ 640.0f, 480.0f });


	// マウスの情報を受け取る構造体
	Eugene::Mouse mouse;

	// フレーム数
	std::uint32_t frameCnt = 0;

	float clearColor[]{ 1.0f,0.0f,0.0f,1.0f };

	graphics->SetImguiImage(*textureResource);
	auto img = graphics->GetImguiImageID(0);

	bool flag = false;
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::Enable(true);
	ImGuiFocusedFlags gizmoWindowFlags = 0;
	
#ifdef USE_EFFEKSEER
	std::unique_ptr<Eugene::EffekseerWarpper> effekseer;
	effekseer.reset(graphics->CreateEffekseerWarpper(*gpuEngine, Eugene::Format::AUTO_BACKBUFFER, 2u));
	effekseer->SetCameraProjection(90.0f / 180.0f * 3.14f, 1280.f / 720.f, { 1.0f, 500.0f });
	effekseer->SetCameraPos({ 0.0f,0.0f,-30.0f }, { 0.0f, 0.0f, 0.0f }, Eugene::upVector3<float>);
	auto effect = Effekseer::Effect::Create(effekseer->GetManager(), u"Laser01.efkefc");
	auto h = effekseer->GetManager()->Play(effect, 0,0,0.0f);
	glm::vec3 effectPos = { 0,0,0 };
	effekseer->GetManager()->SetRotation(h, Eugene::Deg2Rad(45.0f), Eugene::Deg2Rad(90.0f), 0.0f);
#endif

	auto cameraView{ glm::lookAtRH({0.0f,0.0f,-10.0f},{0.0f,0.0f,0.0f}, Eugene::upVector3<float>) };
	auto cameraViewLH{ glm::lookAtLH({0.0f,0.0f,-10.0f},{0.0f,0.0f,0.0f}, Eugene::upVector3<float>) };
	
	
	auto cameraProjection{ glm::perspectiveRH(glm::radians(45.0f), 1280.0f / 720.0f, 1.0f, 500.0f) };

	auto objectTransform{ glm::scale(glm::identity<glm::mat4>(),{1.0f,1.0f,1.0f})};
	auto identity = glm::identity<glm::mat4>();
	

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

		auto nowWindowSize{ system->GetWindowSize() };
		*rtMatrix = glm::ortho(0.0f, nowWindowSize.x, nowWindowSize.y, 0.0f);

		graphics->ImguiNewFrame();
		system->ImguiNewFrame();
		ImGui::NewFrame();
		
		ImGuizmo::BeginFrame();
		ImGuizmo::SetOrthographic(false);
		ImGui::Begin("texture");
		{
			float pos[]{ (*texMatrix)[3][0],(*texMatrix)[3][1] };
			bool dirty = false;
			if (ImGui::DragFloat2("Position", pos))
			{
				dirty = true;
				*texMatrix = { glm::translate(identity,{pos[0],pos[1], 0.0f}) };
			}
		}
		ImGui::End();


		ImGui::Begin("effect", 0, gizmoWindowFlags);
		{
			if (ImGui::Button("Play"))
			{
				h = effekseer->GetManager()->Play(effect, { 0.0f,0.0f,0.0f });
			}
		}

		
		ImGui::End();
		
		auto rectPos{ (system->GetMaxWindowSize() - nowWindowSize) / 2.0f };
		ImGuizmo::SetRect(rectPos.x, rectPos.y, nowWindowSize.x, nowWindowSize.y);

		ImGuizmo::DrawCubes(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(objectTransform), 1);
		ImGuizmo::DrawGrid(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(identity), 100.0f);
		
		if (ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, glm::value_ptr(objectTransform)))
		{
			glm::vec3 decomposeTr;
			glm::vec3 decomposeSc;
			glm::quat decomposeRt;
			glm::vec3 decomposeSkew;
			glm::vec4 decomposePer;
			glm::decompose(objectTransform, decomposeSc, decomposeRt, decomposeTr, decomposeSkew, decomposePer);
			effectPos += decomposeTr;
		}
		ImGuizmo::ViewManipulate(glm::value_ptr(cameraView), 8.0f, ImVec2(rectPos.x, rectPos.y), ImVec2(128, 128), 0x10101010);

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
		effekseer->Update(1.0f / 240.0f);
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

		if (system->IsHitKey(Eugene::KeyID::ESCAPE))
		{
			break;
		}

		frameCnt++;
	}
	ImGuizmo::Enable(true);
	return 0;
}