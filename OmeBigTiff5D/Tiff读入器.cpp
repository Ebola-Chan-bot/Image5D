#include "pch.h"
#include "OmeTiff读入器.h"
#include <stdlib.h>
#include "Tiff文件头.h"
#include "像素类型尺寸.h"
#include <vector>
void Tiff读入器::读入像素(char* 缓冲区, UINT32 IStart, UINT32 ISize)const
{
	if (IStart + ISize > iSizeI)
		throw Image5D异常(指定维度越界);
	const char** 像素头 = IFD像素指针.get(); +IStart;
	const char** const 像素尾 = 像素头 + ISize;
	for (; 像素头 < 像素尾; ++像素头)
	{
		memcpy(缓冲区, *像素头, SizePXY);
		缓冲区 += SizePXY;
	}
}
template<typename T, UINT8 N>
constexpr UINT8 Get数组长度(T(&)[N])
{
	return N;
}
template <版本 V>
const Tiff读入器* Tiff读入器::只读打开(文件指针&文件)
{
	const Tiff文件头<V>* 文件头 = (Tiff文件头<V>*)文件->映射指针();
	const void* 尾指针 = (char*)文件头 + 文件->文件大小();
	if (文件头 + 1 > 尾指针)
		throw Image5D异常(文件头不完整);
	IFD迭代器<V> 当前IFD = 文件头->begin();
	if (当前IFD + 1 > 尾指针)
		throw Image5D异常(头IFD不完整);
	const Tag<V>* 当前标签 = 当前IFD->begin();
	const Tag<V>* 标签尾 = 当前IFD->end();
	if (标签尾 > 尾指针)
		throw Image5D异常(头IFD不完整);
	UINT8 完成进度 = 0;
	const char* 像素指针;
	malloc可选free<const char*>分配器;
	std::vector<const char*, malloc可选free<const char*>> IFD像素向量(分配器);
	UINT8 iSizeP;
	像素类型 iPixelType;
	UINT16 iSizeX;
	UINT16 iSizeY;
	文本数组 图像描述;
	while (当前标签 < 标签尾)
	{
		switch (当前标签->Identifier)
		{
		case TagID::StripOffsets:
			像素指针 = 当前标签->ASCII偏移(文件头);
			if (像素指针 > 尾指针)
				throw Image5D异常(像素偏移超出文件范围);
			IFD像素向量.push_back(像素指针);
			完成进度++;
			break;
		case TagID::BitsPerSample:
			iSizeP = 当前标签->BYTE值 / 8;
			for (UINT8 P = 0; P < Get数组长度(像素类型尺寸); ++P)
				if (像素类型尺寸[P] == iSizeP)
				{
					iPixelType = 像素类型(P);
					break;
				}
			完成进度++;
			break;
		case TagID::ImageLength:
			iSizeY = 当前标签->SHORT值;
			完成进度++;
			break;
		case TagID::ImageWidth:
			iSizeX = 当前标签->SHORT值;
			完成进度++;
			break;
		case TagID::ImageDescription:
		{
			const char* const ImageDescription = 当前标签->ASCII偏移(文件头);
			const UINT32 图像描述字节数 = 当前标签->NoValues;
			if (ImageDescription + 图像描述字节数 > 尾指针)
				throw Image5D异常(图像描述不完整);
			图像描述.reset((char*)malloc(图像描述字节数 + 1));
			memcpy(图像描述.get(), ImageDescription, 图像描述字节数);
			图像描述[图像描述字节数] = 0;
		}
			break;
		}
		当前标签++;
	}
	const UINT32 SizePXY = UINT32(iSizeP) * iSizeX * iSizeY;
	if (像素指针 + SizePXY > 尾指针)
		throw Image5D异常(像素块不完整);
	if (完成进度 < 4)
		throw Image5D异常(缺少必需标签);
	while (++当前IFD && 当前IFD + 1 <= 尾指针)
	{
		当前标签 = 当前IFD->begin();
		标签尾 = 当前IFD->end();
		if (标签尾 > 尾指针)
			break;
		while (当前标签 < 标签尾)
		{
			if (当前标签->Identifier == TagID::StripOffsets)
			{
				if ((像素指针 = 当前标签->ASCII偏移(文件头)) + SizePXY > 尾指针)
					throw Image5D异常(像素块不完整);
				IFD像素向量.push_back(像素指针);
				break;
			}
			当前标签++;
		}
	}
	IFD数组 IFD像素指针(IFD像素向量.data());
	分配器.释放 = false;
	try
	{
		return OmeTiff读入器::只读打开(文件, iPixelType, iSizeX, iSizeY, IFD像素向量.size(), 图像描述, IFD像素指针);
	}
	catch (Image5D异常)
	{
		return new Tiff读入器(文件, iPixelType, iSizeX, iSizeY, IFD像素向量.size(), 图像描述, IFD像素指针);
	}
}
template<>
const Tiff读入器* Tiff读入器::只读打开<基本>(文件指针&文件)
{
	switch (((Tiff文件头<基本>*)文件->映射指针())->版本号)
	{
	case 小:
		return Tiff读入器::只读打开<小>(文件);
		break;
	case 大:
		return Tiff读入器::只读打开<大>(文件);
		break;
	default:
		throw Image5D异常(不支持的版本号);
	}
}