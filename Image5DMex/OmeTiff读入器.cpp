#include "pch.h"
#include "OmeTiff读入器.h"
#include <vector>
#include "OmeXml基本解析.h"
void 预计算参数(维度顺序 iDimensionOrder, UINT8 iSizeC, UINT8 iSizeZ, UINT16 iSizeT, UINT16& 源Size1, UINT32& 源Size21)noexcept
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
OmeTiff读入器::OmeTiff读入器(文件指针& 文件, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, std::string& 图像描述, IFD数组& IFD像素指针, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 颜色数组& 通道颜色) :
	Tiff读入器(文件, iPixelType, iSizeX, iSizeY, 图像描述, IFD像素指针),
	Ome属性读入器(SizeC, SizeZ, SizeT, DimensionOrder, 通道颜色)
{
	预计算参数(iDimensionOrder, iSizeC, iSizeZ, iSizeT, 源Size1, 源Size21);
}
void OmeTiff读入器::读入像素(char* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const
{

	if (UINT32(TStart) + TSize > iSizeT || UINT16(ZStart) + ZSize > iSizeZ || UINT16(CStart) + CSize > iSizeC)
		throw Image5D异常(指定维度越界);
	UINT64 库Size10; UINT64 库Size210;
	using 迭代器 = IFD数组::const_iterator;
	迭代器 头3 = IFD像素指针.cbegin();
	char* 尾3 = 缓冲区;
	switch (iDimensionOrder)
	{
	case XYCZT:
		头3 += CStart + ZStart * UINT32(源Size1) + TStart * 源Size21;
		库Size10 = CSize * UINT64(SizePXY);
		库Size210 = ZSize * 库Size10;
		尾3 += TSize * 库Size210;
		break;
	case XYCTZ:
		头3 += CStart + TStart * UINT32(源Size1) + ZStart * 源Size21;
		库Size10 = CSize * UINT64(SizePXY);
		库Size210 = TSize * 库Size10;
		尾3 += ZSize * 库Size210;
		break;
	case XYZCT:
		头3 += ZStart + CStart * UINT32(源Size1) + TStart * 源Size21;
		库Size10 = ZSize * UINT64(SizePXY);
		库Size210 = CSize * 库Size10;
		尾3 += TSize * 库Size210;
		break;
	case XYZTC:
		头3 += ZStart + TStart * UINT32(源Size1) + CStart * 源Size21;
		库Size10 = ZSize * UINT64(SizePXY);
		库Size210 = TSize * 库Size10;
		尾3 += CSize * 库Size210;
		break;
	case XYTCZ:
		头3 += TStart + CStart * UINT32(源Size1) + ZStart * 源Size21;
		库Size10 = TSize * UINT64(SizePXY);
		库Size210 = CSize * 库Size10;
		尾3 += ZSize * 库Size210;
		break;
	case XYTZC:
		头3 += TStart + ZStart * UINT32(源Size1) + CStart * 源Size21;
		库Size10 = TSize * UINT64(SizePXY);
		库Size210 = ZSize * 库Size10;
		尾3 += CSize * 库Size210;
		break;
	}
	try
	{
		while (缓冲区 < 尾3)
		{
			迭代器 头2 = 头3;
			char* const 尾2 = 缓冲区 + 库Size210;
			while (缓冲区 < 尾2)
			{
				迭代器 头1 = 头2;
				char* const 尾1 = 缓冲区 + 库Size10;
				while (缓冲区 < 尾1)
				{
					memcpy(缓冲区, *(头1++), SizePXY);
					缓冲区 += SizePXY;
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
OmeTiff读入器* OmeTiff读入器::只读打开(文件指针& 文件, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, std::string& 图像描述, IFD数组& IFD像素指针)
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
	UINT8 iSizeC, iSizeZ; uint16_t iSizeT; 维度顺序 iDimensionOrder; 颜色数组 iChannelColors;
	OmeXml基本解析(Pixels, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iPixelType, iChannelColors);
	if (UINT32(iSizeC) * iSizeZ * iSizeT != IFD像素指针.size())
		throw Image5D异常(SizeCZT与SizeI不匹配);
	return new OmeTiff读入器(文件, iPixelType, iSizeX, iSizeY, 图像描述, IFD像素指针, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iChannelColors);
}