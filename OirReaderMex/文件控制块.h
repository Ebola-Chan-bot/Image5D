#pragma once
#include <文件映射.h>
class 文件控制块
{
	std::unique_ptr<文件映射> i内存映射;
	LONGLONG i粒度大小;
public:
	LONGLONG 粒度大小()const noexcept{ return i粒度大小; }
	文件控制块(LPCSTR 文件路径);
	文件控制块(LPCWSTR 文件路径);
	文件控制块(const 文件控制块&) = delete;
	文件映射& 内存映射()const noexcept { return *i内存映射; }
};