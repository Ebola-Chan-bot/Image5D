#pragma once
#include "Tiff标准.h"
#pragma pack(push,4)
namespace Image5D
{
	using 字节序_5D = 字节序_s<从低到高>;
	using 版本_5D = 字节序_5D::Tiff版本_s<字节序_5D::大>;
	using Tag = 版本_5D::Tag;
	template<typename T>
	using 文件偏移 = 版本_5D::文件偏移<T>;
	template<typename T>
	using R_s = 字节序_5D::R_s<T>;
	class IFD5D :public 版本_5D::IFD
	{
		IFD5D();
	public:
		Tag 图像描述;
		Tag 像素偏移;
		Tag 图像宽度;
		Tag 图像长度;
		Tag 每个样本的位数;
		Tag 每条行数;
		Tag 像素字节数;
		Tag 样本格式;
		Tag 压缩;
		Tag 光度解释;
		Tag X分辨率;
		Tag Y分辨率;
		Tag 分辨率单位;
		文件偏移<IFD5D> NextIFD;
		static IFD5D 创建(uint64_t ImageDescription长度, 文件偏移<R_s<char>>ImageDescription偏移, 文件偏移<R_s<char>> StripOffsets, uint8_t SizeP, uint16_t SizeX, uint16_t SizeY, 字节序_5D::SampleFormat SampleFormat);
	};
#pragma pack(pop)
}