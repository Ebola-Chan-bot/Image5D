#pragma once
#include "IOmeTiff读写器.h"
#include "文件映射.h"
namespace Image5D
{
	using 文件指针 = std::unique_ptr<文件映射>;
	class Tiff属性读入器 :virtual public IOmeTiff读写器
	{
	protected:
		文件指针 文件;
		像素类型 iPixelType;
		uint16_t iSizeX;
		uint16_t iSizeY;
		uint32_t iSizeI;
		std::string i图像描述;

		uint8_t iSizeP;
		uint32_t SizeXY;
		uint32_t SizePXY;
		uint64_t SizeIPXY;
		//必须初始化，以免随机指针
		Tiff属性读入器(文件指针&& 文件, 像素类型 iPixelType, uint16_t iSizeX, uint16_t iSizeY, uint32_t iSizeI, std::string&& 图像描述) :文件(std::move(文件)), iPixelType(iPixelType), iSizeX(iSizeX), iSizeY(iSizeY), iSizeI(iSizeI), i图像描述(std::move(图像描述)), iSizeP(像素类型尺寸[uint8_t(iPixelType)]), SizeXY(uint32_t(iSizeX)* iSizeY), SizePXY(iSizeP* SizeXY), SizeIPXY(uint64_t(iSizeI)* SizePXY) {}
	public:
		像素类型 PixelType()const override { return iPixelType; }
		uint8_t SizeP()const override { return iSizeP; }
		uint16_t SizeX()const override { return iSizeX; }
		uint16_t SizeY()const override { return iSizeY; }
		uint32_t SizeI()const override { return iSizeI; }
		const char* 图像描述()const override { return i图像描述.c_str(); }
		virtual ~Tiff属性读入器() {};
	};
}