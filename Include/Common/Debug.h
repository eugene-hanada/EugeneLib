#pragma once
#include <concepts>
#include <string>
#include <filesystem>
#include <format>
#include <semaphore>
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#ifdef _DEBUG

#define DebugLog(...) (Eugene::Debug::GetInstance().Log(__VA_ARGS__))

/// <summary>
/// Vector2�pstd::formatter
/// </summary>
/// <typeparam name="CharT"> ������̌^ </typeparam>
template<Eugene::ValueC T, class CharT>
class std::formatter<Eugene::Vector2Tmp<T>, CharT> :
	public std::formatter<const CharT*>
{
public:

	template<class Out>
	auto format(const Eugene::Vector2Tmp<T>& vec, std::basic_format_context<Out, CharT>& ctx)
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
template<Eugene::ValueC T, class CharT>
class std::formatter<Eugene::Vector3Tmp<T>, CharT>
{

	constexpr CharT* operator"" _charT(const CharT * str, std::size_t length)
	{
		return str;
	}

	enum class Type
	{
		NON,
		RAD,
		DEG
	};
public:

	constexpr auto parse(std::basic_format_parse_context<CharT>& ctx)
	{
		auto it = ctx.begin();
		if (*it == _charT'��')
		{
			++it;
			if (*it = _charT'd')
			{

			}
		}
		it++;
		return it;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <typeparam name="Out"></typeparam>
	/// <param name="vec"></param>
	/// <param name="ctx"></param>
	/// <returns></returns>
	template<class Out>
	auto format(const Eugene::Vector3Tmp<T>& vec, std::basic_format_context<Out, CharT>& ctx)
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			// ���������_�^�̎�
			return std::format_to(ctx.out(), _charT"x={0:f}y={1:f}z={2:f}", vec.x * , vec.y, vec.z);
		}
		else if constexpr (std::is_integral<T>::value)
		{
			// �����^�̎�
			return std::format_to(ctx.out(), _charT"x={0:d}y={1:d}z={2:d}", vec.x, vec.y, vec.z);
		}

		// ����ȊO
		return std::format_to(ctx.out(), _charT"x={0:}y={1:}z={2:}", vec.x, vec.y, vec.z);
	}
private:

	Type type_;
	static constexpr CharT nonFmt{ _charT"x={0:f}y={1:f}z={2:f}" };
};

namespace Eugene
{

	class Debug
	{
	public:
		static Debug& GetInstance(void);

		/// <summary>
		/// std::format�ŏ����w�肵����������f�o�b�O�o�͂���
		/// </summary>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="formatStr"> �t�H�[�}�b�g </param>
		/// <param name="...args"> ���� </param>
		template<class ...Args>
		constexpr void Log(const std::string& formatStr, const Args&... args)
		{
			Log(std::format(formatStr, args...));
		}

		/// <summary>
		/// ��������f�o�b�O�o�͂���
		/// </summary>
		/// <param name="str"></param>
		void Log(const std::string& str);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="str"></param>
		void Log(const char str[]);

	private:
		Debug();
		~Debug();
		Debug(const Debug&) = delete;
		void operator=(const Debug&) = delete;
		std::binary_semaphore binarySemphore_;
	};






	
}


#else

#define DebugLog(...) 

#endif

