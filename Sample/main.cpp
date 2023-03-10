#include <Windows.h>
#include <EugeneLib.h>
#include <Math/Geometry.h>
#include <memory>
#include <Common/Debug.h>

// システム系(Windows関連の処理のクラス)
std::unique_ptr<Eugene::System> libSys;

// グラフィックスのクラス(デバイスとかの処理をする)
std::unique_ptr <Eugene::Graphics> graphics;

// コマンドリストを実行するクラス
std::unique_ptr < Eugene::GpuEngine> gpuEngien;

// コマンドリスト
std::unique_ptr<Eugene::CommandList> cmdList;

// グラフィックパイプライン(処理の流れ)
std::unique_ptr<Eugene::GraphicsPipeline> gpipeLine;

// 頂点データ
std::unique_ptr <Eugene::BufferResource> upVertexBuffer;
std::unique_ptr <Eugene::BufferResource> vertexBuffer;
std::unique_ptr<Eugene::VertexView> vertexView;

// テクスチャデータ
std::unique_ptr <Eugene::BufferResource> upTextureBuffer;
std::unique_ptr <Eugene::ImageResource> textureBuffer;
std::unique_ptr < Eugene::ShaderResourceViews> textureView;

std::unique_ptr<Eugene::BufferResource> textureMat_;

// 行列データ
std::unique_ptr <Eugene::BufferResource> upMatrixBuffer;
std::unique_ptr <Eugene::BufferResource> matrixBuffer;
std::unique_ptr < Eugene::ShaderResourceViews> matrixView_;


// サウンド系
std::unique_ptr<Eugene::Wave> wave;
std::unique_ptr<Eugene::Sound> sound;
std::unique_ptr < Eugene::SoundControl> soundCtrl;
std::unique_ptr<Eugene::Sound3DControl> sound3DCtrl;
std::unique_ptr<Eugene::SoundSpeaker> soundSpeaker;
std::unique_ptr<Eugene::SoundStreamSpeaker> streamSpeaker;

// サンプラー
std::unique_ptr<Eugene::Sampler> sampler;
std::unique_ptr<Eugene::SamplerViews> smpViews;

std::unique_ptr<Eugene::ImageResource> renderTarget;
std::unique_ptr<Eugene::ImageResource> renderTarget2;
std::unique_ptr<Eugene::RenderTargetViews> rtViews;


// 
std::unique_ptr<Eugene::ShaderResourceViews> rtMatviews;
std::unique_ptr<Eugene::BufferResource> rtMatrix_;
std::unique_ptr<Eugene::BufferResource> rtVertex_;
std::unique_ptr<Eugene::VertexView> rtVertexView;

// 一つ目のレンダーターゲットテクスチャと行列
std::unique_ptr<Eugene::ShaderResourceViews> rtSrviews;
std::unique_ptr<Eugene::BufferResource> rtPosMat_;


// 二つ目のレンダーターゲットテクスチャと行列
std::unique_ptr<Eugene::ShaderResourceViews> rt2Srviews;
std::unique_ptr<Eugene::BufferResource> rt2PosMat_;
std::unique_ptr<Eugene::GraphicsPipeline> gpipeLine2;

void Init(void)
{
	// Windowsとかの機能をまとめたクラスを作成
	libSys.reset(Eugene::CreateSystem({ 1280.0f, 720.0f }, u8"sample"));

	{
		// グラフィックの機能のクラスを作成しコマンドリストを実行するクラスをセット
		Eugene::GpuEngine* tmp;
		graphics.reset(libSys->CreateGraphics(tmp,3));
		gpuEngien.reset(tmp);
	}

	// コマンドリストを作成
	cmdList.reset(graphics->CreateCommandList());
}

