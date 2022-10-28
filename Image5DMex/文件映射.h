#pragma once
#include <memory>
#include "Image5D异常.h"
class  文件映射
{
	const HANDLE 文件句柄;
	HANDLE 映射句柄;
	LPVOID i映射指针 = nullptr;
	LONGLONG i文件大小;
public:
	const bool 只读;
	//构造方法不会自动建立映射，需要手动设置映射指针，否则返回nullptr
	LPVOID 映射指针()const noexcept { return i映射指针; }
	//如果输入nullptr，将由系统自动分配映射指针
	void 映射指针(LPVOID);
	LONGLONG 文件大小()const noexcept { return i文件大小; }
	//如果当前映射指针不为nullptr，此方法可能会改变映射指针
	void 文件大小(LONGLONG 大小);
	//禁止拷贝，以免意外的析构
	文件映射(const 文件映射&) = delete;
	//请勿重复析构，会出错
	~文件映射()noexcept;
	文件映射(LPCWSTR 文件路径, bool 只读);
	文件映射(LPCWSTR 文件路径, LONGLONG 文件大小);
};