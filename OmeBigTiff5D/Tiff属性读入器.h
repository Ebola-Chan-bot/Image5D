#pragma once
#include "IOmeTiff读写器.h"
using 文件指针 = std::unique_ptr<文件映射>;
class Tiff属性读入器 :virtual public IOmeTiff读写器
{
protected:
	文件指针 文件;
	像素类型 iPixelType;
	UINT16 iSizeX;
	UINT16 iSizeY;
	UINT32 iSizeI;
	文本数组 i图像描述;

	UINT8 iSizeP;
	UINT32 SizePXY;
	//必须初始化，以免随机指针
	Tiff属性读入器(文件指针& 文件, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, 文本数组& 图像描述);
public:
	像素类型 PixelType()const override{ return iPixelType; }
	UINT8 SizeP()const override{ return iSizeP; }
	UINT16 SizeX()const override{ return iSizeX; }
	UINT16 SizeY()const override{ return iSizeY; }
	UINT32 SizeI()const override{ return iSizeI; }
	const char* 图像描述()const override { return i图像描述.get();}
	virtual ~Tiff属性读入器() {};
};