void InitGraphicsPipeline(void)
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

	std::vector<std::vector<Eugene::ShaderLayout>> shaderLayout
	{
		{Eugene::ShaderLayout{Eugene::ViewType::ConstantBuffer, 1,0}},
		{Eugene::ShaderLayout{Eugene::ViewType::Texture, 1,0},Eugene::ShaderLayout{Eugene::ViewType::ConstantBuffer,1,1}},
		{Eugene::ShaderLayout{Eugene::ViewType::Sampler, 1,0}}
	};


	gpipeLine.reset(graphics->CreateGraphicsPipeline(
		layout,
		shaders,
		rendertargets,
		Eugene::TopologyType::Triangle,
		false,
		shaderLayout
	));

	shaders ={
		{Eugene::Shader{"VertexShader.vso"}, Eugene::ShaderType::Vertex},
		{Eugene::Shader{"MultiRenderTargetPS.pso"}, Eugene::ShaderType::Pixel}
	};

	rendertargets = {
		{Eugene::Format::R8G8B8A8_UNORM, Eugene::BlendType::Non},
		{Eugene::Format::R8G8B8A8_UNORM, Eugene::BlendType::Non}
	};

	gpipeLine2.reset(graphics->CreateGraphicsPipeline(
		layout,
		shaders,
		rendertargets,
		Eugene::TopologyType::Triangle,
		false,
		shaderLayout
	));

}

struct Vertex
{
	Eugene::Vector2 pos;
	Eugene::Vector2 uv;
};


void InitVertex(void)
{
	Vertex vertex[4]
	{
		{{0.0f,0.0f},{0.0f,0.0f}},
		{{246.0f,0.0f},{1.0f,0.0f}},
		{{0.0f, 246.0f},{0.0f, 1.0f}},
		{{246.0f,246.0f},{1.0f, 1.0f}}
	};


	// CPUからアクセスできるリソースを作成
	upVertexBuffer.reset(graphics->CreateUploadableBufferResource(sizeof(vertex)));
	auto ptr = upVertexBuffer->Map();
	std::copy(std::begin(vertex), std::end(vertex), reinterpret_cast<Vertex*>(ptr));
	upVertexBuffer->UnMap();

	// GPUだけでしか使えないリソースを作成
	vertexBuffer.reset(graphics->CreateBufferResource(sizeof(vertex)));

	// 頂点ビュー
	vertexView.reset(graphics->CreateVertexView(sizeof(Vertex), 4ull, *vertexBuffer));
}

void InitTexture(void)
{
	Eugene::Image tex("./sky.dds");
	upTextureBuffer.reset(graphics->CreateBufferResource(tex));
	textureBuffer.reset(graphics->CreateImageResource(tex.GetInfo()));
	textureView.reset(graphics->CreateShaderResourceViews(2));
	textureView->CreateTexture(*textureBuffer, 0);

	auto samplerLayout = Eugene::SamplerLayout{};
	samplerLayout.filter_ = Eugene::SampleFilter::Linear;
	sampler.reset(graphics->CreateSampler(samplerLayout));
	smpViews.reset(graphics->CreateSamplerViews(1));
	smpViews->CreateSampler(*sampler, 0);

	textureMat_.reset(graphics->CreateUploadableBufferResource(256));
	Eugene::Matrix4x4 matrix;
	Eugene::Get2DTranslateMatrix(matrix, { 100.0f, 100.0f});
	*static_cast<Eugene::Matrix4x4*>(textureMat_->Map()) = matrix;
	textureMat_->UnMap();

	textureView->CreateConstantBuffer(*textureMat_, 1);

}

void InitConstantBuffer(void)
{
	Eugene::Matrix4x4 matrix;
	Eugene::Get2DMatrix(matrix, { 1280.0f, 720.0f });
	upMatrixBuffer.reset(graphics->CreateUploadableBufferResource(256));
	*static_cast<Eugene::Matrix4x4*>(upMatrixBuffer->Map()) = matrix;
	upMatrixBuffer->UnMap();

	matrixBuffer.reset(graphics->CreateBufferResource(256));
	matrixView_.reset(graphics->CreateShaderResourceViews(1));
	matrixView_->CreateConstantBuffer(*matrixBuffer, 0);
	
}

void InitSound(void)
{
	sound.reset(Eugene::CreateSound());
	//wave = std::make_unique<Eugene::Wave>(L"./MusicSurround.wav");
	wave = std::make_unique<Eugene::Wave>(L"./exp.wav");
	soundSpeaker.reset(sound->CreateSoundSpeaker(*wave));
	soundCtrl.reset(sound->CreateSoundControl(wave->GetFmt().sample, wave->GetFmt().channel, 2));
	sound3DCtrl.reset(sound->CreateSound3DControl(wave->GetFmt().sample, wave->GetFmt().channel, 2));

	streamSpeaker.reset(sound->CreateSoundStreamSpeaker("./test.wav"));
	streamSpeaker->SetVolume(0.25f);
	//float pan[]{ 0.7f,0.0f,1.0f,1.0f };
	//streamSpeaker->SetPan(pan);
	streamSpeaker->Play();
}

