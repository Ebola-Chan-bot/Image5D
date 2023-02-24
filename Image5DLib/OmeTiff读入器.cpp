#include "OmeTiff读入器.h"
#include "OmeXml基本解析.h"
#include <vector>
using namespace Image5D;
void 预计算参数(维度顺序 iDimensionOrder, uint8_t iSizeC, uint8_t iSizeZ, uint32_t iSizeT, uint32_t& 源Size1, uint32_t& 源Size21)noexcept
{
	switch (iDimensionOrder)
	{
	case XYCZT:
		源Size1 = iSizeC;
		源Size21 = iSizeZ;
		break;
	case XYCTZ:
		源Size1 = iSizeC;
		源Size21 = iSizeT;
		break;
	case XYZCT:
		源Size1 = iSizeZ;
		源Size21 = iSizeC;
		break;
	case XYZTC:
		源Size1 = iSizeZ;
		源Size21 = iSizeT;
		break;
	case XYTCZ:
		源Size1 = iSizeT;
		源Size21 = iSizeC;
		break;
	case XYTZC:
		源Size1 = iSizeT;
		源Size21 = iSizeZ;
		break;
	}
	源Size21 *= 源Size1;
}
OmeTiff读入器::OmeTiff读入器(文件指针&& 文件, 像素类型 iPixelType, uint16_t iSizeX, uint16_t iSizeY, std::string&& 图像描述, IFD数组&& IFD像素指针, uint8_t SizeC, uint8_t SizeZ, uint32_t SizeT, 维度顺序 DimensionOrder, 颜色数组&& 通道颜色, bool 字节反转) :
	Tiff读入器(std::move(文件), iPixelType, iSizeX, iSizeY, std::move(图像描述), std::move(IFD像素指针), 字节反转),
	Ome属性读入器(SizeC, SizeZ, SizeT, DimensionOrder, std::move(通道颜色))
{
	预计算参数(iDimensionOrder, iSizeC, iSizeZ, iSizeT, 源Size1, 源Size21);
}
void OmeTiff读入器::读入像素(void* 缓冲区, uint32_t TStart, uint32_t TSize, uint8_t ZStart, uint8_t ZSize, uint8_t CStart, uint8_t CSize)const
{

	if (uint32_t(TStart) + TSize > iSizeT || uint16_t(ZStart) + ZSize > iSizeZ || uint16_t(CStart) + CSize > iSizeC)
		throw Image5D异常(指定维度越界);
	uint64_t 库Size10; uint64_t 库Size210;
	using 迭代器 = IFD数组::const_iterator;
	迭代器 头3 = IFD像素指针.cbegin();
	char* 尾3 = (char*)缓冲区;
	switch (iDimensionOrder)
	{
	case XYCZT:
		头3 += CStart + ZStart * uint32_t(源Size1) + TStart * 源Size21;
		库Size10 = CSize * uint64_t(SizePXY);
		库Size210 = ZSize * 库Size10;
		尾3 += TSize * 库Size210;
		break;
	case XYCTZ:
		头3 += CStart + TStart * uint32_t(源Size1) + ZStart * 源Size21;
		库Size10 = CSize * uint64_t(SizePXY);
		库Size210 = TSize * 库Size10;
		尾3 += ZSize * 库Size210;
		break;
	case XYZCT:
		头3 += ZStart + CStart * uint32_t(源Size1) + TStart * 源Size21;
		库Size10 = ZSize * uint64_t(SizePXY);
		库Size210 = CSize * 库Size10;
		尾3 += TSize * 库Size210;
		break;
	case XYZTC:
		头3 += ZStart + TStart * uint32_t(源Size1) + CStart * 源Size21;
		库Size10 = ZSize * uint64_t(SizePXY);
		库Size210 = TSize * 库Size10;
		尾3 += CSize * 库Size210;
		break;
	case XYTCZ:
		头3 += TStart + CStart * uint32_t(源Size1) + ZStart * 源Size21;
		库Size10 = TSize * uint64_t(SizePXY);
		库Size210 = CSize * 库Size10;
		尾3 += ZSize * 库Size210;
		break;
	case XYTZC:
		头3 += TStart + ZStart * uint32_t(源Size1) + CStart * 源Size21;
		库Size10 = TSize * uint64_t(SizePXY);
		库Size210 = ZSize * 库Size10;
		尾3 += CSize * 库Size210;
		break;
	}
	try
	{
		while (缓冲区 < 尾3)
		{
			迭代器 头2 = 头3;
			char* const 尾2 = (char*)缓冲区 + 库Size210;
			while (缓冲区 < 尾2)
			{
				迭代器 头1 = 头2;
				char* const 尾1 = (char*)缓冲区 + 库Size10;
				while (缓冲区 < 尾1)
				{
					像素拷贝(缓冲区, *(头1++), 1);
					缓冲区 = (char*)缓冲区 + SizePXY;
				}
				头2 += 源Size1;
			}
			头3 += 源Size21;
		}
	}
	catch (...)
	{
		throw 内存异常;
	}
}
OmeTiff读入器* OmeTiff读入器::只读打开(文件指针&& 文件, 像素类型 iPixelType, uint16_t iSizeX, uint16_t iSizeY, std::string&& 图像描述, IFD数组&& IFD像素指针, bool 字节反转)
{
	xml_document 图像描述文档;
	图像描述文档.load_string(图像描述.c_str());
	xml_node 节点 = 图像描述文档.child("OME");
	if (!(节点))
		throw Image5D异常(缺少OME节点);
	if (!(节点 = 节点.child("Image")))
		throw Image5D异常(缺少Image节点);
	const xml_node Pixels = 节点.child("Pixels");
	if (!Pixels)
		throw Image5D异常(缺少Pixels节点);
	uint8_t iSizeC, iSizeZ; uint32_t iSizeT; 维度顺序 iDimensionOrder; 颜色数组 iChannelColors;
	OmeXml基本解析(Pixels, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iPixelType, iChannelColors);
	if (uint32_t(iSizeC) * iSizeZ * iSizeT != IFD像素指针.size())
		throw Image5D异常(SizeCZT与SizeI不匹配);
	return new OmeTiff读入器(std::move(文件), iPixelType, iSizeX, iSizeY, std::move(图像描述), std::move(IFD像素指针), iSizeC, iSizeZ, iSizeT, iDimensionOrder, std::move(iChannelColors), 字节反转);
}