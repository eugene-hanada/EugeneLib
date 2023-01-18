#pragma once
#include <filesystem>
#include <string>

class DynamicLibrary
{
public:
	virtual ~DynamicLibrary();
	template<typename func>
	func* GetFunction(const std::string& functionName)
	{
		return static_cast<func*>(FindFunction(functionName));
	}

protected:
	DynamicLibrary();
	virtual void* FindFunction(const std::string& functionName) const = 0;
};

