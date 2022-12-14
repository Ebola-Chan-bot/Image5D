#pragma once
#include"文件映射.h"
namespace Image5D
{
	extern const LONGLONG 分配粒度;
	struct 文件控制块 :文件映射
	{
		const LONGLONG 粒度大小;
		文件控制块(LPCWSTR 文件路径) :文件映射(文件路径, true), 粒度大小((文件大小() / 分配粒度 + 1)* 分配粒度) {}
	};
}