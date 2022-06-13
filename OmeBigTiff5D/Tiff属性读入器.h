#pragma once
#include "IOmeTiff读写器.h"
class Tiff属性读入器 :virtual public IOmeTiff读写器
{
protected:
	HANDLE 文件句柄;
	HANDLE 映射句柄;
	LPVOID 映射指针;
	LPVOID 尾指针;
	像素类型 iPixelType;
	UINT16 iSizeX;
	UINT16 iSizeY;
	UINT32 iSizeI;
	char* i图像描述;

	UINT8 iSizeP;
	UINT32 SizePXY;
	//必须初始化，以免随机指针
	Tiff属性读入器(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, char* 图像描述);
public:
	像素类型 PixelType()const override{ return iPixelType; }
	UINT8 SizeP()const override{ return iSizeP; }
	UINT16 SizeX()const override{ return iSizeX; }
	UINT16 SizeY()const override{ return iSizeY; }
	UINT32 SizeI()const override{ return iSizeI; }
	const char* 图像描述()const override{ return i图像描述; }
	virtual ~Tiff属性读入器();
};