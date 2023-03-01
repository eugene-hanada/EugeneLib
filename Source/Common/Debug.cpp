#include "../../Include/Common/Debug.h"

#ifdef _DEBUG

#include <iostream>
#include <format>
#include <filesystem>
#include <cuchar>
#include <chrono>
#include <spanstream>
#ifdef _WIN64
#include <Windows.h>
FILE* fp;
#endif



Eugene::Debug& Eugene::Debug::GetInstance(void)
{
	static Debug instance_;
	return instance_;
}




void Eugene::Debug::Log(const std::string& str)
{
	// ���b�N
	binarySemphore_.acquire();

	// ���݂̎��Ԃ��~���b�Ŏ擾
	auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());

	// �o�b�t�@�𗘗p�����X�g���[������X���b�hID�𕶎���ɕϊ�
	std::spanstream s{buff};
	s << std::this_thread::get_id();

	// ���ԂƃX���b�hID��str���o��
	std::cout << std::format("Log[{0:%H:%M:%S}][Thread={1:}]{2:}\n", std::chrono::zoned_time{ std::chrono::current_zone(),now }, std::string_view{ buff.data(),buff.size()},str);
	binarySemphore_.release();
}

Eugene::Debug::Debug() :
	binarySemphore_{1}
{
#ifdef _WIN64
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);

	// �X���b�hID�𕶎���ɕϊ����ăo�b�t�@�̃T�C�Y�ɂ���
	std::ostringstream oss;
	oss << std::this_thread::get_id();
	buff.resize(oss.str().size());
#endif
}

Eugene::Debug::~Debug()
{
#ifdef _WIN64
	fclose(fp);
#endif
}

#endif