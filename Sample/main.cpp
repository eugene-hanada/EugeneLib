﻿#include <Windows.h>
#include <EugeneLib.h>


#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>
#include <ThirdParty/glm/glm/gtx/transform.hpp>
#include <ThirdParty/glm/glm/gtx/matrix_decompose.hpp>
#include <iostream>


#ifdef EUGENE_IMGUI
#include <ThirdParty/imgui/imgui.h>
#include <ImGuizmo.h>
#endif

struct Vertex2D
{
	Eugene::vec2 pos;
	Eugene::vec2 uv;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	
	Eugene::System::Create({ 1280.0f,720.0f }, u8"Sample");
	Eugene::Sound::Create();

#ifdef EUGENE_SOUND
	Eugene::SoundFile soundFile{ "./seSound.wav" };
	Eugene::SoundSpeaker sp;
	sp = Eugene::Sound::GetInstance().CreateSoundSpeaker(soundFile);
	sp.SetData(soundFile.GetDataPtr(), soundFile.GetDataSize());

	Eugene::SoundStreamSpeaker streamSp;
	streamSp = Eugene::Sound::GetInstance().CreateSoundStreamSpeaker(Eugene::CreateSoundStreamFile("./bgmSound.wav"));
#endif

	auto gpuEngine = Eugene::Graphics::Create();

	//コマンドリスト作成
	auto cmdList = Eugene::Graphics::GetInstance().CreateCommandList();

	// リソースバインド
	auto resourceBind{ Eugene::Graphics::GetInstance().CreateResourceBindLayout(
		{
			{Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}},
			{Eugene::Bind{Eugene::ViewType::Texture,1},Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}},
			{Eugene::Bind{Eugene::ViewType::Sampler,1}}
		},
		Eugene::ResourceBindFlag::Input
	) };

	// グラフィックスパイプライン
	auto graphicsPipeline{ Eugene::Graphics::GetInstance().CreateGraphicsPipeline(
		resourceBind,
		{ Eugene::ShaderInputLayout{"POSITION", 0, Eugene::Format::R32G32_FLOAT},Eugene::ShaderInputLayout{"TEXCOORD", 0, Eugene::Format::R32G32_FLOAT} },
		{ Eugene::ShaderPair{{"VertexShader.vso"}, Eugene::ShaderType::Vertex}, Eugene::ShaderPair{Eugene::Shader{"PixelShader.pso"}, Eugene::ShaderType::Pixel} },
		Eugene::RendertargetLayout{ Eugene::Format::AUTO_BACKBUFFER, Eugene::BlendType::Alpha },
		Eugene::TopologyType::Triangle,
		false,
		false
	) };

	// 頂点バッファ
	auto vertexBuffer{
		Eugene::Graphics::GetInstance().CreateUnloadableBufferResource(sizeof(Vertex2D) * 4)
	};

	// 頂点ビュー
	Eugene::VertexView vertexView{static_cast<std::uint32_t>(sizeof(Vertex2D)),4,vertexBuffer};
	
	{
		Vertex2D vert[4]
		{
			{{0.0f,0.0f},{0.0f,0.0f}},
			{{256.0f,0.0f},{1.0f,0.0f}},
			{{0.0f, 256.0f},{0.0f, 1.0f}},
			{{256.0f,256.0f},{1.0f, 1.0f}}
		};
		auto mapVertex = static_cast<Vertex2D*>(vertexBuffer.Map());
		std::copy( std::begin(vert), std::end(vert), mapVertex);
		vertexBuffer.UnMap();
	}

	auto renderTargetBuffer{
		Eugene::Graphics::GetInstance().CreateUnloadableBufferResource(256)
	};

	auto textureTransformBuffer{
		Eugene::Graphics::GetInstance().CreateUnloadableBufferResource(256)
	};

	auto renderTarget{ static_cast<Eugene::mat4x4*>(renderTargetBuffer.Map()) };
	*renderTarget = Eugene::ortho(0.0f, 1280.0f, 720.0f, 0.0f);

	auto textureTransform{ static_cast<Eugene::mat4x4*>(textureTransformBuffer.Map()) };
	*textureTransform = Eugene::translate(glm::vec3{ (1280.0f / 2.0f) - 128.0f,(720.0f / 2.0f) - 128.0f , 0.0f });

	auto sampler{
	Eugene::Graphics::GetInstance().CreateSampler(
		{
			Eugene::TextureAddressMode::Wrap,
			Eugene::TextureAddressMode::Wrap,
			Eugene::TextureAddressMode::Wrap,
			Eugene::SampleFilter::Linear
		}
	)};


	Eugene::Image image{ "./Logo.png" };
	auto texture{
		Eugene::Graphics::GetInstance().CreateImageResource( image.GetInfo())
	};

	{
		auto uploadTexture{
				Eugene::Graphics::GetInstance().CreateBufferResource(image)
		};

		cmdList.Begin();
		cmdList.CopyTexture(texture, uploadTexture);
		cmdList.End();
		gpuEngine.Push(cmdList);
		gpuEngine.Execute();
		gpuEngine.Wait();
	}

	auto renderTargetView{ Eugene::Graphics::GetInstance().CreateShaderResourceViews(Eugene::Bind{Eugene::ViewType::ConstantBuffer,1})
	};

	renderTargetView.CreateConstantBuffer(renderTargetBuffer, 0);

	auto textureAndTransformView{
		Eugene::Graphics::GetInstance().CreateShaderResourceViews({Eugene::Bind{Eugene::ViewType::Texture,1},Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}})
	};

	textureAndTransformView.CreateTexture(texture, 0);
	textureAndTransformView.CreateConstantBuffer(textureTransformBuffer, 1);

	auto samplerView{
		Eugene::Graphics::GetInstance().CreateSamplerViews({Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}})
	};

	samplerView.CreateSampler(sampler, 0);

