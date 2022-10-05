#include <EugeneLib.h>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int mCmdShow)
{
	EugeneLib::System::Init({ 1280.0f, 720.0f }, u8"sample");
	while (EugeneLib::System::Ptr()->Update())
	{

	}
	return 0;
}