void InitRenderTarget(void)
{
	Vertex vertex[4]
	{
		{{0.0f,0.0f},{0.0f,0.0f}},
		{{640.0f,0.0f},{1.0f,0.0f}},
		{{0.0f, 360},{0.0f, 1.0f}},
		{{640.0f,360},{1.0f, 1.0f}}
	};

	float c[]{ 0.0f,0.0f,0.0f,1.0f };
	renderTarget.reset(graphics->CreateImageResource(Eugene::Vector2I{ 640,360 }, Eugene::Format::R8G8B8A8_UNORM, c));
	renderTarget2.reset(graphics->CreateImageResource(Eugene::Vector2I{ 640,360 }, Eugene::Format::R8G8B8A8_UNORM, c));
	rtViews.reset(graphics->CreateRenderTargetViews(2, false));
	rtViews->Create(*renderTarget, 0, Eugene::Format::R8G8B8A8_UNORM);
	rtViews->Create(*renderTarget2, 1, Eugene::Format::R8G8B8A8_UNORM);

	rtSrviews.reset(graphics->CreateShaderResourceViews(2));
	rtSrviews->CreateTexture(*renderTarget, 0);
	rt2Srviews.reset(graphics->CreateShaderResourceViews(2));
	rt2Srviews->CreateTexture(*renderTarget2, 0);
	
	rtMatrix_.reset(graphics->CreateUploadableBufferResource(256));
	Eugene::Matrix4x4 matrix;
	Eugene::Get2DMatrix(matrix, { 640.0f, 360.0f });
	*static_cast<Eugene::Matrix4x4*>(rtMatrix_->Map()) = matrix;
	rtMatrix_->UnMap();
	rtMatviews.reset(graphics->CreateShaderResourceViews(1));
	rtMatviews->CreateConstantBuffer(*rtMatrix_, 0);

	rtPosMat_.reset(graphics->CreateUploadableBufferResource(256));
	Eugene::Get2DTranslateMatrix(matrix, { 640.0f,0.0f });
	*static_cast<Eugene::Matrix4x4*>(rtPosMat_->Map()) = matrix;
	rtPosMat_->UnMap();
	rtSrviews->CreateConstantBuffer(*rtPosMat_, 1);

	rt2PosMat_.reset(graphics->CreateUploadableBufferResource(256));
	Eugene::Get2DTranslateMatrix(matrix, { 0.0f, 0.0f });
	*static_cast<Eugene::Matrix4x4*>(rt2PosMat_->Map()) = matrix;
	rt2PosMat_->UnMap();
	rt2Srviews->CreateConstantBuffer(*rt2PosMat_, 1);

	rtVertex_.reset(graphics->CreateUploadableBufferResource(sizeof(vertex)));
	std::copy(std::begin(vertex), std::end(vertex), reinterpret_cast<Vertex*>(rtVertex_->Map()));
	rtVertex_->UnMap();
	rtVertexView.reset(graphics->CreateVertexView(sizeof(Vertex), 4ull, *rtVertex_));

}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{
	Init();
	InitGraphicsPipeline();
	InitVertex();
	InitTexture();
	InitConstantBuffer();
	InitRenderTarget();
	InitSound();

	std::unique_ptr<Eugene::ShaderResourceViews> srv;
	srv.reset(graphics->CreateShaderResourceViews(2));
	srv->CreateTexture(*textureBuffer, 0);

	cmdList->Begin();
	cmdList->Copy(*matrixBuffer, *upMatrixBuffer);
	cmdList->Copy(*vertexBuffer, *upVertexBuffer);
	cmdList->CopyTexture(*textureBuffer, *upTextureBuffer);
	cmdList->End();

	gpuEngien->Push(*cmdList);
	gpuEngien->Execute();
	gpuEngien->Wait();

	soundSpeaker->SetOutput(*sound3DCtrl);
	sound3DCtrl->Update3DSound(
		Eugene::forwardVector3<float>, Eugene::upVector3<float>, Eugene::zeroVector3<float>, Eugene::zeroVector3<float>,
		Eugene::forwardVector3<float>, Eugene::upVector3<float>, {3.0f,0.0f, 3.0f}, Eugene::zeroVector3<float>
	);
	soundSpeaker->Play();
	
	float color2[4]{ 0.0f, 0.0f,1.0f,1.0f };
	float color[4]{ 1.0f,0.0f,0.0f,1.0f };
	float color3[4]{ 0.0f,1.0f,0.0f,1.0f };
	Eugene::GamePad pad;

	DebugLog("{}", pad.rightThumb_);
	DebugLog("↑は{}", Eugene::upVector3<float> * Eugene::pi2<float>);

	//Eugene::Image dds{ "./LogoComp.dds" };

	while (libSys->Update())
	{
		Eugene::System::Mouse mouse;
		libSys->GetMouse(mouse);
		//DebugLog(mouse.pos);
		if (soundSpeaker->IsEnd())
		{
			soundSpeaker->Play();
		}
		

		// コマンドの開始
		cmdList->Begin();

		cmdList->TransitionRenderTargetBegin(*renderTarget);
		cmdList->TransitionRenderTargetBegin(*renderTarget2);
		cmdList->SetGraphicsPipeline(*gpipeLine2);

		cmdList->SetRenderTarget(*rtViews, 0u,1u);
		cmdList->ClearRenderTarget(*rtViews, color2, 0);
		cmdList->ClearRenderTarget(*rtViews, color3, 1);

		cmdList->SetShaderResourceView(*rtMatviews, 0, 0);

		cmdList->SetShaderResourceView(*textureView, 0, 1);

		cmdList->SetSamplerView(*smpViews, 0, 2);

		cmdList->SetScissorrect({ 0,0 }, { 640, 360 });

		cmdList->SetViewPort({ 0.0f,0.0f }, { 640, 360 });

		cmdList->SetPrimitiveType(Eugene::PrimitiveType::TriangleStrip);

		cmdList->SetVertexView(*vertexView);

		cmdList->Draw(4);

		cmdList->TransitionRenderTargetEnd(*renderTarget);
		cmdList->TransitionRenderTargetEnd(*renderTarget2);

		
		// レンダーターゲットのセット
		cmdList->TransitionRenderTargetBegin(graphics->GetBackBufferResource());
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());

		// レンダーターゲットをクリア
		cmdList->ClearRenderTarget(graphics->GetViews(), color, graphics->GetNowBackBufferIndex());


		cmdList->SetGraphicsPipeline(*gpipeLine);

		cmdList->TransitionShaderResourceBegin(*renderTarget);
		cmdList->TransitionShaderResourceBegin(*renderTarget2);

		cmdList->SetShaderResourceView(*matrixView_, 0, 0);

		cmdList->SetShaderResourceView(*rtSrviews, 0, 1);

		cmdList->SetSamplerView(*smpViews, 0, 2);

		cmdList->SetScissorrect({ 0,0 }, { 1280, 720 });

		cmdList->SetViewPort({ 0.0f,0.0f }, { 1280.0f, 720.0f });

		cmdList->SetPrimitiveType(Eugene::PrimitiveType::TriangleStrip);

		cmdList->SetVertexView(*rtVertexView);

		cmdList->Draw(4);

		cmdList->SetShaderResourceView(*rt2Srviews, 0, 1);

		cmdList->Draw(4);

		cmdList->TransitionShaderResourceEnd(*renderTarget);
		cmdList->TransitionShaderResourceEnd(*renderTarget2);
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



		auto r = libSys->GetGamePad(pad, 0);
		if (libSys->IsHitKey(Eugene::KeyID::SPACE))
		{
			break;
		}
		if (libSys->IsHitKey(Eugene::KeyID::A))
		{
			streamSpeaker->Stop();
		}
		if (libSys->IsHitKey(Eugene::KeyID::D))
		{
			streamSpeaker->Play();
		}
	}
	return 0;
}