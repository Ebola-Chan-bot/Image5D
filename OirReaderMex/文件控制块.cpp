#include "pch.h"
#include "文件控制块.h"
#include "分配粒度.h"
文件控制块::文件控制块(LPCSTR 文件路径)
{
	Image5D异常 异常 = 文件映射::打开(文件路径, true, i内存映射);
	if (异常)
		throw 异常;
	i粒度大小 = (i内存映射->文件大小() / 分配粒度 + 1) * 分配粒度;
}
文件控制块::文件控制块(LPCWSTR 文件路径)
{
	Image5D异常 异常 = 文件映射::打开(文件路径, true, i内存映射);
	if (异常)
		throw 异常;
	i粒度大小 = (i内存映射->文件大小() / 分配粒度 + 1) * 分配粒度;
}