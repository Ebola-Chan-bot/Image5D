#include "pch.h"
#include "OmeTiff读入器.h"
#include "字符串常量.h"
#include "像素类型尺寸.h"
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
OmeTiff读入器::OmeTiff读入器(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, char* 图像描述, const char** const IFD像素指针, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 颜色* 通道颜色) :
	Tiff读入器(文件句柄, 映射句柄, 映射指针, 尾指针, iPixelType, iSizeX, iSizeY, iSizeI, 图像描述, IFD像素指针),
	Ome属性读入器(SizeC, SizeZ, SizeT, DimensionOrder, 通道颜色)
{
	预计算参数(iDimensionOrder, iSizeC, iSizeZ, iSizeT, 源Size1, 源Size21);
}
Image5D异常 OmeTiff读入器::读入像素(char* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const
{

	if (UINT32(TStart) + TSize > iSizeT || UINT16(ZStart) + ZSize > iSizeZ || UINT16(CStart) + CSize > iSizeC)
		return Image5D异常{ .类型 = 指定维度越界 };
	UINT64 库Size10; UINT64 库Size210;
	const char** 头3 = IFD像素指针;
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
	while (缓冲区 < 尾3)
	{
		const char** 头2 = 头3;
		char* const 尾2 = 缓冲区 + 库Size210;
		while (缓冲区 < 尾2)
		{
			const char** 头1 = 头2;
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
	return 成功;
}
#include <pugixml.hpp>
using namespace pugi;
const OmeTiff读入器* OmeTiff读入器::只读打开(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, char* 图像描述, const char** const IFD像素指针)
{
	xml_document 图像描述文档;
	图像描述文档.load_string(图像描述);
	const xml_node Pixels = 图像描述文档.child("Pixels");
	if (!Pixels)
		throw Image5D异常{ .类型 = 缺少Pixels节点 };
	xml_attribute 属性 = Pixels.attribute("SizeC");
	if (!属性)
		throw Image5D异常{ .类型 = 缺少SizeC属性 };
	const UINT8 iSizeC = 属性.as_uint();
	if (!iSizeC)
		throw Image5D异常{ .类型 = SizeC为0 };
	if (!(属性 = Pixels.attribute("SizeZ")))
		throw Image5D异常{ .类型 = 缺少SizeZ属性 };
	const UINT8  iSizeZ = 属性.as_uint();
	if (!iSizeZ )
		throw Image5D异常{ .类型 = SizeZ为0 };
	if (!(属性 = Pixels.attribute("SizeT")))
		throw Image5D异常{ .类型 = 缺少SizeT属性 };
	const UINT16 iSizeT = 属性.as_uint();
	if (!iSizeT)
		throw Image5D异常{ .类型 = SizeT为0 };
	if (UINT32(iSizeC) * iSizeZ * iSizeT != iSizeI)
		throw Image5D异常{ .类型 = SizeCZT与SizeI不匹配 };
	if (!(属性 = Pixels.attribute("DimensionOrder")))
		throw Image5D异常{ .类型 = 缺少DimensionOrder属性 };
	INT8 序号 = 寻找字符串(属性.as_string(), 维度顺序字符串);
	if (序号 < 0)
		throw Image5D异常{ .类型 = 维度顺序无效 };
	const 维度顺序 iDimensionOrder = (维度顺序)序号;
	if (!(属性 = Pixels.attribute("Type")))
		throw Image5D异常{ .类型 = 缺少Type属性 };
	序号 = 寻找字符串(属性.as_string(), 像素类型字符串);
	if (序号 < 0)
		throw Image5D异常{ .类型 = 像素类型无效 };
	if (像素类型尺寸[(UINT8)iPixelType] != 像素类型尺寸[序号])
		throw Image5D异常{ .类型 = BitsPerSample与PixelType不匹配 };
	iPixelType = (像素类型)序号;
	透明向量<xml_node>ChannelBuffer;
	for (xml_node 节点 : Pixels.children("Channel"))
		ChannelBuffer.加尾(节点);
	if (ChannelBuffer.长度 != iSizeC)
		throw Image5D异常{ .类型 = SizeC与颜色数不匹配 };
	const xml_node* const 通道节点 = ChannelBuffer.指针;
	颜色* iChannelColors = (颜色*)malloc(sizeof(颜色) * iSizeC);
	for (UINT8 C = 0; C < iSizeC; ++C)
		iChannelColors[C].整数值 = (属性 = 通道节点[C].attribute("Color")) ? 属性.as_int() : -1;
	return new OmeTiff读入器(文件句柄, 映射句柄, 映射指针, 尾指针, iPixelType, iSizeX, iSizeY, iSizeI, 图像描述, IFD像素指针, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iChannelColors);
}