#ifdef EUGENE_IMGUI
	Eugene::Graphics::GetInstance().SetImguiImage(texture, 0);
	ImGuiIO& io = ImGui::GetIO();
#endif
	float clearColor[]{ 1.0f,0.0f,0.0f,1.0f };

	while (Eugene::System::GetInstance().Update())
	{
#ifdef EUGENE_IMGUI
		Eugene::Graphics::GetInstance().ImguiNewFrame();
		Eugene::System::GetInstance().ImguiNewFrame();
		ImGui::NewFrame();

		if (ImGui::Begin("Texture"))
		{
			float pos[]{ (*textureTransform)[3][0],(*textureTransform)[3][1] };
			bool dirty = false;
			ImGui::Image(
				Eugene::Graphics::GetInstance().GetImguiImageID(0),
				{ 128,128 }
			);
			if (ImGui::DragFloat2("Position", pos))
			{
				*textureTransform = Eugene::translate(Eugene::vec3{pos[0],pos[1], 0.0f});
			}
		}
		ImGui::End();

#ifdef EUGENE_SOUND
		if (ImGui::Begin("SE"))
		{
			float volume{sp.GetVolume()};
			if (ImGui::DragFloat("Volume", &volume,0.01f, 0.0f, 1.0f))
			{
				sp.SetVolume(volume);
			}

			if (ImGui::Button("Play"))
			{
				sp.Play();
			}
		}
		ImGui::End();

		if (ImGui::Begin("BGM"))
		{
			float volume{ streamSp.GetVolume() };
			if (ImGui::DragFloat("Volume", &volume, 0.01f, 0.0f, 1.0f))
			{
				streamSp.SetVolume(volume);
			}

			if (ImGui::Button("Play"))
			{
				streamSp.Play();
			}
		}
		ImGui::End();
#endif

		ImGui::Render();

		if (!Eugene::System::GetInstance().IsEnd())
		{
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}
#endif

		if (Eugene::System::GetInstance().IsActive())
		{

			// コマンド開始
			cmdList.Begin();

			// レンダーターゲットセット
			cmdList.TransitionRenderTargetBegin(Eugene::Graphics::GetInstance().GetBackBufferResource());
			cmdList.SetRenderTarget(Eugene::Graphics::GetInstance().GetViews(), clearColor, { static_cast<std::uint32_t>(Eugene::Graphics::GetInstance().GetNowBackBufferIndex()),1u });

			cmdList.SetScissorrect({ 0,0 }, { 1280, 720 });

			// ビューポートセット
			cmdList.SetViewPort({ 0.0f,0.0f }, { 1280,720 });

			// プリミティブタイプセット
			cmdList.SetPrimitiveType(Eugene::PrimitiveType::TriangleStrip);

			// 頂点ビューセット
			cmdList.SetVertexView(vertexView);

			// グラフィックスパイプラインをセット
			cmdList.SetGraphicsPipeline(graphicsPipeline);

			// レンダーターゲット座標からの変換用の定数バッファのビューをセット
			cmdList.SetShaderResourceView(renderTargetView, 0);

			// テクスチャとトランスフォームのビューをセット
			cmdList.SetShaderResourceView(textureAndTransformView, 1);

			// サンプラーのビューをセット
			cmdList.SetSamplerView(samplerView, 2);

			// 描画
			cmdList.Draw(4);

			cmdList.TransitionRenderTargetEnd(Eugene::Graphics::GetInstance().GetBackBufferResource());

#ifdef EUGENE_IMGUI
			cmdList.SetImguiCommand(ImGui::GetDrawData());
#endif
			cmdList.End();

			gpuEngine.Push(cmdList);
			gpuEngine.Execute();
			gpuEngine.Wait();
			Eugene::Graphics::GetInstance().Present();
		}
	}

	samplerView.Final();
	textureAndTransformView.Final();
	renderTargetView.Final();

	sampler.Final();
	textureTransformBuffer.Final();
	renderTargetBuffer.Final();
	texture.Final();
	vertexBuffer.Final();

	graphicsPipeline.Final();
	resourceBind.Final();
	cmdList.Final();
	
	gpuEngine.Final();


	streamSp.Final();

	Eugene::Graphics::Destroy();
	Eugene::System::Destroy();

	return 0;
}