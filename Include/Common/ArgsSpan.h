#pragma once
#include <initializer_list>
#include <cstdint>
#include <concepts>

namespace Eugene
{
	/// <summary>
	/// 引数にそのままの値、配列、std::vectorなど同時に使えるようにするクラス(vulkanhppのArrayProxyを参考にした)
	/// </summary>
	/// <typeparam name="T"> 値の型 </typeparam>
	template<class T>
	class ArgsSpan
	{
	public:
		ArgsSpan() :
			ptr_{ nullptr }, size_{ 0ull }
		{
		}

		constexpr ArgsSpan(const T& t) :
			ptr_{ &t }
		{
			size_ = 1ull;
		}

		constexpr ArgsSpan(std::initializer_list<T> inilist) :
			ptr_{ inilist.begin() }
		{
			size_ = inilist.size();
		}

		template<size_t N>
		constexpr ArgsSpan(T const (&ptr)[N]) :
			ptr_{ ptr }, size_{ N }
		{
		}

		template<class Container>
		ArgsSpan(const Container& v) :
			ptr_{ v.data() }, size_{ v.size() }
		{

		}

		constexpr const T* begin() const
		{
			return ptr_;
		}

		constexpr std::uint64_t size() const
		{
			return size_;
		}

	private:
		const T* ptr_;
		std::uint64_t size_;
	};
}