#pragma once
#include <algorithm>
#include "Image5D异常.h"
namespace Image5D
{
	template <typename T>
	void 安全拷贝(const T* 源, size_t 数目, T* 目标)
	{
		try
		{
			std::copy_n(源, 数目, 目标);
		}
		catch (...)
		{
			throw Image5D::Exception::Memory_copy_failed;
		}
	}
	inline void 安全拷贝(const void* 源, size_t 数目, void* 目标)
	{
		try
		{
			memcpy(目标, 源, 数目);
		}
		catch (...)
		{
			throw Image5D::Exception::Memory_copy_failed;
		}
	}
}