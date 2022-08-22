#pragma once
#include <algorithm>
#include "Image5D异常.h"
template <typename T>
void 安全拷贝(const T* 源, size_t 数目, T* 目标)
{
	try
	{
		std::copy_n(源, 数目, 目标);
	}
	catch (...)
	{
		throw 内存异常;
	}
}