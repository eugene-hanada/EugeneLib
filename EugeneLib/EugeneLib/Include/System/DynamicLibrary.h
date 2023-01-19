#pragma once
#include <filesystem>
#include <string>

/// <summary>
/// DLL�Ƃ��̓��I�����N���郉�C�u�������������߂̃N���X
/// </summary>
class DynamicLibrary
{
public:
	virtual ~DynamicLibrary();

	/// <summary>
	/// �w�肳�ꂽ���O�̊֐��̃|�C���^���擾����
	/// </summary>
	/// <typeparam name="func"> �֐��̌^ </typeparam>
	/// <param name="functionName"> �֐��� </param>
	/// <returns> �֐��|�C���^ </returns>
	template<typename func>
	func* GetFunction(const std::string& functionName)
	{
		return static_cast<func*>(FindFunction(functionName));
	}

protected:
	DynamicLibrary();

	/// <summary>
	/// �֐����̊֐����擾����
	/// </summary>
	/// <param name="functionName"></param>
	/// <returns></returns>
	virtual void* FindFunction(const std::string& functionName) const = 0;
};

