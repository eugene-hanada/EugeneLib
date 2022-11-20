#include <Windows.h>
#include <EugeneLib.h>
#include <Graphics/Graphics.h>
#include <Graphics/GpuEngine.h>
#include <Graphics/GpuResource.h>
#include <Graphics/CommandList.h>
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


	// CPUからアクセスできるリソースを作成
	std::unique_ptr <EugeneLib::GpuResource> resource;
	resource.reset(EugeneLib::CreateUploadableResource(12, *graphics));

	// GPUだけでしか使えないリソースを作成
	std::unique_ptr <EugeneLib::GpuResource> defResource;
	defResource.reset(EugeneLib::CreateDefaultResource(12, *graphics));

	// コマンドリストを作成
	std::unique_ptr<EugeneLib::CommandList> cmdList;
	cmdList.reset(EugeneLib::CreateCommandList(*graphics));

	float color[4]{ 1.0f,0.0f,0.0f,1.0f };
	while (system->Update())
	{
		// コマンドの開始
		cmdList->Begin();

		// レンダーターゲットのセット
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());

		// レンダーターゲットをクリア
		cmdList->ClearRenderTarget(graphics->GetViews(), color, graphics->GetNowBackBufferIndex());

		// リソースのコピーをする
		cmdList->Copy(*defResource, *resource);

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