#include "OmeTiff读入器.h"
#include <stdlib.h>
using namespace Image5D;
void Tiff读入器::像素拷贝(void* 写出头, const void* 读入头, uint32_t 帧数)const
{
	if (字节反转)
	{
		const char* const 读入尾 = (char*)读入头 + (uint64_t)帧数 * SizePXY;
		写出头 = (char*)写出头 - 1;
		while (读入头 < 读入尾)
		{
			for (char* 写出尾 = (char*)写出头 + iSizeP; 写出尾 > 写出头; 写出尾--)
			{
				*写出尾 = *(char*)读入头;
				读入头 = (char*)读入头 + 1;
			}
			写出头 = (char*)写出头 + iSizeP;
		}
	}
	else
		memcpy(写出头, 读入头, (uint64_t)帧数 * SizePXY);
}
void Tiff读入器::读入像素(void* 缓冲区)const
{
	try
	{
		for (const void* 指针 : IFD像素指针)
		{
			像素拷贝(缓冲区, 指针, 1);
			缓冲区 = (char*)缓冲区 + SizePXY;
		}
	}
	catch (...)
	{
		throw Exception::Memory_copy_failed;
	}
}
void Tiff读入器::读入像素I(void* 缓冲区, uint32_t IStart, uint32_t ISize)const
{
	if (IStart + ISize > iSizeI)
		throw Exception::Specified_dimension_out_of_bounds;
	IFD数组::const_iterator 像素头 = IFD像素指针.cbegin() + IStart;
	const IFD数组::const_iterator 像素尾 = 像素头 + ISize;
	try
	{
		while (像素头 < 像素尾)
		{
			像素拷贝(缓冲区, *(像素头++), 1);
			缓冲区 = (char*)缓冲区 + SizePXY;
		}
	}
	catch (...)
	{
		throw Exception::Memory_copy_failed;
	}
}
template<typename T, uint8_t N>
constexpr uint8_t Get数组长度(T(&)[N])
{
	return N;
}
template<字节序_e O, 字节序_s<O>::Tiff版本_e V>
Tiff读入器* 只读打开模板(文件指针&& 文件)
{
	using 字节序 = 字节序_s<O>;
	using 版本 = 字节序::template Tiff版本_s<V>;
	const 字节序::template Tiff文件头<V>* 文件头 = (字节序::template Tiff文件头<V>*)(文件->映射指针());
	const void* 尾指针 = (char*)文件头 + 文件->文件大小();
	if (文件头 + 1 > 尾指针)
		throw Exception::File_header_is_incomplete;
	typename 版本::IFD迭代器 当前IFD = 文件头->begin();
	if (当前IFD + 1 > 尾指针)
		throw Exception::Head_IFD_incomplete;
	const 版本::Tag* 当前标签 = 当前IFD->begin();
	const 版本::Tag* 标签尾 = 当前IFD->end();
	if (标签尾 > 尾指针)
		throw Exception::Head_IFD_incomplete;
	uint8_t 完成进度 = 0;
	IFD数组 IFD像素向量;	uint8_t iSizeP;	像素类型 iPixelType;	uint16_t iSizeX;	uint16_t iSizeY;	std::string 图像描述; const void* 像素指针;
	using TagID = 字节序::TagID;
	using TagType = 字节序::TagType;
	while (当前标签 < 标签尾)
	{
		switch (当前标签->Identifier)
		{
		case TagID::StripOffsets:
			像素指针 = 当前标签->ASCII偏移(文件头);
			if (像素指针 > 尾指针)
				throw Exception::Pixel_offset_out_of_file_range;
			IFD像素向量.push_back(像素指针);
			完成进度++;
			break;
		case TagID::BitsPerSample:
			iSizeP = 当前标签->SHORT值 / 8;
			for (uint8_t P = 0; P < Get数组长度(像素类型尺寸); ++P)
				if (像素类型尺寸[P] == iSizeP)
				{
					iPixelType = 像素类型(P);
					break;
				}
			完成进度++;
			break;
		case TagID::ImageLength:
			switch (当前标签->DataType)
			{
			case TagType::SHORT:
				iSizeY = 当前标签->SHORT值;
				break;
			case TagType::LONG:
				iSizeY = 当前标签->LONG值;
				break;
			default:
				throw Exception::Unexpected_label_value_type;
			}
			完成进度++;
			break;
		case TagID::ImageWidth:
			switch (当前标签->DataType)
			{
			case TagType::SHORT:
				iSizeX = 当前标签->SHORT值;
				break;
			case TagType::LONG:
				iSizeX = 当前标签->LONG值;
				break;
			default:
				throw Exception::Unexpected_label_value_type;
			}
			完成进度++;
			break;
		[[unlikely]] case TagID::ImageDescription:
		{
			const char* const ImageDescription = (char*)当前标签->ASCII偏移(文件头);
			const uint32_t 图像描述字节数 = 当前标签->NoValues;
			if (ImageDescription + 图像描述字节数 > 尾指针)
				throw Exception::The_image_description_is_incomplete;
			图像描述.assign(ImageDescription, 图像描述字节数);
		}
		break;
		[[unlikely]] case TagID::Compression:
			if (当前标签->Compression枚举 != 字节序::Compression::NoCompression)
				throw Exception::Image_bytes_compressed_not_supported;
		}
		当前标签++;
	}
	const uint32_t SizePXY = uint32_t(iSizeP) * iSizeX * iSizeY;
	if ((char*)像素指针 + SizePXY > 尾指针)
		throw Exception::The_pixel_block_is_incomplete;
	if (完成进度 < 4)
		throw Exception::Missing_required_labels;
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
				if ((char*)(像素指针 = 当前标签->BYTE偏移(文件头)) + SizePXY > 尾指针)
					throw Exception::The_pixel_block_is_incomplete;
				IFD像素向量.push_back(像素指针);
				break;
			}
			当前标签++;
		}
	}
	try
	{
		return OmeTiff读入器::只读打开(std::move(文件), iPixelType, iSizeX, iSizeY, std::move(图像描述), std::move(IFD像素向量), O == 从高到低);
	}
	catch (Image5D::Exception)
	{
		return new Tiff读入器(std::move(文件), iPixelType, iSizeX, iSizeY, std::move(图像描述), std::move(IFD像素向量), O == 从高到低);
	}
}
Tiff读入器* Tiff读入器::只读打开(文件指针&& 文件)
{
	//constexpr uint8_t 尺寸 = sizeof(字节序_s<从高到低>::Tiff版本_s<字节序_s<从高到低>::Tiff版本_e::小>::Tag);
	if (*(字节序_e*)文件->映射指针() == 从高到低)
	{
		using 字节序 = 字节序_s<从高到低>;
		if (((字节序::Tiff文件头<字节序::基本>*)文件->映射指针())->版本号 == 字节序::小)
			return 只读打开模板<从高到低, 字节序::小>(std::move(文件));
		else[[likely]]
			return 只读打开模板<从高到低, 字节序::大>(std::move(文件));
	}
	else[[likely]]
	{
		using 字节序 = 字节序_s<从低到高>;
		if (((字节序::Tiff文件头<字节序::基本>*)文件->映射指针())->版本号 == 字节序::小)
			return 只读打开模板<从低到高, 字节序::小>(std::move(文件));
		else[[likely]]
			return 只读打开模板<从低到高, 字节序::大>(std::move(文件));
	}
}