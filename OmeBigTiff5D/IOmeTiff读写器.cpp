#include "pch.h"
#include "Tiff读入器.h"
#include "OmeBigTiff5D.h"
IOmeTiff读写器* 只读打开核心(std::unique_ptr<文件映射>& 映射)
{
	映射->映射指针(nullptr);
	try
	{
		return OmeBigTiff5D::只读打开(std::move(映射));
	}
	catch (Image5D异常) {}
	return Tiff读入器::只读打开<基本>(映射);
}
template<字符 T>
IOmeTiff读写器* IOmeTiff读写器::只读打开(const T* 文件路径)
{
	std::unique_ptr<文件映射> 映射;
	Image5D异常 异常 = 文件映射::打开(文件路径, true, 映射);
	if (异常)
		throw 异常;
	return 只读打开核心(映射);
}
template IOmeTiff读写器* IOmeTiff读写器::只读打开<char>(const char* 文件路径);
template IOmeTiff读写器* IOmeTiff读写器::只读打开<wchar_t>(const wchar_t* 文件路径);
template<字符 T>
IOmeTiff读写器* IOmeTiff读写器::读写打开(const T* 文件路径)
{
	文件指针 文件;
	Image5D异常 异常 = 文件映射::打开(文件路径, false, 文件);
	if (异常)
		throw 异常;
	return OmeBigTiff5D::读写打开(std::move(文件));
}
template IOmeTiff读写器* IOmeTiff读写器::读写打开<char>(const char* 文件路径);
template IOmeTiff读写器* IOmeTiff读写器::读写打开<wchar_t>(const wchar_t* 文件路径);
template<字符 T>
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(const T* 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT,const 颜色* 通道颜色, 维度顺序 DimensionOrder)
{
	return OmeBigTiff5D::覆盖创建(文件路径, PixelType, SizeX, SizeY, SizeC, SizeZ, SizeT, 通道颜色, DimensionOrder);
}
template IOmeTiff读写器* IOmeTiff读写器::覆盖创建<char>(const char* 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, const 颜色* 通道颜色, 维度顺序 DimensionOrder);
template IOmeTiff读写器* IOmeTiff读写器::覆盖创建<wchar_t>(const wchar_t* 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, const 颜色* 通道颜色, 维度顺序 DimensionOrder);
template<字符 T>
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(const T* 文件路径, const char* 图像描述)
{
	return OmeBigTiff5D::覆盖创建(文件路径, 图像描述);
}
template IOmeTiff读写器* IOmeTiff读写器::覆盖创建<char>(const char* 文件路径, const char* 图像描述);
template IOmeTiff读写器* IOmeTiff读写器::覆盖创建<wchar_t>(const wchar_t* 文件路径, const char* 图像描述);