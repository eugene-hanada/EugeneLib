#pragma once
#include <concepts>
#include <string>
#include <filesystem>
#include <format>
#include <semaphore>
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#ifdef _DEBUG

#define DebugLog(str,...) (Eugene::Debug::GetInstance().Log(str,__VA_ARGS__))


/// <summary>
/// Vector2�pstd::formatter
/// </summary>
template<Eugene::ValueC T>
class std::formatter<Eugene::Vector2Tmp<T>> :
	public std::formatter<const char*>
{
public:

	/// <summary>
	/// �t�H�[�}�b�g(x=0y=0�݂����ɕ\������)
	/// </summary>
	/// <typeparam name="Out"></typeparam>
	/// <param name="vec"></param>
	/// <param name="ctx"></param>
	/// <returns></returns>
	template<class Out>
	auto format(const Eugene::Vector2Tmp<T>& vec, std::basic_format_context<Out, char>& ctx)
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			// ���������_�^�̎�
			return std::format_to(ctx.out(), "x={0:f}y={1:f}", vec.x, vec.y);
		}
		else if constexpr (std::is_integral<T>::value)
		{
			// �����^�̎�
			return std::format_to(ctx.out(), "x={0:d}y={1:d}", vec.x, vec.y);
		}

		// ����ȊO
		return std::format_to(ctx.out(), "x={0:}y={1:}", vec.x, vec.y);
	}
};

/// <summary>
/// Vector3�pstd::formatter
/// </summary>
/// <typeparam name="CharT"> ������̌^ </typeparam>
template<Eugene::ValueC T>
class std::formatter<Eugene::Vector3Tmp<T>> :
	public std::formatter<const char*>
{
public:

	/// <summary>
	/// �t�H�[�}�b�g(x=0y=0z=0�݂����ɕ\������)
	/// </summary>
	/// <typeparam name="Out"></typeparam>
	/// <param name="vec"></param>
	/// <param name="ctx"></param>
	/// <returns></returns>
	template<class Out>
	auto format(const Eugene::Vector3Tmp<T>& vec, std::basic_format_context<Out, char>& ctx)
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			// ���������_�^�̎�
			return std::format_to(ctx.out(), "x={0:f}y={1:f}z={2:f}", vec.x , vec.y, vec.z);
		}
		else if constexpr (std::is_integral<T>::value)
		{
			// �����^�̎�
			return std::format_to(ctx.out(), "x={0:d}y={1:d}z={2:d}", vec.x, vec.y, vec.z);
		}

		// ����ȊO
		return std::format_to(ctx.out(), "x={0:}y={1:}z={2:}", vec.x, vec.y, vec.z);
	}
};

namespace Eugene
{

	class Debug
	{
	public:
		static Debug& GetInstance(void);

		/// <summary>
		/// std::format���g�p���ĕ�������f�o�b�O�o�͂���
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="fmt"></param>
		/// <param name="...args"></param>
		template<class... Args>
		constexpr void Log(std::format_string<Args...> fmt, const Args ...args)
		{
			Log(std::vformat(fmt.get(), std::make_format_args(args...)));
		}

		/// <summary>
		/// ��������f�o�b�O�o�͂���
		/// </summary>
		/// <param name="str"></param>
		void Log(const std::string& str);

	private:
		Debug();
		~Debug();
		Debug(const Debug&) = delete;
		void operator=(const Debug&) = delete;

		/// <summary>
		/// �A�N�Z�X����p�o�C�i���Z�}�t�H
		/// </summary>
		std::binary_semaphore binarySemphore_;

		/// <summary>
		/// �X���b�hID�o�͗p�o�b�t�@
		/// </summary>
		std::vector<char> buff;
	};






	
}


#else

#define DebugLog(...) 

#endif

