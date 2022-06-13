#include "pch.h"
#include "OmeBigTiff5D.h"
#include <algorithm>
#include <rpc.h>
#include "resource.h"
#include "模块句柄.h"
#include "Tiff文件头.h"
#include "IFD5D.h"
#include "字符串常量.h"
#include "像素类型尺寸.h"
constexpr char 通道ID模板[] = "Channel:0:%u";
#pragma pack(push,1)
struct OmeBigTiff5D文件头 :public Tiff文件头<基本>
{
	UINT16 偏移尺寸 = 8;
	UINT16 留空 = 0;
	文件偏移<大,IFD5D> 首IFD偏移;
	UINT8 规范5D = 79;
};
#pragma pack(pop)
void 预计算参数(维度顺序 iDimensionOrder, UINT8 iSizeC, UINT8 iSizeZ, UINT16 iSizeT, UINT64 SizePXY, UINT64& 源Size10, UINT64& 源Size210)noexcept
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
OmeBigTiff5D::OmeBigTiff5D(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, char* 图像描述, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 颜色* 通道颜色, const char* 文件名, xml_node Pixels, xml_document&& 图像描述文档, const char* 唯一标识符, char* 像素头)
	:Tiff属性读入器(文件句柄, 映射句柄, 映射指针, 尾指针, iPixelType, iSizeX, iSizeY, iSizeI, 图像描述),
	Ome属性读入器(SizeC, SizeZ, SizeT, DimensionOrder, 通道颜色),
	图像描述文档(std::move(图像描述文档)), 唯一标识符(唯一标识符), i文件名(文件名), Pixels(Pixels), 像素头(像素头)
{
	预计算参数(iDimensionOrder, iSizeC, iSizeZ, iSizeT, SizePXY, 源Size10, 源Size210);
}
inline void 建立映射(HANDLE 文件句柄, HANDLE& 映射句柄, LPVOID& 映射指针,UINT64 文件尺寸,LPVOID& 尾指针)noexcept
{
	映射句柄 = CreateFileMapping(文件句柄, NULL, PAGE_READWRITE, 0, 0, NULL);
	映射指针 = MapViewOfFile(映射句柄, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	尾指针 = (char*)映射指针 + 文件尺寸;
}
inline void 更改文件尺寸(HANDLE 文件句柄, HANDLE& 映射句柄, LPVOID& 映射指针, LPVOID& 尾指针, UINT64 新尺寸)noexcept
{
	UnmapViewOfFile(映射指针);
	CloseHandle(映射句柄);
	SetFilePointerEx(文件句柄, *(LARGE_INTEGER*)&新尺寸, NULL, FILE_BEGIN);
	SetEndOfFile(文件句柄);
	建立映射(文件句柄, 映射句柄, 映射指针, 新尺寸, 尾指针);
}
//调用方负责释放iChannelColors；如果异常则无需释放
Image5D异常 解析图像描述(const xml_document& 图像描述文档, const char*& 唯一标识符, xml_attribute& 文件名属性, xml_node& Pixels, UINT16& iSizeX, UINT16& iSizeY, UINT8& iSizeC, UINT8& iSizeZ, UINT16& iSizeT, 维度顺序& iDimensionOrder, 像素类型& iPixelType, 颜色*& iChannelColors)noexcept
{
	xml_node 节点 = 图像描述文档.child("OME");
	if (!(节点))
		return Image5D异常{ .类型 = 缺少OME节点 };
	xml_attribute 属性;
	if (!(属性 = 节点.attribute("UUID")))
		return Image5D异常{ .类型 = 缺少UUID属性 };
	唯一标识符 = 属性.as_string();
	if (!(节点 = 节点.child("Image")))
		return Image5D异常{ .类型 = 缺少Image节点 };
	if (!(文件名属性 = 节点.attribute("Name")))
		return Image5D异常{ .类型 = 缺少Name属性 };
	Pixels = 节点.child("Pixels");
	if (!Pixels)
		return Image5D异常{ .类型 = 缺少Pixels节点 };
	if (!(属性 = Pixels.attribute("SizeX")))
		return Image5D异常{ .类型 = 缺少SizeX属性 };
	if (!(iSizeX = 属性.as_uint()))
		return Image5D异常{ .类型 = SizeX为0 };
	if (!(属性 = Pixels.attribute("SizeY")))
		return Image5D异常{ .类型 = 缺少SizeY属性 };
	if (!(iSizeY = 属性.as_uint()))
		return Image5D异常{ .类型 = SizeY为0 };
	if (!(属性 = Pixels.attribute("SizeC")))
		return Image5D异常{ .类型 = 缺少SizeC属性 };
	if (!(iSizeC = 属性.as_uint()))
		return Image5D异常{ .类型 = SizeC为0 };
	if (!(属性 = Pixels.attribute("SizeZ")))
		return Image5D异常{ .类型 = 缺少SizeZ属性 };
	if (!(iSizeZ = 属性.as_uint()))
		return Image5D异常{ .类型 = SizeZ为0 };
	if (!(属性 = Pixels.attribute("SizeT")))
		return Image5D异常{ .类型 = 缺少SizeT属性 };
	if (!(iSizeT = 属性.as_uint()))
		return Image5D异常{ .类型 = SizeT为0 };
	if (!(属性 = Pixels.attribute("DimensionOrder")))
		return Image5D异常{ .类型 = 缺少DimensionOrder属性 };
	INT8 序号 = 寻找字符串(属性.as_string(), 维度顺序字符串);
	if (序号 < 0)
		return Image5D异常{ .类型 = 维度顺序无效 };
	iDimensionOrder = (维度顺序)序号;
	if (!(属性 = Pixels.attribute("Type")))
		return Image5D异常{ .类型 = 缺少Type属性 };
	序号 = 寻找字符串(属性.as_string(), 像素类型字符串);
	if (序号 < 0)
		return Image5D异常{ .类型 = 像素类型无效 };
	iPixelType = (像素类型)序号;
	透明向量<xml_node>ChannelBuffer;
	for (xml_node 节点 : Pixels.children("Channel"))
		ChannelBuffer.加尾(节点);
	if (ChannelBuffer.长度 != iSizeC)
		return Image5D异常{ .类型 = SizeC与颜色数不匹配 };
	const xml_node* const 通道节点 = ChannelBuffer.指针;
	iChannelColors = (颜色*)malloc(sizeof(颜色) * iSizeC);
	for (UINT8 C = 0; C < iSizeC; ++C)
		iChannelColors[C].整数值 = (属性 = 通道节点[C].attribute("Color")) ? 属性.as_int() : -1;
	return 成功;
}
inline void 设置TiffData(xml_node& TiffData, xml_node Pixels, xml_node 模板, const char* First, UINT16 I, UINT32& Index)noexcept
{
	TiffData = Pixels.append_copy(模板);
	TiffData.attribute(First).set_value(I);
	TiffData.attribute("IFD").set_value(++Index);
}
//输入的Pixels节点应当只有首个TiffData子节点并包含无需修改的必要信息
void 填充TiffData(xml_node Pixels, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 iDimensionOrder)noexcept
{
	xml_node 模板 = Pixels.child("TiffData");
	UINT16 Size0, Size1, Size2; const char* First0, * First1, * First2;
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
	for (UINT16 I0 = 1; I0 < Size0; ++I0)
		设置TiffData(TiffData, Pixels, 模板, First0, I0, Index);
	for (UINT16 I1 = 1; I1 < Size1; ++I1)
	{
		设置TiffData(模板1, Pixels, 模板, First1, I1, Index);
		for (UINT16 I0 = 1; I0 < Size0; ++I0)
			设置TiffData(TiffData, Pixels, 模板1, First0, I0, Index);
	}
	for (UINT16 I2 = 1; I2 < Size2; ++I2)
	{
		设置TiffData(模板1, Pixels, 模板, First2, I2, Index);
		for (UINT16 I0 = 1; I0 < Size0; ++I0)
			设置TiffData(TiffData, Pixels, 模板1, First0, I0, Index);
		for (UINT16 I1 = 1; I1 < Size1; ++I1)
		{
			设置TiffData(模板0, Pixels, 模板1, First1, I1, Index);
			for (UINT16 I0 = 1; I0 < Size0; ++I0)
				设置TiffData(TiffData, Pixels, 模板0, First0, I0, Index);
		}
	}
}
//返回TiffData是否被修复
bool 检修TiffData(xml_node Pixels, const char* 文件名, const char* 唯一标识符, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder)
{
	xml_node 节点;
	const bool 需要修复 = !((节点 = Pixels.child("TiffData")) && (节点 = 节点.child("UUID")) && 节点.attribute("FileName"));
	if (需要修复)
	{
		透明向量<xml_node> 待删除;
		for (xml_node 节点 : Pixels.children("TiffData"))
			待删除.加尾(节点);
		xml_node* 头指针 = 待删除.指针;
		xml_node* const 尾指针 = 头指针 + 待删除.长度;
		for (; 头指针 < 尾指针; ++头指针)
			Pixels.remove_child(*头指针);
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
//此接收器输出的字符串不含尾随0，但会预留空间
class XML文本接收器 :public xml_writer
{
	UINT32 容量; //缓存容量
public:
	UINT32 尺寸 = 0; //有效数据尺寸
	char* 缓存;
	void write(const void* data, size_t size)override
	{
		const UINT32 旧尺寸 = 尺寸;
		if ((尺寸 += size) >= 容量)
			缓存 = (char*)realloc(缓存, (容量 = 尺寸 * 2));
		memcpy(缓存 + 旧尺寸, data, size);
	}
	XML文本接收器(char* 缓存, UINT32 容量) :缓存(缓存), 容量(容量) {}
	XML文本接收器() :缓存(nullptr), 容量(0) {}
};
//调用方负责释放iChannelColors和i图像描述；如果异常无需释放iChannelColors
inline Image5D异常 打开核心(LPVOID 映射指针, LPVOID 尾指针,UINT64& 最小文件尺寸,xml_document& 图像描述文档,const char*& 唯一标识符,const char*& i文件名,xml_node& Pixels,UINT16& iSizeX, UINT16& iSizeY, UINT8& iSizeC, UINT8& iSizeZ, UINT16& iSizeT,维度顺序& iDimensionOrder,像素类型& iPixelType,颜色*& iChannelColors,char*& i图像描述)noexcept
{
	const IFD5D* const FirstIFD = ((OmeBigTiff5D文件头*)映射指针)->首IFD偏移(映射指针);
	if (尾指针 < FirstIFD + 1)
		return Image5D异常{ .类型 = 头IFD不完整 };
	const UINT64 NoValues = FirstIFD->图像描述.NoValues;
	const char* const iImageDescription = FirstIFD->图像描述.ASCII偏移(映射指针);
	if (尾指针 < iImageDescription + NoValues)
		return Image5D异常{ .类型 = 图像描述不完整 };
	xml_attribute 文件名属性;
	const xml_parse_status XML异常 = 图像描述文档.load_buffer(iImageDescription, NoValues).status;
	if (XML异常)
		return Image5D异常{ .XML错误代码 = (XML解析状态)XML异常,.类型 = 图像描述解析失败 };
	Image5D异常 异常 = 解析图像描述(图像描述文档, 唯一标识符, 文件名属性, Pixels, iSizeX, iSizeY, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iPixelType, iChannelColors);
	if (异常.类型 != 操作成功)
		return 异常;
	i文件名 = 文件名属性.as_string();
	if (检修TiffData(Pixels, i文件名, 唯一标识符, iSizeC, iSizeZ, iSizeT, iDimensionOrder))
	{
		XML文本接收器 接收器;
		图像描述文档.save(接收器);
		i图像描述 = 接收器.缓存;
		i图像描述[接收器.尺寸] = 0;
	}
	else
	{
		i图像描述 = (char*)malloc(NoValues + 1);
		memcpy(i图像描述, iImageDescription, NoValues);
		i图像描述[NoValues] = 0;
	}
	最小文件尺寸 = FirstIFD->像素偏移.LONG8值 + UINT64(iSizeX) * iSizeY * iSizeC * iSizeZ * iSizeT * 像素类型尺寸[(UINT8)iPixelType];
	return 成功;
}
const OmeBigTiff5D* OmeBigTiff5D::只读打开(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针)
{
	char* i图像描述 = nullptr;
	UINT64 最小文件尺寸; xml_document 图像描述文档; const char* 唯一标识符; const char* i文件名; xml_node Pixels; UINT16 iSizeX, iSizeY, iSizeT; UINT8 iSizeC, iSizeZ; 维度顺序 iDimensionOrder; 像素类型 iPixelType; 颜色* iChannelColors;
	Image5D异常 异常 = 打开核心(映射指针, 尾指针, 最小文件尺寸, 图像描述文档, 唯一标识符, i文件名, Pixels, iSizeX, iSizeY, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iPixelType, iChannelColors, i图像描述);
	if (异常.类型 != 操作成功)
	{
		free(i图像描述);
		throw 异常;
	}
	if ((char*)映射指针 + 最小文件尺寸 > 尾指针)
	{
		free(i图像描述);
		throw Image5D异常{ .类型 = 像素块不完整 };
	}
	return new OmeBigTiff5D(文件句柄, 映射句柄, 映射指针, 尾指针, iPixelType, iSizeX, iSizeY, UINT32(iSizeC) * iSizeZ * iSizeT, i图像描述, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iChannelColors, i文件名, Pixels, std::move(图像描述文档), 唯一标识符, ((OmeBigTiff5D文件头*)映射指针)->首IFD偏移(映射指针)->像素偏移.ASCII偏移(映射指针));
}
OmeBigTiff5D* OmeBigTiff5D::读写打开(HANDLE 文件句柄)
{
	if (文件句柄 == INVALID_HANDLE_VALUE)
		throw Image5D异常{ .Win32错误代码 = GetLastError(),.类型 = 文件打开失败 };
	LARGE_INTEGER 文件尺寸{ .QuadPart = 0 };
	SetFilePointerEx(文件句柄, 文件尺寸, &文件尺寸, FILE_END);
	HANDLE 映射句柄 = CreateFileMapping(文件句柄, NULL, PAGE_READWRITE, 0, 0, NULL);
	LPVOID 映射指针 = MapViewOfFile(映射句柄, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	LPVOID 尾指针 = (char*)映射指针 + 文件尺寸.QuadPart;
	char* i图像描述 = nullptr;
	UINT64 最小文件尺寸; xml_document 图像描述文档; const char* 唯一标识符; const char* i文件名; xml_node Pixels; UINT16 iSizeX, iSizeY, iSizeT; UINT8 iSizeC, iSizeZ; 维度顺序 iDimensionOrder; 像素类型 iPixelType; 颜色* iChannelColors;
	Image5D异常 异常 = 打开核心(映射指针, 尾指针, 最小文件尺寸, 图像描述文档, 唯一标识符, i文件名, Pixels, iSizeX, iSizeY, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iPixelType, iChannelColors, i图像描述);
	if (异常.类型 != 操作成功)
	{
		free(i图像描述);
		UnmapViewOfFile(映射指针);
		CloseHandle(映射句柄);
		CloseHandle(文件句柄);
		throw 异常;
	}
	if ((char*)映射指针 + 最小文件尺寸 > 尾指针)
		更改文件尺寸(文件句柄, 映射句柄, 映射指针, 尾指针, 最小文件尺寸);
	return new OmeBigTiff5D(文件句柄, 映射句柄, 映射指针, 尾指针, iPixelType, iSizeX, iSizeY, UINT32(iSizeC) * iSizeZ * iSizeT, i图像描述, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iChannelColors, i文件名, Pixels, std::move(图像描述文档), 唯一标识符, ((OmeBigTiff5D文件头*)映射指针)->首IFD偏移(映射指针)->像素偏移.ASCII偏移(映射指针));
}
void 填充IFD(UINT32 SizeI, 文件偏移<大,IFD5D> 当前IFD偏移, LPVOID 映射指针, bool 必须重新生成, UINT32 SizePXY)noexcept
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
	IFD5D* const 最后IFD指针 = 当前IFD指针 + SizeI - 2;
	while (当前IFD指针<最后IFD指针)
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
	当前IFD指针->NextIFD = 文件偏移<大,IFD5D>{ .偏移 = 0 };
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
//调用方负责释放文件句柄和文件名，如果异常则无需释放
Image5D异常 打开并取文件名(const char* 文件路径, HANDLE& 文件句柄, char*& 文件名)noexcept
{
	if ((文件句柄 = CreateFileA(文件路径, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		return Image5D异常{ .Win32错误代码 = GetLastError(),.类型 = 文件打开失败 };
	const size_t 长度 = strlen(文件路径) + 1;
	文件名 = (char*)malloc(长度 * 2);
	char* 扩展名 = 文件名 + 长度;
	_splitpath(文件路径, nullptr, nullptr, 文件名, 扩展名);
	strcat(文件名, 扩展名);
	return 成功;
}
void 构造文件(const char* 图像描述, UINT32 图像描述字节数,UINT32 SizeI, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, HANDLE 文件句柄, HANDLE& 映射句柄, LPVOID& 映射指针, LPVOID& 尾指针, 文件偏移<大, char>& 像素头偏移)noexcept
{
	文件偏移<大, IFD5D> IFD偏移对象(图像描述字节数 * 2 + sizeof(OmeBigTiff5D文件头));
	const UINT64 像素偏移 = IFD偏移对象 + SizeI * sizeof(IFD5D) * 2;
	const UINT32 SizePXY = UINT32(SizeX) * SizeY * 像素类型尺寸[(UINT8)PixelType];
	const LARGE_INTEGER 文件尺寸{ .QuadPart = INT64(像素偏移 + UINT64(SizePXY) * SizeI) };
	SetFilePointerEx(文件句柄, 文件尺寸, NULL, FILE_BEGIN);
	SetEndOfFile(文件句柄);
	建立映射(文件句柄, 映射句柄, 映射指针, 文件尺寸.QuadPart, 尾指针);
	((OmeBigTiff5D文件头*)映射指针)->首IFD偏移 = IFD偏移对象;
	文件偏移<大, char> 图像描述偏移{ .偏移 = sizeof(OmeBigTiff5D文件头) };
	strcpy(图像描述偏移(映射指针), 图像描述);
	像素头偏移.偏移 = 像素偏移;
	*IFD偏移对象(映射指针) = IFD5D::创建(图像描述字节数, 文件偏移<大, char>{.偏移 = sizeof(OmeBigTiff5D文件头)}, 像素头偏移, 像素类型尺寸[(UINT8)PixelType], SizeX, SizeY, 像素类型采样格式[(UINT8)PixelType]);
	填充IFD(SizeI, IFD偏移对象, 映射指针, true, SizePXY);
}
OmeBigTiff5D* OmeBigTiff5D::覆盖创建(const char* 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 颜色* iChannelColors, 维度顺序 DimensionOrder)
{
	if (!(PixelType < 像素类型::缺省 && SizeX && SizeY && SizeC && SizeZ && SizeT && DimensionOrder < 维度顺序::缺省))
		throw Image5D异常{ .类型 = 构造参数无效 };
	HANDLE 文件句柄;
	char* 待释放文件名;
	Image5D异常 异常 = 打开并取文件名(文件路径, 文件句柄, 待释放文件名);
	if (异常.类型 != 操作成功)
		throw 异常;
	const HRSRC 资源信息 = FindResourceW(模块句柄, MAKEINTRESOURCEW(IDR_XmlTemplate), L"XML");
	xml_document 图像描述文档;
	图像描述文档.load_buffer(LockResource(LoadResource(模块句柄, 资源信息)), SizeofResource(模块句柄, 资源信息), parse_declaration);
	char URN[46] = "urn:uuid:";
	xml_node 节点 = 图像描述文档.child("OME");
	const char* const 唯一标识符 = 添加UUID属性(节点, URN).as_string();
	xml_attribute 属性 = (节点 = 节点.child("Image")).append_attribute("Name");
	属性.set_value(待释放文件名);
	free(待释放文件名);
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
	节点.attribute("Color").set_value(iChannelColors->整数值);
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
	XML文本接收器 接收器;
	图像描述文档.save(接收器);
	char* const 图像描述 = 接收器.缓存;
	图像描述[接收器.尺寸] = 0;
	HANDLE 映射句柄; LPVOID 映射指针, 尾指针; 文件偏移<大, char> 像素头偏移;
	构造文件(图像描述, 接收器.尺寸 + 1, SizeI, PixelType, SizeX, SizeY, 文件句柄, 映射句柄, 映射指针, 尾指针, 像素头偏移);
	return new OmeBigTiff5D(文件句柄, 映射句柄, 映射指针, 尾指针, PixelType, SizeX, SizeY, SizeI, 图像描述, SizeC, SizeZ, SizeT, DimensionOrder, iChannelColors, i文件名, Pixels, std::move(图像描述文档), 唯一标识符, 像素头偏移(映射指针));
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
OmeBigTiff5D* OmeBigTiff5D::覆盖创建(const char* 文件路径, char* 图像描述)
{
	HANDLE 文件句柄;
	char* 待释放文件名;
	Image5D异常 异常 = 打开并取文件名(文件路径, 文件句柄, 待释放文件名);
	if (异常.类型 != 操作成功)
		throw 异常;
	xml_document 图像描述文档; const char* 唯一标识符; xml_attribute 文件名属性; xml_node Pixels; UINT16 iSizeX, iSizeY, iSizeT; UINT8 iSizeC, iSizeZ; 维度顺序 iDimensionOrder; 像素类型 iPixelType; 颜色* iChannelColors;
	const xml_parse_status XML异常 = 图像描述文档.load_string(图像描述).status;
	if (XML异常)
	{
		free(待释放文件名);
		CloseHandle(文件句柄);
		throw Image5D异常{ .XML错误代码 = (XML解析状态)XML异常,.类型 = 图像描述解析失败 };
	}
	异常 = 解析图像描述(图像描述文档, 唯一标识符, 文件名属性, Pixels, iSizeX, iSizeY, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iPixelType, iChannelColors);
	if (异常.类型 != 操作成功)
	{
		free(待释放文件名);
		CloseHandle(文件句柄);
		throw 异常;
	}
	文件名属性.set_value(待释放文件名);
	free(待释放文件名);
	const char* i文件名 = 文件名属性.as_string();
	UINT32 图像描述字节数 = strlen(图像描述) + 1;
	if (检修TiffData(Pixels, i文件名, 唯一标识符, iSizeC, iSizeZ, iSizeT, iDimensionOrder))
	{
		XML文本接收器 接收器(图像描述, 图像描述字节数);
		图像描述文档.save(接收器);
		图像描述 = 接收器.缓存;
		图像描述字节数 = 接收器.尺寸 + 1;
		图像描述[接收器.尺寸] = 0;
	}
	else
		更新文件名(Pixels, i文件名);
	HANDLE 映射句柄; LPVOID 映射指针, 尾指针; 文件偏移<大, char> 像素头偏移;
	const UINT32 iSizeI = UINT32(iSizeC) * iSizeZ * iSizeT;
	构造文件(图像描述, 图像描述字节数,iSizeI , iPixelType, iSizeX, iSizeY, 文件句柄, 映射句柄, 映射指针, 尾指针, 像素头偏移);
	return new OmeBigTiff5D(文件句柄, 映射句柄, 映射指针, 尾指针, iPixelType, iSizeX, iSizeY, iSizeI, 图像描述, iSizeC, iSizeZ, iSizeT, iDimensionOrder, iChannelColors, i文件名, Pixels, std::move(图像描述文档), 唯一标识符, 像素头偏移(映射指针));
}
constexpr Image5D异常 越界{ .类型 = 指定维度越界 };
Image5D异常 OmeBigTiff5D::读入像素(char* 缓冲区, UINT32 IStart, UINT32 ISize)const
{
	if (UINT64(IStart) + ISize > iSizeI)
		return 越界;
	memcpy(缓冲区, 像素头 + UINT64(IStart) * SizePXY, UINT64(ISize) * SizePXY);
	return 成功;
}
template <bool 读>
inline Image5D异常 读写3D(维度顺序 iDimensionOrder, UINT16 TStart, UINT16 TSize,UINT16 iSizeT, UINT8 ZStart, UINT8 ZSize, UINT8 iSizeZ,UINT8 CStart, UINT8 CSize,UINT8 iSizeC, UINT32 SizePXY, UINT64 源Size10, UINT64 源Size210, char* 头3, char* 缓冲区)
{
	if (UINT32(TStart) + TSize > iSizeT || UINT16(ZStart) + ZSize > iSizeZ || UINT8(CStart) + CSize > iSizeC)
		return 越界;
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
	while (缓冲区 < 尾3)
	{
		char* 头2 = 头3;
		char* const 尾2 = 缓冲区 + 库Size210;
		while (缓冲区 < 尾2)
		{
			if (读)
				memcpy(缓冲区, 头2, 库Size10);
			else
				memcpy(头2, 缓冲区, 库Size10);
			头2 += 源Size10;
			缓冲区 += 库Size10;
		}
		头3 += 源Size210;
	}
	return 成功;
}
Image5D异常 OmeBigTiff5D::读入像素(char* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const
{
	return 读写3D<true>(iDimensionOrder, TStart, TSize, iSizeT, ZStart, ZSize, iSizeZ, CStart, CSize, iSizeC, SizePXY, 源Size10, 源Size210, 像素头, 缓冲区);
}
Image5D异常 OmeBigTiff5D::写出像素(const char* 缓冲区, UINT32 IStart, UINT32 ISize)const
{
	if (UINT64(IStart) + ISize > iSizeI)
		return 越界;
	memcpy(像素头 + UINT64(IStart) * SizePXY, 缓冲区, UINT64(ISize) * SizePXY);
	return 成功;
}
Image5D异常 OmeBigTiff5D::写出像素(const char* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const
{
	return 读写3D<false>(iDimensionOrder, TStart, TSize, iSizeT, ZStart, ZSize, iSizeZ, CStart, CSize, iSizeC, SizePXY, 源Size10, 源Size210, 像素头, (char*)缓冲区);
}
char* OmeBigTiff5D::像素指针(UINT32 I)const
{
	if (I < iSizeI)
		return 像素头 + UINT64(I) * SizePXY;
	else
		throw 越界;
}
char* OmeBigTiff5D::像素指针(UINT16 T, UINT8 Z, UINT8 C)const
{
	if (T < iSizeT && Z < iSizeZ && C < iSizeC)
		switch (iDimensionOrder)
		{
		case XYCZT:
			return 像素头 + C * UINT64(SizePXY) + Z * 源Size10 + T * 源Size210;
		case XYCTZ:
			return 像素头 + C * UINT64(SizePXY) + T * 源Size10 + Z * 源Size210;
		case XYZCT:
			return 像素头 + Z * UINT64(SizePXY) + C * 源Size10 + T * 源Size210;
		case XYZTC:
			return 像素头 + Z * UINT64(SizePXY) + T * 源Size10 + C * 源Size210;
		case XYTCZ:
			return 像素头 + T * UINT64(SizePXY) + C * 源Size10 + Z * 源Size210;
		case XYTZC:
			return 像素头 + T * UINT64(SizePXY) + Z * 源Size10 + C * 源Size210;
		}
	else
		throw 越界;
}
void 更新图像描述并扩展文件(bool 必须重新生成, const xml_document& 图像描述文档, UINT32 SizeI, UINT32 SizePXY,HANDLE 文件句柄, HANDLE& 映射句柄, LPVOID& 映射指针,LPVOID& 尾指针,const char* 图像描述,UINT32 图像描述字节数)noexcept
{
	文件偏移<大, IFD5D> 当前IFD偏移 = ((OmeBigTiff5D文件头*)映射指针)->首IFD偏移;
	IFD5D* FirstIFD = 当前IFD偏移(映射指针);
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
			LARGE_INTEGER 文件大小;
			SetFilePointerEx(文件句柄, LARGE_INTEGER{ .QuadPart = 0 }, &文件大小, FILE_END);
			if (新文件大小 > 文件大小.QuadPart)
				更改文件尺寸(文件句柄, 映射句柄, 映射指针, 尾指针, 新文件大小);
			//此时的FirstTags是不对的
			像素偏移 = 新像素偏移;
		}
		//IFD偏移存储一个偏移量，不受重映射影响
		memmove(新FirstIFD偏移(映射指针), 当前IFD偏移(映射指针), 必须重新生成 ? sizeof(IFD5D) : 全IFD尺寸);
		((OmeBigTiff5D文件头*)映射指针)->首IFD偏移 = 新FirstIFD偏移;
		FirstIFD = 新FirstIFD偏移(映射指针);
		FirstIFD->像素偏移.BYTE偏移 = 文件偏移<大, BYTE>{ .偏移 = 像素偏移 };
	}
	strcpy(图像描述偏移(映射指针), 图像描述);
	//即使空间充足，图像描述字节数也可能发生缩小
	FirstIFD->图像描述.NoValues = 图像描述字节数;
}
//返回通道数是否更新，无论颜色是否改变
bool 更新通道(UINT8 SizeC, UINT8& iSizeC, 颜色* ChannelColors, 颜色*& iChannelColors, xml_node Pixels)
{
	if (!SizeC)
		SizeC = iSizeC;
	const bool 通道数更新 = SizeC != iSizeC;
	if (ChannelColors)
	{
		if (iChannelColors !=ChannelColors)
			free(iChannelColors);
		iChannelColors = ChannelColors;
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
				iChannelColors = (颜色*)realloc(iChannelColors, sizeof(颜色) * SizeC);
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
inline void 重置TiffData(xml_node Pixels, UINT32 SizeI, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder)noexcept
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
requires std::_Is_any_of_v<T,UINT8,UINT16>
inline bool 修改尺寸(T& 旧尺寸, T 新尺寸, xml_attribute 属性)
{
	const bool 修改 = 新尺寸 && 新尺寸 != 旧尺寸;
	if (修改)
	{
		旧尺寸 = 新尺寸;
		属性.set_value(新尺寸);
	}
	return 修改;
}
Image5D异常 OmeBigTiff5D::修改参数(像素类型 PT, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 颜色* ChannelColors, 维度顺序 DO, const char* 文件名)
{
	if ((PT > 像素类型::缺省 || DO > 维度顺序::缺省))
		return Image5D异常{ .类型 = 修改参数无效 };
	const bool 像素类型改变 = PT != 像素类型::缺省 && PT != iPixelType;
	const bool 单帧尺寸改变 = 修改尺寸(iSizeX, SizeX, Pixels.attribute("SizeX")) || 修改尺寸(iSizeY, SizeY, Pixels.attribute("SizeY")) || 像素类型改变;
	const bool IFD数目改变 = 更新通道(SizeC, iSizeC, ChannelColors, i通道颜色, Pixels) || 修改尺寸(iSizeZ, SizeZ, Pixels.attribute("SizeZ")) || 修改尺寸(iSizeT, SizeT, Pixels.attribute("SizeT"));
	const bool 维度顺序改变 = DO != 维度顺序::缺省 && DO != iDimensionOrder;
	const bool IFD尺寸改变 = 单帧尺寸改变 || IFD数目改变;
	if (文件名 || 维度顺序改变 || IFD尺寸改变)
	{
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
		//单帧尺寸改变会导致IFD中多个参数需要修改，因此也必须重新生成IFD
		XML文本接收器 接收器(i图像描述, strlen(i图像描述) + 1);
		图像描述文档.save(接收器);
		i图像描述 = 接收器.缓存;
		i图像描述[接收器.尺寸] = 0;
		更新图像描述并扩展文件(IFD尺寸改变, 图像描述文档, iSizeI, SizePXY, 文件句柄, 映射句柄, 映射指针, 尾指针, i图像描述, 接收器.尺寸);
		const 文件偏移<大, IFD5D> FirstIFD偏移 = ((OmeBigTiff5D文件头*)映射指针)->首IFD偏移;
		if (单帧尺寸改变)
		{
			SizePXY = UINT32(iSizeP) * iSizeX * iSizeY;
			IFD5D* FirstIFD = FirstIFD偏移(映射指针);
			FirstIFD->图像宽度.SHORT值 = iSizeX;
			FirstIFD->图像长度.SHORT值 = iSizeY;
			FirstIFD->每个样本的位数.SHORT值 = iSizeP * 8;
			FirstIFD->每条行数.SHORT值 = iSizeY;
			FirstIFD->像素字节数.LONG值 = SizePXY;
		}
		填充IFD(iSizeI, FirstIFD偏移, 映射指针, IFD尺寸改变, SizePXY);
		if (维度顺序改变 || IFD尺寸改变)
			预计算参数(iDimensionOrder, iSizeC, iSizeZ, iSizeT, SizePXY, 源Size10, 源Size210);
	}
	return 成功;
}
Image5D异常 OmeBigTiff5D::SizeX(UINT16 SizeX)
{
	return 修改参数(像素类型::缺省, SizeX, 0, 0, 0, 0, nullptr, 维度顺序::缺省, nullptr);
}
Image5D异常 OmeBigTiff5D::SizeY(UINT16 SizeY)
{
	return 修改参数(像素类型::缺省, 0, SizeY, 0, 0, 0, nullptr, 维度顺序::缺省, nullptr);
}
Image5D异常 OmeBigTiff5D::SizeC(UINT8 SizeC)
{
	return 修改参数(像素类型::缺省, 0, 0, SizeC, 0, 0, nullptr, 维度顺序::缺省, nullptr);
}
Image5D异常 OmeBigTiff5D::SizeZ(UINT8 SizeZ)
{
	return 修改参数(像素类型::缺省, 0, 0, 0, SizeZ, 0, nullptr, 维度顺序::缺省, nullptr);
}
Image5D异常 OmeBigTiff5D::SizeT(UINT16 SizeT)
{
	return 修改参数(像素类型::缺省, 0, 0, 0, 0, SizeT, nullptr, 维度顺序::缺省, nullptr);
}
Image5D异常 OmeBigTiff5D::DimensionOrder(维度顺序 DO)
{
	return 修改参数(像素类型::缺省, 0, 0, 0, 0, 0, nullptr, DO, nullptr);
}
Image5D异常 OmeBigTiff5D::PixelType(像素类型 PT)
{
	return 修改参数(PT, 0, 0, 0, 0, 0, nullptr, 维度顺序::缺省, nullptr);
}
Image5D异常 OmeBigTiff5D::通道颜色(颜色* Colors)
{
	return 修改参数(像素类型::缺省, 0, 0, 0, 0, 0, Colors, 维度顺序::缺省, nullptr);
}
Image5D异常 OmeBigTiff5D::文件名(const char* 新文件名)
{
	return 修改参数(像素类型::缺省, 0, 0, 0, 0, 0, nullptr, 维度顺序::缺省, 新文件名);
}
Image5D异常 OmeBigTiff5D::图像描述(char* WriteIn)
{
	if (WriteIn)
	{
		xml_document 新图像描述文档; const char* 新唯一标识符; xml_attribute 文件名属性; xml_node 新Pixels; UINT16 新iSizeX, 新iSizeY, 新iSizeT; UINT8 新iSizeC, 新iSizeZ; 维度顺序 新iDimensionOrder; 像素类型 新iPixelType; 颜色* 新iChannelColors;
		const xml_parse_status XML异常 = 新图像描述文档.load_string(WriteIn).status;
		if (XML异常)
			return Image5D异常{ .XML错误代码 = (XML解析状态)XML异常,.类型 = 图像描述解析失败 };
		Image5D异常 异常 = 解析图像描述(新图像描述文档, 新唯一标识符, 文件名属性, 新Pixels, 新iSizeX, 新iSizeY, 新iSizeC, 新iSizeZ, 新iSizeT, 新iDimensionOrder, 新iPixelType, 新iChannelColors);
		if (异常.类型 != 操作成功)
			return 异常;
		free(i通道颜色);
		图像描述文档 = std::move(新图像描述文档), Pixels = 新Pixels, iSizeX = 新iSizeX, iSizeY = 新iSizeY, iPixelType = 新iPixelType; i通道颜色 = 新iChannelColors; iSizeP = 像素类型尺寸[UINT8(iPixelType)];
		if (i图像描述 != WriteIn)
			free(i图像描述);
		UINT32 图像描述长度;
		if (检修TiffData(Pixels, i文件名 = 文件名属性.as_string(), 唯一标识符 = 新唯一标识符, iSizeC = 新iSizeC, iSizeZ = 新iSizeZ, iSizeT = 新iSizeT, iDimensionOrder = 新iDimensionOrder))
		{
			XML文本接收器 接收器(WriteIn, strlen(WriteIn) + 1);
			图像描述文档.save(接收器);
			i图像描述 = 接收器.缓存;
			图像描述长度 = 接收器.尺寸;
			i图像描述[图像描述长度] = 0;
		}
		else
		{
			i图像描述 = WriteIn;
			图像描述长度 = strlen(i图像描述);
		}
		更新图像描述并扩展文件(true, 图像描述文档, iSizeI = UINT32(iSizeC) * iSizeZ * iSizeT, SizePXY = UINT32(iSizeP) * iSizeX * iSizeY, 文件句柄, 映射句柄, 映射指针, 尾指针, i图像描述, 图像描述长度 + 1);
		const 文件偏移<大, IFD5D> FirstIFD偏移 = ((OmeBigTiff5D文件头*)映射指针)->首IFD偏移;
		IFD5D* FirstIFD = FirstIFD偏移(映射指针);
		FirstIFD->图像宽度.SHORT值 = iSizeX;
		FirstIFD->图像长度.SHORT值 = iSizeY;
		FirstIFD->每个样本的位数.SHORT值 = iSizeP * 8;
		FirstIFD->每条行数.SHORT值 = iSizeY;
		FirstIFD->像素字节数.LONG值 = SizePXY;
		填充IFD(iSizeI, FirstIFD偏移, 映射指针, true, SizePXY);
		预计算参数(iDimensionOrder, iSizeC, iSizeZ, iSizeT, SizePXY, 源Size10, 源Size210);
		return 成功;
	}
	else
		return Image5D异常{ .类型 = 图像描述为NULL };
}