#include "pch.h"
#include <rpc.h>
#include <vector>
#include "安全拷贝.h"
#include "OmeBigTiff5D.h"
#include "resource.h"
#include "Tiff文件头.h"
#include "IFD5D.h"
#include "OmeXml基本解析.h"
constexpr char 通道ID模板[] = "Channel:0:%u";
#pragma pack(push,1)
struct OmeBigTiff5D文件头 :public Tiff文件头<基本>
{
	UINT16 偏移尺寸 = 8;
	UINT16 留空 = 0;
	文件偏移<大, IFD5D> 首IFD偏移;
	UINT8 规范5D = 79;
	constexpr OmeBigTiff5D文件头() :Tiff文件头<基本>(大) {}
};
#pragma pack(pop)
void 预计算参数(维度顺序 iDimensionOrder, UINT8 iSizeC, UINT8 iSizeZ, uint32_t iSizeT, UINT64 SizePXY, UINT64& 源Size10, UINT64& 源Size210)noexcept
{
	switch (iDimensionOrder)
	{
	case XYCZT:
		源Size10 = iSizeC * SizePXY;
		源Size210 = iSizeZ * 源Size10;
		break;
	case XYCTZ:
		源Size10 = iSizeC * SizePXY;
		源Size210 = iSizeT * 源Size10;
		break;
	case XYZCT:
		源Size10 = iSizeZ * SizePXY;
		源Size210 = iSizeC * 源Size10;
		break;
	case XYZTC:
		源Size10 = iSizeZ * SizePXY;
		源Size210 = iSizeT * 源Size10;
		break;
	case XYTCZ:
		源Size10 = iSizeT * SizePXY;
		源Size210 = iSizeC * 源Size10;
		break;
	case XYTZC:
		源Size10 = iSizeT * SizePXY;
		源Size210 = iSizeZ * 源Size10;
		break;
	}
}
OmeBigTiff5D::OmeBigTiff5D(文件指针& 文件, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, std::string& 图像描述, UINT8 SizeC, UINT8 SizeZ, uint32_t SizeT, 维度顺序 DimensionOrder, 颜色数组& 通道颜色, const char* 文件名, xml_node Pixels, xml_document& 图像描述文档, const char* 唯一标识符, char* 像素头)
	:Tiff属性读入器(文件, iPixelType, iSizeX, iSizeY, iSizeI, 图像描述),
	Ome属性读入器(SizeC, SizeZ, SizeT, DimensionOrder, 通道颜色),
	图像描述文档(std::move(图像描述文档)), 唯一标识符(唯一标识符), i文件名(文件名), Pixels(Pixels), 像素头(像素头)
{
	预计算参数(iDimensionOrder, iSizeC, iSizeZ, iSizeT, SizePXY, 源Size10, 源Size210);
}
//调用方负责释放iChannelColors；如果异常则无需释放
void 解析图像描述(const xml_document& 图像描述文档, const char*& 唯一标识符, xml_attribute& 文件名属性, xml_node& Pixels, UINT16& iSizeX, UINT16& iSizeY, UINT8& iSizeC, UINT8& iSizeZ, uint32_t& iSizeT, 维度顺序& iDimensionOrder, 像素类型& iPixelType, 颜色数组& iChannelColors)
{
	xml_node 节点 = 图像描述文档.child("OME");
	if (!(节点))
		throw Image5D异常(缺少OME节点);
	xml_attribute 属性;
	if (!(属性 = 节点.attribute("UUID")))
		throw Image5D异常(缺少UUID属性);
	唯一标识符 = 属性.as_string();
	if (!(节点 = 节点.child("Image")))
		throw Image5D异常(缺少Image节点);
	if (!(文件名属性 = 节点.attribute("Name")))
		throw Image5D异常(缺少Name属性);
	Pixels = 节点.child("Pixels");
	if (!Pixels)
		throw Image5D异常(缺少Pixels节点);
	if (!(属性 = Pixels.attribute("SizeX")))
		throw Image5D异常(缺少SizeX属性);
	if (!(iSizeX = 属性.as_uint()))
		throw Image5D异常(SizeX为0);
	if (!(属性 = Pixels.attribute("SizeY")))
		throw Image5D异常(缺少SizeY属性);
	if (!(iSizeY = 属性.as_uint()))
		throw Image5D异常(SizeY为0);
	OmeXml基本解析(Pixels, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iPixelType, iChannelColors);
}
inline void 设置TiffData(xml_node& TiffData, xml_node Pixels, xml_node 模板, const char* First, uint32_t I, UINT32& Index)noexcept
{
	TiffData = Pixels.append_copy(模板);
	TiffData.attribute(First).set_value(I);
	TiffData.attribute("IFD").set_value(++Index);
}
//输入的Pixels节点应当只有首个TiffData子节点并包含无需修改的必要信息
void 填充TiffData(xml_node Pixels, UINT8 SizeC, UINT8 SizeZ, uint32_t SizeT, 维度顺序 iDimensionOrder)noexcept
{
	xml_node 模板 = Pixels.child("TiffData");
	uint32_t Size0, Size1, Size2; const char* First0, * First1, * First2;
	switch (iDimensionOrder)
	{
	case XYCZT:
		Size0 = SizeC, Size1 = SizeZ, Size2 = SizeT, First0 = "FirstC", First1 = "FirstZ", First2 = "FirstT";
		break;
	case XYCTZ:
		Size0 = SizeC, Size1 = SizeT, Size2 = SizeZ, First0 = "FirstC", First1 = "FirstT", First2 = "FirstZ";
		break;
	case XYZCT:
		Size0 = SizeZ, Size1 = SizeC, Size2 = SizeT, First0 = "FirstZ", First1 = "FirstC", First2 = "FirstT";
		break;
	case XYZTC:
		Size0 = SizeZ, Size1 = SizeT, Size2 = SizeC, First0 = "FirstZ", First1 = "FirstT", First2 = "FirstC";
		break;
	case XYTCZ:
		Size0 = SizeT, Size1 = SizeC, Size2 = SizeZ, First0 = "FirstT", First1 = "FirstC", First2 = "FirstZ";
		break;
	case XYTZC:
		Size0 = SizeT, Size1 = SizeZ, Size2 = SizeC, First0 = "FirstT", First1 = "FirstZ", First2 = "FirstC";
		break;
	}
	xml_node 模板1, 模板0, TiffData;
	UINT32 Index = 0;
	for (uint32_t I0 = 1; I0 < Size0; ++I0)
		设置TiffData(TiffData, Pixels, 模板, First0, I0, Index);
	for (uint32_t I1 = 1; I1 < Size1; ++I1)
	{
		设置TiffData(模板1, Pixels, 模板, First1, I1, Index);
		for (uint32_t I0 = 1; I0 < Size0; ++I0)
			设置TiffData(TiffData, Pixels, 模板1, First0, I0, Index);
	}
	for (uint32_t I2 = 1; I2 < Size2; ++I2)
	{
		设置TiffData(模板1, Pixels, 模板, First2, I2, Index);
		for (uint32_t I0 = 1; I0 < Size0; ++I0)
			设置TiffData(TiffData, Pixels, 模板1, First0, I0, Index);
		for (uint32_t I1 = 1; I1 < Size1; ++I1)
		{
			设置TiffData(模板0, Pixels, 模板1, First1, I1, Index);
			for (uint32_t I0 = 1; I0 < Size0; ++I0)
				设置TiffData(TiffData, Pixels, 模板0, First0, I0, Index);
		}
	}
}
//返回TiffData是否被修复
bool 检修TiffData(xml_node Pixels, const char* 文件名, const char* 唯一标识符, UINT8 SizeC, UINT8 SizeZ, uint32_t SizeT, 维度顺序 DimensionOrder)
{
	xml_node 节点;
	const bool 需要修复 = !((节点 = Pixels.child("TiffData")) && (节点 = 节点.child("UUID")) && 节点.attribute("FileName"));
	if (需要修复)
	{
		std::vector<xml_node> 待删除;
		for (xml_node 节点 : Pixels.children("TiffData"))
			待删除.push_back(节点);
		for (xml_node 节点 : 待删除)
			Pixels.remove_child(节点);
		xml_document TiffData模板;
		const HRSRC 资源信息 = FindResourceW(模块句柄, MAKEINTRESOURCEW(IDR_TiffData), L"XML");
		TiffData模板.load_buffer(LockResource(LoadResource(模块句柄, 资源信息)), SizeofResource(模块句柄, 资源信息));
		节点 = Pixels.append_copy(TiffData模板.first_child()).child("UUID");
		节点.append_attribute("FileName").set_value(文件名);
		节点.text().set(唯一标识符);
		填充TiffData(Pixels, SizeC, SizeZ, SizeT, DimensionOrder);
	}
	return 需要修复;
}
//此接收器输出的字符串不含尾随0，但会预留空间。此对象析构时不会释放自己持有的缓存，调用方必须负责释放。
class XML文本接收器 :public xml_writer
{
public:
	std::string& 字符串;
	void write(const void* data, size_t size)override
	{
		字符串.append((char*)data, size);
	}
	XML文本接收器(std::string& 字符串) :字符串(字符串) {}
};
//调用方负责释放iChannelColors和i图像描述；如果异常无需释放iChannelColors
void 打开核心(const void* 映射指针, const void* 尾指针, UINT64& 最小文件尺寸, xml_document& 图像描述文档, const char*& 唯一标识符, const char*& i文件名, xml_node& Pixels, UINT16& iSizeX, UINT16& iSizeY, UINT8& iSizeC, UINT8& iSizeZ, uint32_t& iSizeT, 维度顺序& iDimensionOrder, 像素类型& iPixelType, 颜色数组& iChannelColors, std::string& i图像描述)
{
	const IFD5D* const FirstIFD = ((OmeBigTiff5D文件头*)映射指针)->首IFD偏移(映射指针);
	if (尾指针 < FirstIFD + 1)
		throw Image5D异常(头IFD不完整);
	const UINT64 NoValues = FirstIFD->图像描述.NoValues;
	const char* const iImageDescription = FirstIFD->图像描述.ASCII偏移(映射指针);
	if (尾指针 < iImageDescription + NoValues)
		throw Image5D异常(图像描述不完整);
	xml_attribute 文件名属性;
	const xml_parse_status XML异常 = 图像描述文档.load_buffer(iImageDescription, NoValues).status;
	if (XML异常)
		throw Image5D异常(图像描述解析失败, XML异常);
	解析图像描述(图像描述文档, 唯一标识符, 文件名属性, Pixels, iSizeX, iSizeY, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iPixelType, iChannelColors);
	i文件名 = 文件名属性.as_string();
	if (检修TiffData(Pixels, i文件名, 唯一标识符, iSizeC, iSizeZ, iSizeT, iDimensionOrder))
	{
		XML文本接收器 接收器(i图像描述);
		图像描述文档.save(接收器);
	}
	else
		i图像描述.assign(iImageDescription);
	最小文件尺寸 = FirstIFD->像素偏移.LONG8值 + UINT64(iSizeX) * iSizeY * iSizeC * iSizeZ * iSizeT * 像素类型尺寸[(UINT8)iPixelType];
}
OmeBigTiff5D* OmeBigTiff5D::只读打开(文件指针&& 文件)
{
	std::string i图像描述;	UINT64 最小文件尺寸; xml_document 图像描述文档; const char* 唯一标识符; const char* i文件名; xml_node Pixels; UINT16 iSizeX, iSizeY; UINT8 iSizeC, iSizeZ; 维度顺序 iDimensionOrder; 像素类型 iPixelType; 颜色数组 iChannelColors; uint32_t iSizeT;
	const char* const 映射指针 = (char*)文件->映射指针();
	打开核心(映射指针, 映射指针 + 文件->文件大小(), 最小文件尺寸, 图像描述文档, 唯一标识符, i文件名, Pixels, iSizeX, iSizeY, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iPixelType, iChannelColors, i图像描述);
	if (最小文件尺寸 > 文件->文件大小())
		throw Image5D异常(像素块不完整);
	return new OmeBigTiff5D(文件, iPixelType, iSizeX, iSizeY, UINT32(iSizeC) * iSizeZ * iSizeT, i图像描述, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iChannelColors, i文件名, Pixels, 图像描述文档, 唯一标识符, ((OmeBigTiff5D文件头*)映射指针)->首IFD偏移(映射指针)->像素偏移.ASCII偏移(映射指针));
}
OmeBigTiff5D* OmeBigTiff5D::读写打开(文件指针&& 文件)
{
	文件->映射指针(nullptr);
	std::string i图像描述; UINT64 最小文件尺寸; xml_document 图像描述文档; const char* 唯一标识符; const char* i文件名; xml_node Pixels; UINT16 iSizeX, iSizeY; UINT8 iSizeC, iSizeZ; 维度顺序 iDimensionOrder; 像素类型 iPixelType; 颜色数组 iChannelColors; uint32_t iSizeT;
	打开核心(文件->映射指针(), (char*)文件->映射指针() + 文件->文件大小(), 最小文件尺寸, 图像描述文档, 唯一标识符, i文件名, Pixels, iSizeX, iSizeY, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iPixelType, iChannelColors, i图像描述);
	if (最小文件尺寸 > 文件->文件大小())
		文件->文件大小(最小文件尺寸);
	const OmeBigTiff5D文件头* const 映射指针 = (OmeBigTiff5D文件头*)文件->映射指针();
	return new OmeBigTiff5D(文件, iPixelType, iSizeX, iSizeY, UINT32(iSizeC) * iSizeZ * iSizeT, i图像描述, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iChannelColors, i文件名, Pixels, 图像描述文档, 唯一标识符, (映射指针)->首IFD偏移(映射指针)->像素偏移.ASCII偏移(映射指针));
}
void 填充IFD(UINT32 SizeI, 文件偏移<大, IFD5D> 当前IFD偏移, const void* 映射指针, bool 必须重新生成, UINT32 SizePXY)noexcept
{
	IFD5D* 当前IFD指针 = 当前IFD偏移(映射指针);
	if (SizeI > 1)
	{
		if (必须重新生成)
		{
			当前IFD指针->NextIFD = 当前IFD偏移 += sizeof(IFD5D);
			当前IFD指针[1] = 当前IFD指针[0];
			(++当前IFD指针)->图像描述.NoValues = 0;
			当前IFD指针->图像描述.LONG8值 = 0;
		}
		else
			当前IFD指针[1].像素偏移.LONG8值 = 当前IFD指针[0].像素偏移.LONG8值;
		当前IFD指针->像素偏移.LONG8值 += SizePXY;
	}
	IFD5D* const 最后IFD指针 = 当前IFD指针 + SizeI - 1;
	while (当前IFD指针 < 最后IFD指针)
	{
		if (必须重新生成)
		{
			当前IFD指针->NextIFD = 当前IFD偏移 += sizeof(IFD5D);
			当前IFD指针[1] = 当前IFD指针[0];
		}
		else
			当前IFD指针[1].像素偏移.LONG8值 = 当前IFD指针[0].像素偏移.LONG8值;
		(++当前IFD指针)->像素偏移.LONG8值 += SizePXY;
	}
	当前IFD指针->NextIFD = 文件偏移<大, IFD5D>{ .偏移 = 0 };
}
xml_attribute 添加UUID属性(xml_node OME, char* URN)noexcept
{
	UUID uuid;
	UuidCreate(&uuid);
	RPC_CSTR UUID字符串;
	UuidToStringA(&uuid, &UUID字符串);
	strcat(URN, (char*)UUID字符串);
	RpcStringFreeA(&UUID字符串);
	xml_attribute 唯一标识符 = OME.append_attribute("UUID");
	唯一标识符.set_value(URN);
	return 唯一标识符;
}
std::string 取文件名(const wchar_t* 文件路径)noexcept
{
	const size_t 长度 = wcslen(文件路径) + 1;
	std::string 文件名;
	文件名.resize_and_overwrite(长度 * 7, [长度, 文件路径](char* 文件名指针, size_t)
		{
			const size_t 窄区长度 = 长度 * 3;
	wchar_t* const 宽文件名 = (wchar_t*)(文件名指针 + 窄区长度);
	wchar_t* const 宽扩展名 = 宽文件名 + 长度;
	_wsplitpath(文件路径, nullptr, nullptr, 宽文件名, 宽扩展名);
	wcscat(宽文件名, 宽扩展名);
	WideCharToMultiByte(CP_UTF8, 0, 宽文件名, -1, 文件名指针, 窄区长度, nullptr, nullptr);
	return strlen(文件名指针);
		});
	return 文件名;
}
void 构造文件(const char* 图像描述, UINT32 图像描述字节数, UINT32 SizeI, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, LPCWSTR 文件路径, 文件偏移<大, char>& 像素头偏移, 文件指针& 文件)
{
	文件偏移<大, IFD5D> IFD偏移对象(图像描述字节数 * 2 + sizeof(OmeBigTiff5D文件头));
	const LONGLONG 像素偏移 = IFD偏移对象 + SizeI * sizeof(IFD5D) * 2;
	const UINT32 SizePXY = UINT32(SizeX) * SizeY * 像素类型尺寸[(UINT8)PixelType];
	try
	{
		文件 = std::make_unique<文件映射>(文件路径, 像素偏移 + LONGLONG(SizePXY) * SizeI);
		文件->映射指针(nullptr);
	}
	catch (const Image5D异常& 异常)
	{
		throw Image5D异常(Tiff文件创建失败, 异常.Win32错误代码);
	}
	OmeBigTiff5D文件头* const 文件头 = (OmeBigTiff5D文件头*)文件->映射指针();
	*文件头 = OmeBigTiff5D文件头();
	文件头->首IFD偏移 = IFD偏移对象;
	文件偏移<大, char> 图像描述偏移{ .偏移 = sizeof(OmeBigTiff5D文件头) };
	strcpy(图像描述偏移(文件头), 图像描述);
	像素头偏移.偏移 = 像素偏移;
	*IFD偏移对象(文件头) = IFD5D::创建(图像描述字节数, 文件偏移<大, char>{.偏移 = sizeof(OmeBigTiff5D文件头)}, 像素头偏移, 像素类型尺寸[(UINT8)PixelType], SizeX, SizeY, 像素类型采样格式[(UINT8)PixelType]);
	填充IFD(SizeI, IFD偏移对象, 文件头, true, SizePXY);
}
OmeBigTiff5D* OmeBigTiff5D::覆盖创建(LPCWSTR 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, uint32_t SizeT, const 颜色* iChannelColors, 维度顺序 DimensionOrder)
{
	if (!(PixelType < 像素类型::缺省 && SizeX && SizeY && SizeC && SizeZ && SizeT && DimensionOrder < 维度顺序::缺省))
		throw Image5D异常(构造参数无效);
	std::string 文本 = 取文件名(文件路径);
	const HRSRC 资源信息 = FindResourceW(模块句柄, MAKEINTRESOURCEW(IDR_XmlTemplate), L"XML");
	xml_document 图像描述文档;
	图像描述文档.load_buffer(LockResource(LoadResource(模块句柄, 资源信息)), SizeofResource(模块句柄, 资源信息), parse_declaration);
	char URN[46] = "urn:uuid:";
	xml_node 节点 = 图像描述文档.child("OME");
	const char* const 唯一标识符 = 添加UUID属性(节点, URN).as_string();
	xml_attribute 属性 = (节点 = 节点.child("Image")).append_attribute("Name");
	属性.set_value(文本.c_str());
	const char* const i文件名 = 属性.as_string();
	xml_node Pixels = 节点.child("Pixels");
	Pixels.append_attribute("SizeX").set_value(SizeX);
	Pixels.append_attribute("SizeY").set_value(SizeY);
	Pixels.append_attribute("SizeC").set_value(SizeC);
	Pixels.append_attribute("SizeZ").set_value(SizeZ);
	Pixels.append_attribute("SizeT").set_value(SizeT);
	Pixels.append_attribute("DimensionOrder").set_value(维度顺序字符串[UINT8(DimensionOrder)]);
	Pixels.append_attribute("Type").set_value(像素类型字符串[UINT8(PixelType)]);
	节点 = Pixels.child("Channel");
	节点.attribute("Color").set_value(iChannelColors[0].整数值);
	char ChannelID[12];
	for (UINT8 C = 1; C < SizeC; ++C)
	{
		sprintf(ChannelID, 通道ID模板, C);
		节点 = Pixels.insert_copy_after(节点, 节点);
		节点.attribute("ID").set_value(ChannelID);
		节点.attribute("Color").set_value(iChannelColors[C].整数值);
	}
	节点 = Pixels.child("TiffData").child("UUID");
	节点.append_attribute("FileName").set_value(i文件名);
	节点.text().set(URN);
	填充TiffData(Pixels, SizeC, SizeZ, SizeT, DimensionOrder);
	const UINT32 SizeI = UINT32(SizeC) * SizeZ * SizeT;
	文本.clear();
	XML文本接收器 接收器(文本);
	图像描述文档.save(接收器);
	文件偏移<大, char> 像素头偏移; 文件指针 文件;
	构造文件(文本.c_str(), 文本.size() + 1, SizeI, PixelType, SizeX, SizeY, 文件路径, 像素头偏移, 文件);
	颜色数组 通道颜色 = std::make_unique_for_overwrite<颜色[]>(SizeC);
	std::copy_n(iChannelColors, SizeC, 通道颜色.get());
	return new OmeBigTiff5D(文件, PixelType, SizeX, SizeY, SizeI, 文本, SizeC, SizeZ, SizeT, DimensionOrder, 通道颜色, i文件名, Pixels, 图像描述文档, 唯一标识符, 像素头偏移(文件->映射指针()));
}
void 更新文件名(xml_node Pixels, const char* 新文件名)noexcept
{
	for (xml_node TiffData : Pixels.children("TiffData"))
	{
		xml_node 节点 = TiffData.child("UUID");
		if (!节点)
			节点 = TiffData.append_child("UUID");
		xml_attribute 属性 = 节点.attribute("FileName");
		if (!属性)
			属性 = 节点.append_attribute("FileName");
		属性.set_value(新文件名);
	}
}
OmeBigTiff5D* OmeBigTiff5D::覆盖创建(LPCWSTR 文件路径, const char* 图像描述)
{
	std::string 文本 = 取文件名(文件路径);
	xml_document 图像描述文档; const char* 唯一标识符; xml_attribute 文件名属性; xml_node Pixels; UINT16 iSizeX, iSizeY; UINT8 iSizeC, iSizeZ; 维度顺序 iDimensionOrder; 像素类型 iPixelType; 颜色数组 iChannelColors; uint32_t iSizeT;
	const xml_parse_status XML异常 = 图像描述文档.load_string(图像描述).status;
	if (XML异常)
		throw Image5D异常(图像描述解析失败, XML异常);
	解析图像描述(图像描述文档, 唯一标识符, 文件名属性, Pixels, iSizeX, iSizeY, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iPixelType, iChannelColors);
	文件名属性.set_value(文本.c_str());
	const char* i文件名 = 文件名属性.as_string();
	size_t 图像描述字节数 = strlen(图像描述) + 1;
	检修TiffData(Pixels, i文件名, 唯一标识符, iSizeC, iSizeZ, iSizeT, iDimensionOrder);
	更新文件名(Pixels, i文件名);
	文本.clear();
	XML文本接收器 接收器(文本);
	图像描述文档.save(接收器);
	图像描述字节数 = 文本.size() + 1;
	文件偏移<大, char> 像素头偏移; 文件指针 文件;
	const UINT32 iSizeI = UINT32(iSizeC) * iSizeZ * iSizeT;
	构造文件(文本.c_str(), 图像描述字节数, iSizeI, iPixelType, iSizeX, iSizeY, 文件路径, 像素头偏移, 文件);
	return new OmeBigTiff5D(文件, iPixelType, iSizeX, iSizeY, iSizeI, 文本, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iChannelColors, i文件名, Pixels, 图像描述文档, 唯一标识符, 像素头偏移(文件->映射指针()));
}
void OmeBigTiff5D::读入像素(char* 缓冲区)const
{
	安全拷贝(像素头, SizeIPXY, 缓冲区);
}
void OmeBigTiff5D::读入像素I(char* 缓冲区, UINT32 IStart, UINT32 ISize)const
{
	if (UINT64(IStart) + ISize > iSizeI)
		throw 越界异常;
	安全拷贝(像素头 + UINT64(IStart) * SizePXY, UINT64(ISize) * SizePXY, 缓冲区);
}
void 读写3D(维度顺序 iDimensionOrder, UINT16 TStart, uint32_t TSize, uint32_t iSizeT, UINT8 ZStart, UINT8 ZSize, UINT8 iSizeZ, UINT8 CStart, UINT8 CSize, UINT8 iSizeC, UINT32 SizePXY, UINT64 源Size10, UINT64 源Size210, char* 头3, char* 缓冲区,void(*拷贝)(void*,void*,size_t))
{
	if (UINT32(TStart) + TSize > iSizeT || UINT16(ZStart) + ZSize > iSizeZ || UINT8(CStart) + CSize > iSizeC)
		throw 越界异常;
	UINT64 库Size10, 库Size210;
	char* 尾3 = 缓冲区;
	switch (iDimensionOrder)
	{
	case XYCZT:
		头3 += CStart * UINT64(SizePXY) + ZStart * 源Size10 + TStart * 源Size210;
		库Size10 = CSize * UINT64(SizePXY);
		库Size210 = ZSize * 库Size10;
		尾3 += TSize * 库Size210;
		break;
	case XYCTZ:
		头3 += CStart * UINT64(SizePXY) + TStart * 源Size10 + ZStart * 源Size210;
		库Size10 = CSize * UINT64(SizePXY);
		库Size210 = TSize * 库Size10;
		尾3 += ZSize * 库Size210;
		break;
	case XYZCT:
		头3 += ZStart * UINT64(SizePXY) + CStart * 源Size10 + TStart * 源Size210;
		库Size10 = ZSize * UINT64(SizePXY);
		库Size210 = CSize * 库Size10;
		尾3 += TSize * 库Size210;
		break;
	case XYZTC:
		头3 += ZStart * UINT64(SizePXY) + TStart * 源Size10 + CStart * 源Size210;
		库Size10 = ZSize * UINT64(SizePXY);
		库Size210 = TSize * 库Size10;
		尾3 += CSize * 库Size210;
		break;
	case XYTCZ:
		头3 += TStart * UINT64(SizePXY) + CStart * 源Size10 + ZStart * 源Size210;
		库Size10 = TSize * UINT64(SizePXY);
		库Size210 = CSize * 库Size10;
		尾3 += ZSize * 库Size210;
		break;
	case XYTZC:
		头3 += TStart * UINT64(SizePXY) + ZStart * 源Size10 + CStart * 源Size210;
		库Size10 = TSize * UINT64(SizePXY);
		库Size210 = ZSize * 库Size10;
		尾3 += CSize * 库Size210;
		break;
	}
	try
	{
		while (缓冲区 < 尾3)
		{
			char* 头2 = 头3;
			char* const 尾2 = 缓冲区 + 库Size210;
			while (缓冲区 < 尾2)
			{
				拷贝(缓冲区, 头2, 库Size10);
				头2 += 源Size10;
				缓冲区 += 库Size10;
			}
			头3 += 源Size210;
		}
	}
	catch (...)
	{
		throw 内存异常;
	}
}
void 写读(void* 写, void* 读, size_t 数目)
{
	memcpy(写, 读, 数目);
}
void 读写(void* 读, void* 写, size_t 数目)
{
	memcpy(写, 读, 数目);
}
void OmeBigTiff5D::读入像素(char* 缓冲区, uint32_t TStart, uint32_t TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const
{
	return 读写3D(iDimensionOrder, TStart, TSize, iSizeT, ZStart, ZSize, iSizeZ, CStart, CSize, iSizeC, SizePXY, 源Size10, 源Size210, 像素头, 缓冲区,写读);
}
void OmeBigTiff5D::写出像素(const char* 缓冲区)const
{
	if (文件->只读)
		throw Image5D异常(只读打开不可写出);
	安全拷贝(缓冲区, SizeIPXY, 像素头);
}
void OmeBigTiff5D::写出像素I(const char* 缓冲区, UINT32 IStart, UINT32 ISize)const
{
	if (UINT64(IStart) + ISize > iSizeI)
		throw 越界异常;
	if (文件->只读)
		throw Image5D异常(只读打开不可写出);
	安全拷贝(缓冲区, UINT64(ISize) * SizePXY, 像素头 + UINT64(IStart) * SizePXY);
}
void OmeBigTiff5D::写出像素(const char* 缓冲区, uint32_t TStart, uint32_t TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const
{
	if (文件->只读)
		throw Image5D异常(只读打开不可写出);
	return 读写3D(iDimensionOrder, TStart, TSize, iSizeT, ZStart, ZSize, iSizeZ, CStart, CSize, iSizeC, SizePXY, 源Size10, 源Size210, 像素头, (char*)缓冲区,读写);
}
void OmeBigTiff5D::像素指针(char*& 指针, size_t& 容量)const
{
	指针 = 像素头;
	容量 = SizeIPXY;
}
void OmeBigTiff5D::像素指针I(UINT32 I, char*& 指针, size_t& 容量)const
{
	if (I < iSizeI)
	{
		容量 = size_t(I) * SizePXY;
		指针 = 像素头 + 容量;
		容量 = SizeIPXY - 容量;
	}
	else
		throw 越界异常;
}
void OmeBigTiff5D::像素指针(uint32_t T, char*& 指针, size_t& 容量)const
{
	if (T < iSizeT)
	{
		switch (iDimensionOrder)
		{
		case XYCZT:
		case XYZCT:
			容量 = 源Size210;
			break;
		case XYCTZ:
		case XYZTC:
			容量 = 源Size10;
			break;
		case XYTCZ:
		case XYTZC:
			容量 = UINT64(SizePXY);
			break;
		}
		指针 = 像素头 + (容量 *= T);
		容量 = SizeIPXY - 容量;
	}
	else
		throw 越界异常;
}
void OmeBigTiff5D::像素指针(uint32_t T, UINT8 Z, UINT8 C, char*& 指针, size_t& 容量)const
{
	if (T < iSizeT && Z < iSizeZ && C < iSizeC)
	{
		switch (iDimensionOrder)
		{
		case XYCZT:
			容量 = C * UINT64(SizePXY) + Z * 源Size10 + T * 源Size210;
			break;
		case XYCTZ:
			容量 = C * UINT64(SizePXY) + T * 源Size10 + Z * 源Size210;
			break;
		case XYZCT:
			容量 = Z * UINT64(SizePXY) + C * 源Size10 + T * 源Size210;
			break;
		case XYZTC:
			容量 = Z * UINT64(SizePXY) + T * 源Size10 + C * 源Size210;
			break;
		case XYTCZ:
			容量 = T * UINT64(SizePXY) + C * 源Size10 + Z * 源Size210;
			break;
		case XYTZC:
			容量 = T * UINT64(SizePXY) + Z * 源Size10 + C * 源Size210;
			break;
		}
		指针 = 像素头 + 容量;
		容量 = SizeIPXY - 容量;
	}
	else
		throw 越界异常;
}
void 更新图像描述并扩展文件(bool 必须重新生成, const xml_document& 图像描述文档, UINT32 SizeI, UINT32 SizePXY, 文件映射& 文件, const char* 图像描述, UINT32 图像描述字节数)noexcept
{
	OmeBigTiff5D文件头* 文件头 = (OmeBigTiff5D文件头*)文件.映射指针();
	文件偏移<大, IFD5D> 当前IFD偏移 = 文件头->首IFD偏移;
	IFD5D* FirstIFD = 当前IFD偏移(文件头);
	const 文件偏移<大, char> 图像描述偏移 = FirstIFD->图像描述.ASCII偏移;
	UINT64 像素偏移 = FirstIFD->像素偏移.LONG8值;
	if (当前IFD偏移 < 图像描述偏移 + 图像描述字节数)
	{
		const 文件偏移<大, IFD5D> 新FirstIFD偏移{ .偏移 = 图像描述偏移 + (图像描述字节数) * 2 };
		const UINT64 全IFD尺寸 = sizeof(IFD5D) * SizeI;
		if (像素偏移 < 新FirstIFD偏移 + 全IFD尺寸)
		{
			const UINT64 新像素偏移 = 新FirstIFD偏移 + 全IFD尺寸 * 2;
			const UINT64 新文件大小 = 新像素偏移 + UINT64(SizePXY) * SizeI;
			if (新文件大小 > 文件.文件大小())
				文件.文件大小(新文件大小);
			//此时的FirstTags是不对的
			像素偏移 = 新像素偏移;
		}
		//IFD偏移存储一个偏移量，不受重映射影响
		文件头 = (OmeBigTiff5D文件头*)文件.映射指针();
		memmove(新FirstIFD偏移(文件头), 当前IFD偏移(文件头), 必须重新生成 ? sizeof(IFD5D) : 全IFD尺寸);
		文件头->首IFD偏移 = 新FirstIFD偏移;
		FirstIFD = 新FirstIFD偏移(文件头);
		FirstIFD->像素偏移.BYTE偏移 = 文件偏移<大, BYTE>{ .偏移 = 像素偏移 };
	}
	strcpy(图像描述偏移(文件头), 图像描述);
	//即使空间充足，图像描述字节数也可能发生缩小
	FirstIFD->图像描述.NoValues = 图像描述字节数;
}
//返回通道数是否更新，无论颜色是否改变
bool 更新通道(UINT8 SizeC, UINT8& iSizeC, const 颜色* ChannelColors, 颜色数组& iChannelColors, xml_node Pixels)
{
	if (!SizeC)
		SizeC = iSizeC;
	const bool 通道数更新 = SizeC != iSizeC;
	if (ChannelColors)
	{
		const size_t 新尺寸 = sizeof(颜色) * SizeC;
		iChannelColors.reset((颜色*)malloc(新尺寸));
		memcpy(iChannelColors.get(), ChannelColors, 新尺寸);
	}
	if (通道数更新)
	{
		const bool 通道增加 = SizeC > iSizeC;
		UINT8 通道数 = 通道增加 ? iSizeC : SizeC;
		xml_named_node_iterator 头节点 = Pixels.children("Channel").begin();
		if (ChannelColors)
			for (UINT8 C = 0; C < 通道数; ++C)
				头节点++->attribute("Color").set_value(iChannelColors[C].整数值);
		else
		{
			for (UINT8 C = 0; C < 通道数; ++C)
				头节点++;
			if (通道增加)
				iChannelColors.reset((颜色*)realloc(iChannelColors.release(), sizeof(颜色) * SizeC));
		}
		if (通道增加)
		{
			char ChannelID[12];
			sprintf(ChannelID, 通道ID模板, iSizeC);
			xml_node 新通道 = Pixels.append_child("Channel");
			新通道.append_attribute("ID").set_value(ChannelID);
			新通道.append_attribute("SamplesPerPixel").set_value(1);
			新通道.append_attribute("Color").set_value(iChannelColors[iSizeC].整数值);
			for (UINT8 C = iSizeC + 1; C < SizeC; ++C)
			{
				sprintf(ChannelID, 通道ID模板, C);
				新通道 = Pixels.append_copy(新通道);
				新通道.attribute("ID").set_value(ChannelID);
				新通道.attribute("Color").set_value(iChannelColors[C].整数值);
			}
		}
		else
		{
			通道数 = iSizeC - SizeC;
			xml_node* const 待删除 = (xml_node*)malloc(通道数);
			for (UINT8 C = 0; C < 通道数; ++C)
				待删除[C] = *(头节点++);
			for (UINT8 C = 0; C < 通道数; ++C)
				Pixels.remove_child(待删除[C]);
			free(待删除);
		}
		iSizeC = SizeC;
		Pixels.attribute("SizeC").set_value(SizeC);
	}
	else if (ChannelColors)
	{
		xml_named_node_iterator 头节点 = Pixels.children("Channel").begin();
		for (UINT8 C = 0; C < iSizeC; ++C)
			头节点++->attribute("Color").set_value(iChannelColors[C].整数值);
	}
	return 通道数更新;
}
//这里需要的是旧的SizeI
void 重置TiffData(xml_node Pixels, UINT32 SizeI, UINT8 SizeC, UINT8 SizeZ, uint32_t SizeT, 维度顺序 DimensionOrder)noexcept
{
	xml_node* const TiffData = (xml_node*)malloc(sizeof(xml_node) * SizeI);
	xml_node* TD指针 = TiffData;
	for (xml_node TD节点 : Pixels.children("TiffData"))
		*(TD指针++) = TD节点;
	xml_node* const TD尾指针 = TiffData + SizeI;
	//第1个TiffData留做模板不能删除！
	for (TD指针 = TiffData + 1; TD指针 < TD尾指针; ++TD指针)
		Pixels.remove_child(*TD指针);
	填充TiffData(Pixels, SizeC, SizeZ, SizeT, DimensionOrder);
}
template <typename T>
	requires std::_Is_any_of_v<T, UINT8, UINT16, uint32_t>
bool 修改尺寸(T& 旧尺寸, T 新尺寸, xml_attribute 属性)
{
	const bool 修改 = 新尺寸 && 新尺寸 != 旧尺寸;
	if (修改)
	{
		旧尺寸 = 新尺寸;
		属性.set_value(新尺寸);
	}
	return 修改;
}
void OmeBigTiff5D::修改参数(像素类型 PT, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, uint32_t SizeT, const 颜色* ChannelColors, 维度顺序 DO, const char* 文件名)
{
	if (文件->只读)
		throw Image5D异常(只读打开不可修改);
	if ((PT > 像素类型::缺省 || DO > 维度顺序::缺省))
		throw Image5D异常(修改参数无效);
	const bool 像素类型改变 = PT != 像素类型::缺省 && PT != iPixelType;
	const bool 单帧尺寸改变 = 修改尺寸(iSizeX, SizeX, Pixels.attribute("SizeX")) || 修改尺寸(iSizeY, SizeY, Pixels.attribute("SizeY")) || 像素类型改变;
	const bool IFD数目改变 = 更新通道(SizeC, iSizeC, ChannelColors, i通道颜色, Pixels) || 修改尺寸(iSizeZ, SizeZ, Pixels.attribute("SizeZ")) || 修改尺寸(iSizeT, SizeT, Pixels.attribute("SizeT"));
	const bool 维度顺序改变 = DO != 维度顺序::缺省 && DO != iDimensionOrder;
	const bool IFD尺寸改变 = 单帧尺寸改变 || IFD数目改变;
	if (维度顺序改变)
	{
		iDimensionOrder = DO;
		Pixels.attribute("DimensionOrder").set_value(维度顺序字符串[(UINT8)DO]);
	}
	if (像素类型改变)
	{
		iPixelType = PT;
		Pixels.attribute("Type").set_value(像素类型字符串[(UINT8)PT]);
		iSizeP = 像素类型尺寸[(UINT8)PT];
	}
	if (文件名)
	{
		xml_attribute 属性 = 图像描述文档.child("Image").attribute("Name");
		属性.set_value(文件名);
		i文件名 = 属性.as_string();
	}
	if (维度顺序改变 || IFD数目改变)
	{
		if (文件名)
			Pixels.child("TiffData").child("UUID").attribute("FileName").set_value(文件名);
		//此函数需要旧SizeI，因此必须在更新SizeI之前调用
		重置TiffData(Pixels, iSizeI, iSizeC, iSizeZ, iSizeT, iDimensionOrder);
	}
	else if (文件名)
		for (xml_node TD : Pixels.children("TiffData"))
			TD.child("UUID").attribute("FileName").set_value(文件名);
	if (IFD数目改变)
		iSizeI = UINT32(iSizeC) * iSizeZ * iSizeT;
	if (文件名 || 维度顺序改变 || IFD尺寸改变 || ChannelColors)
	{
		i图像描述.clear();
		XML文本接收器 接收器(i图像描述);
		图像描述文档.save(接收器);
		//单帧尺寸改变会导致IFD中多个参数需要修改，因此也必须重新生成IFD
		更新图像描述并扩展文件(IFD尺寸改变, 图像描述文档, iSizeI, SizePXY, *文件, i图像描述.c_str(), i图像描述.size() + 1);
		const OmeBigTiff5D文件头* const 文件头 = (OmeBigTiff5D文件头*)文件->映射指针();
		const 文件偏移<大, IFD5D> FirstIFD偏移 = 文件头->首IFD偏移;
		if (单帧尺寸改变)
		{
			SizePXY = UINT32(iSizeP) * iSizeX * iSizeY;
			IFD5D* FirstIFD = FirstIFD偏移(文件头);
			FirstIFD->图像宽度.SHORT值 = iSizeX;
			FirstIFD->图像长度.SHORT值 = iSizeY;
			FirstIFD->每个样本的位数.SHORT值 = iSizeP * 8;
			FirstIFD->每条行数.SHORT值 = iSizeY;
			FirstIFD->像素字节数.LONG值 = SizePXY;
		}
		填充IFD(iSizeI, FirstIFD偏移, 文件头, IFD尺寸改变, SizePXY);
	}
	if (维度顺序改变 || IFD尺寸改变)
		预计算参数(iDimensionOrder, iSizeC, iSizeZ, iSizeT, SizePXY, 源Size10, 源Size210);
}
void OmeBigTiff5D::SizeX(UINT16 SizeX)
{
	return 修改参数(像素类型::缺省, SizeX, 0, 0, 0, 0, nullptr, 维度顺序::缺省, nullptr);
}
void OmeBigTiff5D::SizeY(UINT16 SizeY)
{
	return 修改参数(像素类型::缺省, 0, SizeY, 0, 0, 0, nullptr, 维度顺序::缺省, nullptr);
}
void OmeBigTiff5D::SizeC(UINT8 SizeC)
{
	return 修改参数(像素类型::缺省, 0, 0, SizeC, 0, 0, nullptr, 维度顺序::缺省, nullptr);
}
void OmeBigTiff5D::SizeZ(UINT8 SizeZ)
{
	return 修改参数(像素类型::缺省, 0, 0, 0, SizeZ, 0, nullptr, 维度顺序::缺省, nullptr);
}
void OmeBigTiff5D::SizeT(uint32_t SizeT)
{
	return 修改参数(像素类型::缺省, 0, 0, 0, 0, SizeT, nullptr, 维度顺序::缺省, nullptr);
}
void OmeBigTiff5D::DimensionOrder(维度顺序 DO)
{
	return 修改参数(像素类型::缺省, 0, 0, 0, 0, 0, nullptr, DO, nullptr);
}
void OmeBigTiff5D::PixelType(像素类型 PT)
{
	return 修改参数(PT, 0, 0, 0, 0, 0, nullptr, 维度顺序::缺省, nullptr);
}
void OmeBigTiff5D::通道颜色(const 颜色* Colors)
{
	return 修改参数(像素类型::缺省, 0, 0, 0, 0, 0, Colors, 维度顺序::缺省, nullptr);
}
void OmeBigTiff5D::文件名(const char* 新文件名)
{
	return 修改参数(像素类型::缺省, 0, 0, 0, 0, 0, nullptr, 维度顺序::缺省, 新文件名);
}
void OmeBigTiff5D::图像描述(std::string&& WriteIn)
{
	xml_document 新图像描述文档; const char* 新唯一标识符; xml_attribute 文件名属性; xml_node 新Pixels; UINT16 新iSizeX, 新iSizeY; UINT8 新iSizeC, 新iSizeZ; 维度顺序 新iDimensionOrder; 像素类型 新iPixelType; 颜色数组 新iChannelColors; uint32_t 新iSizeT;
	const xml_parse_status XML异常 = 新图像描述文档.load_string(WriteIn.c_str()).status;
	if (XML异常)
		throw Image5D异常(图像描述解析失败, XML异常);
	解析图像描述(新图像描述文档, 新唯一标识符, 文件名属性, 新Pixels, 新iSizeX, 新iSizeY, 新iSizeC, 新iSizeZ, 新iSizeT, 新iDimensionOrder, 新iPixelType, 新iChannelColors);
	图像描述文档 = std::move(新图像描述文档), Pixels = 新Pixels, iSizeX = 新iSizeX, iSizeY = 新iSizeY, iPixelType = 新iPixelType; i通道颜色.reset(新iChannelColors.release()); iSizeP = 像素类型尺寸[UINT8(iPixelType)];
	if (检修TiffData(Pixels, i文件名 = 文件名属性.as_string(), 唯一标识符 = 新唯一标识符, iSizeC = 新iSizeC, iSizeZ = 新iSizeZ, iSizeT = 新iSizeT, iDimensionOrder = 新iDimensionOrder))
	{
		i图像描述.clear();
		XML文本接收器 接收器(i图像描述);
		图像描述文档.save(接收器);
	}
	else
		i图像描述 = std::move(WriteIn);
	更新图像描述并扩展文件(true, 图像描述文档, iSizeI = UINT32(iSizeC) * iSizeZ * iSizeT, SizePXY = UINT32(iSizeP) * iSizeX * iSizeY, *文件, i图像描述.c_str(), i图像描述.size() + 1);
	const OmeBigTiff5D文件头* const 文件头 = (OmeBigTiff5D文件头*)文件->映射指针();
	const 文件偏移<大, IFD5D> FirstIFD偏移 = 文件头->首IFD偏移;
	IFD5D* FirstIFD = FirstIFD偏移(文件头);
	FirstIFD->图像宽度.SHORT值 = iSizeX;
	FirstIFD->图像长度.SHORT值 = iSizeY;
	FirstIFD->每个样本的位数.SHORT值 = iSizeP * 8;
	FirstIFD->每条行数.SHORT值 = iSizeY;
	FirstIFD->像素字节数.LONG值 = SizePXY;
	填充IFD(iSizeI, FirstIFD偏移, 文件头, true, SizePXY);
	预计算参数(iDimensionOrder, iSizeC, iSizeZ, iSizeT, SizePXY, 源Size10, 源Size210);
}