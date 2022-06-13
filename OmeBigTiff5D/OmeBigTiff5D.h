#pragma once
#include "Tiff属性读入器.h"
#include "Ome属性读入器.h"
#include <pugixml.hpp>
using namespace pugi;
class OmeBigTiff5D :public Tiff属性读入器,public Ome属性读入器
{
	//独立参数：静态构造输入并集

	const char* i文件名; //指向文档内，无需释放
	xml_node Pixels;

	//构造参数：非输入，但构造过程必需

	xml_document 图像描述文档;
	const char* 唯一标识符; //指向文档内，无需释放
	char* 像素头;

	//预计算参数：无关构造，读写中频繁使用

	UINT64 源Size10;
	UINT64 源Size210;
	OmeBigTiff5D(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, char* 图像描述, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 颜色* 通道颜色, const char* 文件名, xml_node Pixels, xml_document&& 图像描述文档, const char* 唯一标识符, char* 像素头);
public:
	Image5D异常 PixelType(像素类型)override;
	Image5D异常 SizeX(UINT16)override;
	Image5D异常 SizeY(UINT16)override;
	Image5D异常 SizeC(UINT8)override;
	Image5D异常 SizeZ(UINT8)override;
	Image5D异常 SizeT(UINT16)override;
	//如无异常，读写器取得所有权并负责释放；如有异常，所有权返还给调用方。
	Image5D异常 通道颜色(颜色*)override;
	Image5D异常 DimensionOrder(维度顺序)override;
	/*
	如无异常，读写器取得所有权并负责释放；如有异常，所有权返还给调用方。
	图像描述中的文件名字段将覆盖对象中的文件名属性，而不会修改图像描述
	*/
	Image5D异常 图像描述(char*)override;
	const char* 文件名()const { return i文件名; }
	Image5D异常 文件名(const char*);
	//如无异常，读写器取得通道颜色所有权并负责释放；如有异常，所有权返还给调用方。
	Image5D异常 修改参数(像素类型 PT, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 颜色* 通道颜色, 维度顺序 DO, const char* 文件名)override;
	Image5D异常 读入像素(char* 缓冲区, UINT32 IStart, UINT32 ISize)const override;
	Image5D异常 读入像素(char* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const override;
	Image5D异常 读入像素(char* 缓冲区, UINT16 TStart, UINT16 TSize)const override { return 读入像素(缓冲区, TStart, TSize, 0, iSizeZ, 0, iSizeC); }
	Image5D异常 写出像素(const char* 缓冲区, UINT32 IStart, UINT32 ISize)const override;
	Image5D异常 写出像素(const char* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const override;
	Image5D异常 写出像素(const char* 缓冲区, UINT16 TStart, UINT16 TSize)const override { return 写出像素(缓冲区, TStart, TSize, 0, iSizeZ, 0, iSizeC); }
	char* 像素指针(UINT32 I)const override;
	char* 像素指针(UINT16 T, UINT8 Z, UINT8 C)const override;
	static const OmeBigTiff5D* 只读打开(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针);
	static OmeBigTiff5D* 读写打开(HANDLE 文件句柄);
	//如无异常，读写器取得颜色通道所有权并负责释放；如有异常，所有权返还给调用方。
	static OmeBigTiff5D* 覆盖创建(const char* 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 颜色* 通道颜色, 维度顺序 DimensionOrder);
	//如无异常，读写器取得图像描述所有权并负责释放；如有异常，所有权返还给调用方。
	static OmeBigTiff5D* 覆盖创建(const char* 文件路径, char* 图像描述);
	virtual ~OmeBigTiff5D(){}
};