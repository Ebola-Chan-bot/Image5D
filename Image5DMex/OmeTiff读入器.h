#pragma once
#include "Tiff读入器.h"
#include "Ome属性读入器.h"
class OmeTiff读入器 :public Tiff读入器, public Ome属性读入器
{
private:
	uint32_t 源Size1;
	UINT32 源Size21;
protected:
	OmeTiff读入器(文件指针& 文件, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, std::string& 图像描述, IFD数组& IFD像素指针, UINT8 SizeC, UINT8 SizeZ, uint32_t SizeT, 维度顺序 DimensionOrder, 颜色数组& 通道颜色);
public:
	void 读入像素(char* 缓冲区, uint32_t TStart, uint32_t TSize)const override { 读入像素(缓冲区, TStart, TSize, 0, iSizeZ, 0, iSizeC); }
	void 读入像素(char* 缓冲区, uint32_t TStart, uint32_t TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const override;
	virtual ~OmeTiff读入器() {}
	static OmeTiff读入器* 只读打开(文件指针& 文件, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, std::string& 图像描述, IFD数组& IFD像素指针);
};