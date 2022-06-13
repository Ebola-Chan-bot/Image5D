#include "pch.h"
#include "Image5D.h"
LPVOID Get尾指针(HANDLE 文件句柄, LPVOID 映射指针)
{
	LARGE_INTEGER 文件大小;
	文件大小.QuadPart = 0;
	SetFilePointerEx(文件句柄, 文件大小, &文件大小, FILE_END);
	return (char*)映射指针 + 文件大小.QuadPart;
}
Image5D异常 打开文件_核心(HANDLE 文件句柄, HANDLE& 映射句柄, LPVOID& 映射指针, LPVOID& 尾指针)
{
	if ((映射句柄 = CreateFileMapping(文件句柄, NULL, PAGE_READONLY, 0, 0, NULL)) == INVALID_HANDLE_VALUE)
		return Image5D异常{ .Win32错误代码 = GetLastError() ,.类型 = 内存映射失败 };
	if ((映射指针 = MapViewOfFile(映射句柄, FILE_MAP_READ, 0, 0, 0)) == NULL)
		return Image5D异常{ .Win32错误代码 = GetLastError() ,.类型 = 内存映射失败 };
	尾指针 = Get尾指针(文件句柄, 映射指针);
	return Image5D异常{ .类型 = 操作成功 };
}
Image5D异常 打开文件(HANDLE 文件句柄, HANDLE& 映射句柄, LPVOID& 映射指针, LPVOID& 尾指针)
{
	if (文件句柄 == INVALID_HANDLE_VALUE)
		return Image5D异常{ .Win32错误代码 = GetLastError() ,.类型 = 文件打开失败 };
	映射句柄 = INVALID_HANDLE_VALUE;
	Image5D异常 异常 = 打开文件_核心(文件句柄, 映射句柄, 映射指针, 尾指针);
	if (异常.类型 != 操作成功)
	{
		CloseHandle(映射句柄);
		CloseHandle(文件句柄);
	}
	return 异常;
}
Image5D异常 打开文件(LPCSTR 文件路径, HANDLE& 文件句柄, HANDLE& 映射句柄, LPVOID& 映射指针, LPVOID& 尾指针)
{
	return 打开文件(CreateFileA(文件路径, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL), 映射句柄, 映射指针, 尾指针);
}
Image5D异常 打开文件(LPCWSTR 文件路径, HANDLE& 文件句柄, HANDLE& 映射句柄, LPVOID& 映射指针, LPVOID& 尾指针)
{
	return 打开文件(CreateFileW(文件路径, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL), 映射句柄, 映射指针, 尾指针);
}