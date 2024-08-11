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

struct Test :
	public Eugene::DynamicSingleton<Test>
{

	void Exec()
	{

	}
};

struct Vertex2D
{
	Eugene::vec2 pos;
	Eugene::vec2 uv;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{	
	Eugene::System::Create({ 1280.0f,720.0f }, u8"Sample");
	auto gpuEngine = Eugene::Graphics::Create();

	//コマンドリスト作成
	auto cmdList = Eugene::Graphics::GetInstance().CreateCommandList();

	//
	auto resourceBind{ Eugene::Graphics::GetInstance().CreateResourceBindLayout(
		{
			{Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}},
			{Eugene::Bind{Eugene::ViewType::Texture,1},Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}},
			{Eugene::Bind{Eugene::ViewType::Sampler,1}}
		},
		Eugene::ResourceBindFlag::Input
	) };

	auto graphicsPipeline{ Eugene::Graphics::GetInstance().CreateGraphicsPipeline(
		resourceBind,
		{ Eugene::ShaderInputLayout{"POSITION", 0, Eugene::Format::R32G32_FLOAT},Eugene::ShaderInputLayout{"TEXCOORD", 0, Eugene::Format::R32G32_FLOAT} },
		{ Eugene::ShaderPair{{"VertexShader.vso"}, Eugene::ShaderType::Vertex}, Eugene::ShaderPair{Eugene::Shader{"PixelShader.pso"}, Eugene::ShaderType::Pixel} },
		Eugene::RendertargetLayout{ Eugene::Format::AUTO_BACKBUFFER, Eugene::BlendType::Alpha },
		Eugene::TopologyType::Triangle,
		false,
		false
	) };

	auto vertex{
		Eugene::Graphics::GetInstance().CreateUnloadableBufferResource(sizeof(Vertex2D) * 4)
	};

	Eugene::VertexView vertexView{static_cast<std::uint32_t>(sizeof(Vertex2D)),4,vertex};

	{
		Vertex2D vert[4]
		{
			{{0.0f,0.0f},{0.0f,0.0f}},
			{{256.0f,0.0f},{1.0f,0.0f}},
			{{0.0f, 256.0f},{0.0f, 1.0f}},
			{{256.0f,256.0f},{1.0f, 1.0f}}
		};
		auto mapVertex = static_cast<Vertex2D*>(vertex.Map());
		std::copy( std::begin(vert), std::end(vert), mapVertex);
		vertex.UnMap();
	}

	auto renderTarget2D{
		Eugene::Graphics::GetInstance().CreateUnloadableBufferResource(256)
	};

	auto textureTransform{
		Eugene::Graphics::GetInstance().CreateUnloadableBufferResource(256)
	};

	*static_cast<Eugene::mat4x4*>(renderTarget2D.Map()) = Eugene::ortho(0.0f, 1280.0f, 720.0f, 0.0f);
	renderTarget2D.UnMap();

	*static_cast<Eugene::mat4x4*>(textureTransform.Map()) = Eugene::translate(glm::vec3{ (1280.0f / 2.0f) - 128.0f,(720.0f / 2.0f) - 128.0f , 0.0f });
	textureTransform.UnMap();

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
	auto renderTarget2DView{ Eugene::Graphics::GetInstance().CreateShaderResourceViews(Eugene::Bind{Eugene::ViewType::ConstantBuffer,1})
	};

	renderTarget2DView.CreateConstantBuffer(renderTarget2D, 0);

	auto textureAndTransformView{
		Eugene::Graphics::GetInstance().CreateShaderResourceViews({Eugene::Bind{Eugene::ViewType::Texture,1},Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}})
	};

	textureAndTransformView.CreateTexture(texture, 0);
	textureAndTransformView.CreateConstantBuffer(textureTransform, 1);

	auto samplerView{
		Eugene::Graphics::GetInstance().CreateSamplerViews({Eugene::Bind{Eugene::ViewType::ConstantBuffer,1}})
	};
	samplerView.CreateSampler(sampler, 0);

	float clearColor[]{ 1.0f,0.0f,0.0f,1.0f };
	while (Eugene::System::GetInstance().Update())
	{
		// コマンド開始
		cmdList.Begin();

		// レンダーターゲットセット
		cmdList.TransitionRenderTargetBegin(Eugene::Graphics::GetInstance().GetBackBufferResource());
		cmdList.SetRenderTarget(Eugene::Graphics::GetInstance().GetViews(), clearColor, {static_cast<std::uint32_t>(Eugene::Graphics::GetInstance().GetNowBackBufferIndex()),1u});

		cmdList.SetScissorrect({ 0,0 }, { 1280, 720});

		// ビューポートセット
		cmdList.SetViewPort({ 0.0f,0.0f }, {1280,720});

		// プリミティブタイプセット
		cmdList.SetPrimitiveType(Eugene::PrimitiveType::TriangleStrip);

		// 頂点ビューセット
		cmdList.SetVertexView(vertexView);

		// グラフィックスパイプラインをセット
		cmdList.SetGraphicsPipeline(graphicsPipeline);

		// レンダーターゲット座標からの変換用の定数バッファのビューをセット
		cmdList.SetShaderResourceView(renderTarget2DView, 0);

		// テクスチャとトランスフォームのビューをセット
		cmdList.SetShaderResourceView(textureAndTransformView, 1);

		// サンプラーのビューをセット
		cmdList.SetSamplerView(samplerView, 2);

		// 描画
		cmdList.Draw(4);

		cmdList.TransitionRenderTargetEnd(Eugene::Graphics::GetInstance().GetBackBufferResource());
		cmdList.End();

		gpuEngine.Push(cmdList);
		gpuEngine.Execute();
		gpuEngine.Wait();
		Eugene::Graphics::GetInstance().Present();
	}

	samplerView.Final();
	textureAndTransformView.Final();
	renderTarget2DView.Final();
	textureTransform.Final();
	renderTarget2D.Final();
	texture.Final();
	vertex.Final();
	graphicsPipeline.Final();
	resourceBind.Final();
	cmdList.Final();
	
	gpuEngine.Final();
	Eugene::Graphics::Destroy();
	Eugene::System::Destroy();

	return 0;
}