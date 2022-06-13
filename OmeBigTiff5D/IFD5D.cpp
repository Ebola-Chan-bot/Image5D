#include "pch.h"
#include "IFD5D.h"
IFD5D::IFD5D() :
	图像描述(Tag{ .Identifier = TagID::ImageDescription, .DataType = TagType::ASCII }),
	像素偏移(Tag{ .Identifier = TagID::StripOffsets, .DataType = TagType::LONG8, .NoValues = 1 }),
	图像宽度(Tag{ .Identifier = TagID::ImageWidth, .DataType = TagType::SHORT, .NoValues = 1 }),
	图像长度(Tag{ .Identifier = TagID::ImageLength, .DataType = TagType::SHORT, .NoValues = 1 }),
	每个样本的位数(Tag{ .Identifier = TagID::BitsPerSample, .DataType = TagType::SHORT, .NoValues = 1 }),
	每条行数(Tag{ .Identifier = TagID::RowsPerStrip, .DataType = TagType::SHORT, .NoValues = 1 }),
	像素字节数(Tag{ .Identifier = TagID::StripByteCounts, .DataType = TagType::LONG, .NoValues = 1 }),
	样本格式(Tag{ .Identifier = TagID::SampleFormat, .DataType = TagType::SHORT, .NoValues = 1 }),
	压缩(Tag{ .Identifier = TagID::Compression, .DataType = TagType::SHORT, .NoValues = 1, .Compression枚举 = Compression::NoCompression }),
	光度解释(Tag{ .Identifier = TagID::PhotometricInterpretation, .DataType = TagType::SHORT, .NoValues = 1, .PhotometricInterpretation枚举 = PhotometricInterpretation::BlackIsZero }),
	X分辨率(Tag{ .Identifier = TagID::XResolution, .DataType = TagType::RATIONAL, .NoValues = 1, .RATIONAL值 = Rational{.Numerator = 1,.Denominator = 1 } }),
	Y分辨率(Tag{ .Identifier = TagID::YResolution, .DataType = TagType::RATIONAL, .NoValues = 1, .RATIONAL值 = Rational{.Numerator = 1,.Denominator = 1 } }),
	分辨率单位(Tag{ .Identifier = TagID::ResolutionUnit, .DataType = TagType::SHORT, .NoValues = 1, .SHORT值 = UINT16(ResolutionUnit::NoUnit) }),
	NextIFD()
{
	标签数 = 13;
}
IFD5D IFD5D::创建(UINT64 ImageDescription长度, 文件偏移<char>ImageDescription偏移, 文件偏移<char> StripOffsets, UINT8 SizeP, UINT16 SizeX, UINT16 SizeY, SampleFormat SF)
{
	static IFD5D 模板 = IFD5D();
	IFD5D 新对象 = 模板;
	新对象.图像描述.NoValues = ImageDescription长度;
	新对象.图像描述.ASCII偏移 = ImageDescription偏移;
	新对象.像素偏移.ASCII偏移 = StripOffsets;
	新对象.图像宽度.SHORT值 = SizeX;
	新对象.图像长度.SHORT值 = SizeY;
	新对象.每个样本的位数.SHORT值 = SizeP * 8;
	新对象.每条行数.SHORT值 = SizeY;
	新对象.像素字节数.LONG值 = UINT32(SizeP) * SizeX * SizeY;
	新对象.样本格式.SampleFormat枚举 = SF;
	return 新对象;
}