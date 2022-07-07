#include "pch.h"
#include "Tiff读入器.h"
#include "OmeBigTiff5D.h"
const IOmeTiff读写器* 只读打开核心(std::unique_ptr<文件映射>& 映射)
{
	映射->映射指针(nullptr);
	try
	{
		return OmeBigTiff5D::只读打开(std::move(映射));
	}
	catch (Image5D异常) {}
	return Tiff读入器::只读打开<基本>(映射);
}
const IOmeTiff读写器* IOmeTiff读写器::只读打开(LPCSTR 文件路径)
{
	std::unique_ptr<文件映射> 映射;
	Image5D异常 异常 = 文件映射::打开(文件路径, true, 映射);
	if (异常.类型 != 操作成功)
		throw 异常;
	return 只读打开核心(映射);
}
const IOmeTiff读写器* IOmeTiff读写器::只读打开(LPCWSTR 文件路径) 
{
	std::unique_ptr<文件映射> 映射;
	Image5D异常 异常 = 文件映射::打开(文件路径, true, 映射);
	if (异常.类型 != 操作成功)
		throw 异常;
	return 只读打开核心(映射);
}
IOmeTiff读写器* IOmeTiff读写器::读写打开(LPCSTR 文件路径)
{
	文件指针 文件;
	Image5D异常 异常 = 文件映射::打开(文件路径, false, 文件);
	if (异常.类型 != 操作成功)
		throw 异常;
	return OmeBigTiff5D::读写打开(std::move(文件));
}
IOmeTiff读写器* IOmeTiff读写器::读写打开(LPCWSTR 文件路径)
{
	文件指针 文件;
	Image5D异常 异常 = 文件映射::打开(文件路径, false, 文件);
	if (异常.类型 != 操作成功)
		throw 异常;
	return OmeBigTiff5D::读写打开(std::move(文件));
}
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(LPCSTR 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT,const 颜色* 通道颜色, 维度顺序 DimensionOrder)
{
	return OmeBigTiff5D::覆盖创建(文件路径, PixelType, SizeX, SizeY, SizeC, SizeZ, SizeT, 通道颜色, DimensionOrder);
}
文本数组 GetUtf8(LPCWSTR UTF16)
{
	const size_t 长度 = wcslen(UTF16) * 3 + 1;
	文本数组 返回 = std::make_unique_for_overwrite<char[]>(长度);
	WideCharToMultiByte(CP_UTF8, 0, UTF16, -1, 返回.get(), 长度, nullptr, nullptr);
	return 返回;
}
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(LPCWSTR 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, const 颜色* 通道颜色, 维度顺序 DimensionOrder)
{
	return OmeBigTiff5D::覆盖创建(GetUtf8(文件路径).get(), PixelType, SizeX, SizeY, SizeC, SizeZ, SizeT, 通道颜色, DimensionOrder);
}
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(LPCSTR 文件路径, const char* 图像描述)
{
	return OmeBigTiff5D::覆盖创建(文件路径, 图像描述);
}
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(LPCWSTR 文件路径, const char* 图像描述)
{
	return OmeBigTiff5D::覆盖创建(GetUtf8(文件路径).get(), 图像描述);
}