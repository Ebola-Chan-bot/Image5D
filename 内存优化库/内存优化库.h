#pragma once
#include <stdlib.h>
#include <new> // bad_alloc, bad_array_new_length
template <class T>
struct malloc可选free
{
	using value_type = T;
	bool 释放 = true;
	template<class U> malloc可选free(const malloc可选free<U>& 源) noexcept { 释放 = 源.释放; }
	T* allocate(const size_t n) const
	{
		size_t 内存 = n * sizeof(T);
		if (内存 / sizeof(T) < n)
			throw std::bad_array_new_length();
		return static_cast<T*>(malloc(内存));
	}
	void deallocate(T* const p, size_t) const noexcept
	{
		if (释放)
			free(p);
	}
	malloc可选free() noexcept {}
};
struct free删除器
{
	void operator()(void* 指针)const noexcept { free(指针); }
};