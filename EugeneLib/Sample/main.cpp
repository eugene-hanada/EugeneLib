#include <Windows.h>
#include <EugeneLib.h>
#include <Graphics/Graphics.h>
#include <Graphics/GpuEngine.h>
#include <Graphics/GpuResource.h>
#include <Graphics/CommandList.h>
#include <memory>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{
	// Windows�Ƃ��̋@�\���܂Ƃ߂��N���X���쐬
	std::unique_ptr<EugeneLib::System>system;
	system.reset(EugeneLib::CreateSystem({ 1280.0f, 720.0f }, u8"sample"));

	std::unique_ptr < EugeneLib::GpuEngine> gpuEngien;
	std::unique_ptr <EugeneLib::Graphics> graphics;

	{
		// �O���t�B�b�N�̋@�\�̃N���X���쐬���R�}���h���X�g�����s����N���X���Z�b�g
		EugeneLib::GpuEngine* tmp;
		graphics.reset(EugeneLib::CreateGraphics(*system, tmp));
		gpuEngien.reset(tmp);
	}


	// CPU����A�N�Z�X�ł��郊�\�[�X���쐬
	std::unique_ptr <EugeneLib::GpuResource> resource;
	resource.reset(EugeneLib::CreateUploadableResource(12, *graphics));

	// GPU�����ł����g���Ȃ����\�[�X���쐬
	std::unique_ptr <EugeneLib::GpuResource> defResource;
	defResource.reset(EugeneLib::CreateDefaultResource(12, *graphics));

	// �R�}���h���X�g���쐬
	std::unique_ptr<EugeneLib::CommandList> cmdList;
	cmdList.reset(EugeneLib::CreateCommandList(*graphics));

	float color[4]{ 1.0f,0.0f,0.0f,1.0f };
	while (system->Update())
	{
		// �R�}���h�̊J�n
		cmdList->Begin();

		// �����_�[�^�[�Q�b�g�̃Z�b�g
		cmdList->SetRenderTarget(graphics->GetViews(), graphics->GetNowBackBufferIndex());

		// �����_�[�^�[�Q�b�g���N���A
		cmdList->ClearRenderTarget(graphics->GetViews(), color, graphics->GetNowBackBufferIndex());

		// ���\�[�X�̃R�s�[������
		cmdList->Copy(*defResource, *resource);

		// �R�}���h�I��
		cmdList->End();

		// ���s����R�}���h��ǉ�
		gpuEngien->Push(*cmdList);

		// �R�}���h�����s
		gpuEngien->Execute();

		// �R�}���h���s��҂�
		gpuEngien->Wait();

		// �X�N���[�����o�b�N�o�b�t�@�ɓ���ւ�����
		graphics->Present();
	}

	
	return 0;
}