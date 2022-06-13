#pragma once
#include "Tiff读入器.h"
#include "Ome属性读入器.h"
class OmeTiff读入器 :public Tiff读入器,public Ome属性读入器
{
private:
	UINT16 源Size1;
	UINT32 源Size21;
protected:
	OmeTiff读入器(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, char* 图像描述, const char** const IFD像素指针, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 颜色* 通道颜色);
public:
	Image5D异常 读入像素(char* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const override;
	Image5D异常 读入像素(char* 缓冲区, UINT16 TStart, UINT16 TSize)const override{ return 读入像素(缓冲区, TStart, TSize, 0, iSizeZ, 0, iSizeC); }
	virtual ~OmeTiff读入器() {}
	static const OmeTiff读入器* 只读打开(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, char* 图像描述, const char** const IFD像素指针);
};