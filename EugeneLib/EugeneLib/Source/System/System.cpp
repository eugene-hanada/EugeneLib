#include "../../Include/System/System.h"
#include "../../Source/System/Windows/WindowsSystem.h"


EugeneLib::System::System(const EugeneLib::Vector2& size, const std::u8string& title) :
	windowSize_{size}, title_{title}
{
}


EugeneLib::System::~System()
{
}

const EugeneLib::Vector2& EugeneLib::System::GetWindowSize(void) const&
{
	return windowSize_;
}


EugeneLib::System* EugeneLib::CreateSystem(const Vector2& size, const std::u8string& title)
{
	return new WindowsSystem{size,title};
}
