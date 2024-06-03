#pragma once
#include <initializer_list>
#include <cstdint>
#include <concepts>
#include "EugeneLibException.h"

namespace Eugene
{
	/// <summary>
	/// �����ɂ��̂܂܂̒l�A�z��Astd::vector�ȂǓ����Ɏg����悤�ɂ���N���X(vulkanhpp��ArrayProxy���Q�l�ɂ���)
	/// </summary>
	/// <typeparam name="T"> �l�̌^ </typeparam>
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

		constexpr const T* end()
		{

		}

		constexpr std::uint64_t size() const
		{
			return size_;
		}

		const T& at(size_t index) const&
		{
			if (ptr_ == nullptr || index >= size_)
			{
				throw EugeneLibException{"�͈͊O�̃C���f�b�N�X���w�肵�Ă��܂�"};
			}
			return *(ptr_ + index);
		}

		constexpr T& operator[](size_t index)&
		{
			if (ptr_ == nullptr || index >= size_)
			{
				throw EugeneLibException{"�͈͊O�̃C���f�b�N�X���w�肵�Ă��܂�"};
			}
			return *(ptr_ + index);
		}

	private:
		const T* ptr_;
		std::uint64_t size_;
	};

}