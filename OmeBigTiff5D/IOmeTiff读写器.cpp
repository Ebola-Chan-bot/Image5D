#include "pch.h"
#include "Tiff读入器.h"
#include "OmeBigTiff5D.h"
const IOmeTiff读写器* 只读打开_异常安全(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针)
{
	try
	{
		return OmeBigTiff5D::只读打开(文件句柄, 映射句柄, 映射指针, 尾指针);
	}
	catch (Image5D异常) {}
	try
	{
		return Tiff读入器::只读打开<基本>(文件句柄, 映射句柄, 映射指针, 尾指针);
	}
	catch (Image5D异常 异常)
	{
		UnmapViewOfFile(映射指针);
		CloseHandle(映射句柄);
		CloseHandle(文件句柄);
		throw 异常;
	}
}
const IOmeTiff读写器* IOmeTiff读写器::只读打开(LPCSTR 文件路径)
{
	HANDLE 文件句柄;
	HANDLE 映射句柄;
	LPVOID 映射指针;
	LPVOID 尾指针;
	Image5D异常 异常 = 打开文件(文件路径, 文件句柄, 映射句柄, 映射指针, 尾指针);
	if (异常.类型 != 操作成功)
		throw 异常;
	return 只读打开_异常安全(文件句柄, 映射句柄, 映射指针, 尾指针);
}
const IOmeTiff读写器* IOmeTiff读写器::只读打开(LPCWSTR 文件路径) 
{
	HANDLE 文件句柄;
	HANDLE 映射句柄;
	LPVOID 映射指针;
	LPVOID 尾指针;
	Image5D异常 异常 = 打开文件(文件路径, 文件句柄, 映射句柄, 映射指针, 尾指针);
	if (异常.类型 != 操作成功)
		throw 异常;
	return 只读打开_异常安全(文件句柄, 映射句柄, 映射指针, 尾指针);
}
IOmeTiff读写器* IOmeTiff读写器::读写打开(LPCSTR 文件路径)
{
	return OmeBigTiff5D::读写打开(CreateFileA(文件路径, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
}
IOmeTiff读写器* IOmeTiff读写器::读写打开(LPCWSTR 文件路径)
{ 
	return OmeBigTiff5D::读写打开(CreateFileW(文件路径, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
}
//调用方负责释放
char* GetUtf8(LPCWSTR UTF16)
{
	const int 长度 = (wcslen(UTF16) + 1) * 3;
	char* const UTF8 = (char*)malloc(长度);
	WideCharToMultiByte(CP_UTF8, NULL, UTF16, -1, UTF8, 长度, NULL, NULL);
	return UTF8;
}
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(LPCSTR 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT,颜色* 通道颜色, 维度顺序 DimensionOrder)
{
	return OmeBigTiff5D::覆盖创建(文件路径, PixelType, SizeX, SizeY, SizeC, SizeZ, SizeT, 通道颜色, DimensionOrder);
}
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(LPCWSTR 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 颜色* 通道颜色, 维度顺序 DimensionOrder)
{
	char* const UTF8 = GetUtf8(文件路径);
	IOmeTiff读写器* 返回指针;
	try
	{
		返回指针 = OmeBigTiff5D::覆盖创建(UTF8, PixelType, SizeX, SizeY, SizeC, SizeZ, SizeT,通道颜色, DimensionOrder);
	}
	catch (Image5D异常 异常)
	{
		free(UTF8);
		throw 异常;
	}
	free(UTF8);
	return 返回指针;
}
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(LPCSTR 文件路径, char* 图像描述)
{
	return OmeBigTiff5D::覆盖创建(文件路径, 图像描述);
}
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(LPCWSTR 文件路径, char* 图像描述)
{
	char* const UTF8 = GetUtf8(文件路径);
	IOmeTiff读写器* 返回指针;
	try
	{
		返回指针 = OmeBigTiff5D::覆盖创建(UTF8, 图像描述);
	}
	catch (Image5D异常 异常)
	{
		free(UTF8);
		throw 异常;
	}
	free(UTF8);
	return 返回指针;
}