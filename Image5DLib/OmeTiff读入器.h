#pragma once
#include "Tiff读入器.h"
#include "Ome属性读入器.h"
namespace Image5D
{
	class OmeTiff读入器 :public Tiff读入器, public Ome属性读入器
	{
	private:
		uint32_t 源Size1;
		uint32_t 源Size21;
	protected:
		OmeTiff读入器(文件指针&& 文件, 像素类型 iPixelType, uint16_t iSizeX, uint16_t iSizeY, std::string&& 图像描述, IFD数组&& IFD像素指针, uint8_t SizeC, uint8_t SizeZ, uint32_t SizeT, 维度顺序 DimensionOrder, 颜色数组&& 通道颜色,bool 字节反转);
	public:
		void 读入像素(void* 缓冲区, uint32_t TStart, uint32_t TSize)const override { 读入像素(缓冲区, TStart, TSize, 0, iSizeZ, 0, iSizeC); }
		void 读入像素(void* 缓冲区, uint32_t TStart, uint32_t TSize, uint8_t ZStart, uint8_t ZSize, uint8_t CStart, uint8_t CSize)const override;
		virtual ~OmeTiff读入器() {}
		static OmeTiff读入器* 只读打开(文件指针&& 文件, 像素类型 iPixelType, uint16_t iSizeX, uint16_t iSizeY, std::string&& 图像描述, IFD数组&& IFD像素指针,bool 字节反转);
	};
}