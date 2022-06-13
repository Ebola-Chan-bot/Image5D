#include "pch.h"
#include "Oir读入器.h"
#include <stdlib.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <algorithm>
#include <pugixml.hpp>
#include <numeric>
#pragma pack(push,4)
struct 像素块
{
	uint32_t 像素长度;
	int 保留;
};
struct UID块
{
	int CheckLength;
	int Check;
	int 保留[2];
	uint32_t UID长度;
};
#pragma pack(pop)

struct 通道设备
{
	const char* 通道;
	const char* 设备;
	uint8_t 顺序;
};
constexpr const char XML标头[] = "<?xml version=\"1.0\" encoding=\"ASCII\"?>\n";
constexpr uint8_t XML标头长度 = sizeof(XML标头) - 1;
constexpr const char 帧属性标头[] = "<lsmframe";
constexpr uint8_t 帧标头长度 = sizeof(帧属性标头) - 1;
void 块循环(const char* s1指针, LPVOID 尾指针, 透明向量<const uint16_t*>& 块指针)
{
	while (true)
	{
		while (!std::equal(帧属性标头, 帧属性标头 + 帧标头长度, s1指针 + XML标头长度))
			if ((s1指针 = std::search(s1指针 += *((uint32_t*)s1指针 - 1), (const char*)尾指针, XML标头, XML标头 + XML标头长度)) >= 尾指针) //必须带等号，否则可能无限循环
				return;
		const UID块* UID块指针 = (UID块*)(s1指针 + *((uint32_t*)s1指针 - 1));
		if (UID块指针 + 1 > 尾指针)
			return;
		while (UID块指针->Check == 3)
		{
			const 像素块* 像素块指针 = (像素块*)((char*)(UID块指针 + 1) + UID块指针->UID长度);
			s1指针 = (char*)(像素块指针 + 1);
			UID块指针 = (UID块*)(s1指针 + 像素块指针->像素长度);
			if (UID块指针 > 尾指针)
				return;
			块指针.加尾((uint16_t*)s1指针);
			if (UID块指针 + 1 > 尾指针)
				return;
		}
		if ((s1指针 = std::search((char*)UID块指针, (char*)尾指针, XML标头, XML标头 + XML标头长度)) >= 尾指针)
			return;
	}
}
using namespace pugi;
Image5D异常 读头文件(const char* 文件路径, HANDLE& 文件句柄, HANDLE& 映射句柄, LPVOID& 映射指针, char*& 图像属性, uint16_t& SizeX, uint16_t& SizeY, uint8_t& SizeC, uint8_t& SizeZ, char**& 设备名称, float& 系列间隔, uint8_t& 每帧分块数, uint32_t*& 每块像素数, float**& 通道颜色, 透明向量<const uint16_t*>& 块指针)
{
	constexpr const char 图像属性标头[] = "<lsmimage";
	constexpr uint8_t 图像标头长度 = sizeof(图像属性标头) - 1;
	constexpr const char 查找表标头[] = "<lut";
	constexpr uint8_t 查找表标头长度 = sizeof(查找表标头) - 1;
	constexpr uint8_t UUID长度 = 36;
	LPVOID 尾指针;
	Image5D异常 异常 = 打开文件(文件路径, 文件句柄, 映射句柄, 映射指针, 尾指针);
	if (异常.类型 != 操作成功)
		return 异常;
	const UID块* UID块指针 = (UID块*)((char*)映射指针 + 96);
	if (UID块指针 + 1 > 尾指针)
		return Image5D异常{ .类型 = 文件不包含块 };
	const 像素块* 像素块指针;
	while (UID块指针->Check == 3)
	{
		//说明有REF图块，需要跳过
		像素块指针 = (像素块*)((char*)(UID块指针 + 1) + UID块指针->UID长度);
		if (像素块指针 + 1 > 尾指针)
			return Image5D异常{ .类型 = REF块不完整 };
		UID块指针 = (UID块*)((char*)(像素块指针 + 1) + 像素块指针->像素长度);
		if (UID块指针 + 1 > 尾指针)
			return Image5D异常{ .类型 = REF块不完整 };
	}
	const char* s1指针;
	if ((s1指针 = std::search((char*)UID块指针, (char*)尾指针, XML标头, XML标头 + XML标头长度)) >= 尾指针)
		return Image5D异常{ .类型 = 找不到帧标头 };
	while (!std::equal(帧属性标头, 帧属性标头 + 帧标头长度, s1指针 + XML标头长度))
		if ((s1指针 = std::search(s1指针 += *((uint32_t*)s1指针 - 1), (const char*)尾指针, XML标头, XML标头 + XML标头长度)) >= 尾指针) //必须带等号，否则可能无限循环
			return Image5D异常{ .类型 = 找不到帧标头 };
	UID块指针 = (UID块*)(s1指针 + *((uint32_t*)s1指针 - 1));
	if (UID块指针 + 1 > 尾指针)
		return Image5D异常{ .类型 = UID块不完整 };
	if (UID块指针->Check != 3)
		return Image5D异常{ .类型 = 空的像素块 };
	透明向量<uint32_t> 每块像素数向量;
	while (UID块指针->Check == 3)
	{
		像素块指针 = (像素块*)((char*)(UID块指针 + 1) + UID块指针->UID长度);
		s1指针 = (char*)(像素块指针 + 1);
		if (s1指针 > 尾指针)
			return Image5D异常{ .类型 = 像素块不完整 };
		每块像素数向量.加尾(像素块指针->像素长度 / 2);
		块指针.加尾((uint16_t*)s1指针);
		UID块指针 = (UID块*)(s1指针 + 像素块指针->像素长度);
		if (UID块指针 + 1 > 尾指针)
			return Image5D异常{ .类型 = UID块不完整 };
	}
	if ((s1指针 = std::search((char*)UID块指针, (char*)尾指针, XML标头, XML标头 + XML标头长度)) >= 尾指针)
		return Image5D异常{ .类型 = 找不到图像标头 };
	while (!std::equal(图像属性标头, 图像属性标头 + 图像标头长度, s1指针 + XML标头长度))
		if ((s1指针 = std::search(s1指针 += *((uint32_t*)s1指针 - 1), (const char*)尾指针, XML标头, XML标头 + XML标头长度)) >= 尾指针) //必须带等号，否则可能无限循环
			return Image5D异常{ .类型 = 找不到帧标头 };
	uint32_t 长度;
	if (s1指针 + (长度 = *((uint32_t*)s1指针 - 1)) > 尾指针)
		return Image5D异常{ .类型 = 图像属性不完整 };
	memcpy((图像属性 = (char*)malloc(长度 + 1)), s1指针, 长度);
	图像属性[长度] = 0;
	xml_parse_status 解析结果;
	xml_document XML解析文档;
	if ((解析结果 = XML解析文档.load_string(图像属性).status) != xml_parse_status::status_ok)
		return Image5D异常{ .XML错误代码 = XML解析状态(解析结果) ,.类型 = 图像属性解析失败 };
	xml_node 普通图像采集;
	if (!(普通图像采集 = XML解析文档.child("commonimage:acquisition")))
		return Image5D异常{ .类型 = 图像采集未定义 };
	xml_node 节点;
	if (!(节点 = 普通图像采集.child("commonimage:imagingParam")))
		return Image5D异常{ .类型 = 成像参数未定义 };
	xml_object_range<xml_named_node_iterator>节点集合 = 节点.children("commonparam:axis");
	xml_named_node_iterator 头节点 = 节点集合.begin();
	xml_named_node_iterator 尾节点 = 节点集合.end();
	xml_attribute 节点属性;
	xml_text 节点文本;
	SizeZ = 1;
	while (头节点 != 尾节点)
	{
		if (!strcmp(头节点->name(), "commonparam:axis") && (节点属性 = 头节点->attribute("paramEnable")) && 节点属性.as_bool())
		{
			if (!((节点 = 头节点->child("commonparam:maxSize")) && (节点文本 = 节点.text())))
				return Image5D异常{ .类型 = Z层尺寸未定义 };
			SizeZ = 节点文本.as_uint();
			break;
		}
		else
			头节点++;
	}
	节点集合 = 普通图像采集.children("commonphase:channel");
	if ((头节点 = 节点集合.begin()) == (尾节点 = 节点集合.end()))
		return Image5D异常{ .类型 = 通道未定义 };
	透明向量<通道设备> 通道设备向量;
	透明向量<uint32_t> 长度向量;
	const char* 属性值;
	while (头节点 != 尾节点)
	{
		if (!(节点属性 = 头节点->attribute("enable")))
			return Image5D异常{ .类型 = 通道enable未定义 };
		if (!节点属性.as_bool())
			continue;
		通道设备向量.加尾(通道设备());
		通道设备& 通道设备对象 = 通道设备向量.指针[通道设备向量.长度 - 1];
		if (!(节点属性 = 头节点->attribute("id")))
			return Image5D异常{ .类型 = 通道id未定义 };
		通道设备对象.通道 = 节点属性.as_string();
		if (!(节点属性 = 头节点->attribute("order")))
			return Image5D异常{ .类型 = 通道order未定义 };
		通道设备对象.顺序 = 节点属性.as_uint();
		if (!(节点 = 头节点->child("commonphase:deviceName")) && (节点文本 = 节点.text()))
			return Image5D异常{ .类型 = 通道设备名未定义 };
		通道设备对象.设备 = 节点文本.as_string();
		长度向量.加尾(strlen(通道设备对象.设备));
		头节点++;
	}
	SizeC = 通道设备向量.长度;
	通道设备* 通道设备指针 = 通道设备向量.指针;
	std::sort(通道设备指针, 通道设备指针 + SizeC, [](const 通道设备 对象1, const 通道设备 对象2) {return 对象1.顺序 <= 对象2.顺序; });
	float* 颜色三元 = (通道颜色 = (float**)malloc((sizeof(float) * 3 + sizeof(float*)) * SizeC))[SizeC];
	uint32_t* 长度指针 = 长度向量.指针;
	char* 名称文本 = (设备名称 = (char**)malloc(std::accumulate(长度指针, 长度指针 + SizeC, 0) + SizeC + sizeof(char*) * SizeC))[SizeC];
	for (uint8_t C = 0; C < SizeC; ++C)
	{
		设备名称[C] = (char*)memcpy(名称文本, 通道设备指针[C].设备, 长度指针[C]);
		名称文本 += 长度指针[C];
		通道颜色[C] = 颜色三元;
		颜色三元 += 3;
	}
	if (!((节点 = 普通图像采集.child("lsmimage:scannerType")) && (节点文本 = 节点.text())))
		return Image5D异常{ .类型 = 扫描类型未定义 };
	属性值 = 节点文本.as_string();
	节点集合 = 普通图像采集.children("lsmimage:scannerSettings");
	if ((头节点 = 节点集合.begin()) == (尾节点 = 节点集合.end()))
		return Image5D异常{ .类型 = 扫描设置未定义 };
	while (头节点 != 尾节点)
	{
		if (!(节点属性 = 头节点->attribute("type")))
			return Image5D异常{ .类型 = 扫描类型未定义 };
		if (strcmp(节点属性.value(), 属性值))
			continue;
		if (!((节点 = 头节点->child("commonparam:width")) && (节点文本 = 节点.text())))
			return Image5D异常{ .类型 = 扫描宽度未定义 };
		SizeX = 节点文本.as_uint();
		if (!((节点 = 头节点->child("commonparam:height")) && (节点文本 = 节点.text())))
			return Image5D异常{ .类型 = 扫描高度未定义 };
		SizeY = 节点文本.as_uint();
		if (!((节点 = 头节点->child("commonparam:seriesInterval")) && (节点文本 = 节点.text())))
			return Image5D异常{ .类型 = 系列间隔未定义 };
		系列间隔 = 节点文本.as_float();
	}
	每帧分块数 = 每块像素数向量.长度 / SizeC;
	每块像素数 = 每块像素数向量.指针;
	const uint32_t* 块像素数 = 每块像素数;
	for (uint8_t 块 = 1; 块 < 每帧分块数; ++块)
		每块像素数[块] = *(块像素数 += SizeC);
	for (uint8_t C1 = 0; C1 < SizeC; ++C1)
	{
		if ((s1指针 = std::search(s1指针 + 长度, (const char*)尾指针, XML标头, XML标头 + XML标头长度)) >= 尾指针)
			return Image5D异常{ .类型 = 找不到查找表 };
		while (!std::equal(查找表标头, 查找表标头 + 查找表标头长度, s1指针 + XML标头长度))
			if ((s1指针 = std::search(s1指针 += *((uint32_t*)s1指针 - 1), (const char*)尾指针, XML标头, XML标头 + XML标头长度)) >= 尾指针) //必须带等号，否则可能无限循环
				return Image5D异常{ .类型 = 找不到查找表 };
		长度 = *((uint32_t*)s1指针 - 1);
		if ((解析结果 = XML解析文档.load_buffer(s1指针, 长度).status) != xml_parse_status::status_ok)
			return Image5D异常{ .XML错误代码 = XML解析状态(解析结果),.类型 = 找不到查找表 };
		属性值 = (char*)((uint32_t*)s1指针 - 1) - UUID长度;
		for (uint8_t C2 = 0; C2 < SizeC; ++C2)
			if (std::equal(属性值, 属性值 + UUID长度, 通道设备指针[C2].通道))
			{
				颜色三元 = 通道颜色[C2];
				break;
			}
		if (!((节点 = XML解析文档.child("lut:red")) && (节点 = 节点.child("lut:contrast")) && (节点文本 = 节点.text())))
			return Image5D异常{ .类型 = 红色分量未定义 };
		长度 = *((uint32_t*)s1指针 - 1);
		颜色三元[0] = 节点文本.as_float();
		if (!((节点 = XML解析文档.child("lut:green")) && (节点 = 节点.child("lut:contrast")) && (节点文本 = 节点.text())))
			return Image5D异常{ .类型 = 绿色分量未定义 };
		颜色三元[1] = 节点文本.as_float();
		if (!((节点 = XML解析文档.child("lut:blue")) && (节点 = 节点.child("lut:contrast")) && (节点文本 = 节点.text())))
			return Image5D异常{ .类型 = 蓝色分量未定义 };
		颜色三元[2] = 节点文本.as_float();
	}
	if ((s1指针 = std::search(s1指针 + 长度, (const char*)尾指针, XML标头, XML标头 + XML标头长度)) >= 尾指针)
		return Image5D异常{ .类型 = 找不到帧标头 };
	块循环(s1指针, 尾指针, 块指针);
	return 异常;
}
Image5D异常 读单个文件(const char* 文件路径, HANDLE& 文件句柄, HANDLE& 映射句柄, LPVOID& 映射指针, 透明向量<const uint16_t*>& 块指针)
{
	LPVOID 尾指针;
	Image5D异常 异常 = 打开文件(文件路径, 文件句柄, 映射句柄, 映射指针, 尾指针);
	if (异常.类型 != 操作成功)
		return 异常;
	const char* s1指针;
	if ((s1指针 = std::search((char*)映射指针 + 96, (char*)尾指针, XML标头, XML标头 + XML标头长度)) >= 尾指针)
		return Image5D异常{ .类型 = 文件不包含块 };
	块循环(s1指针, 尾指针, 块指针);
	return 异常;
}
Oir读入器* Oir读入器::创建(LPCSTR 头文件路径)
{
	size_t 长度 = strlen(头文件路径) + 1;
	char* const 驱动器号 = (char*)malloc(长度 * 5 + 6);
	char* const 目录路径 = 驱动器号 + 长度;
	char* const 基文件名 = 目录路径 + 长度;
	char* const 文件扩展名 = 基文件名 + 长度;
	char* const 当前路径 = 文件扩展名 + 长度;
	_splitpath(头文件路径, 驱动器号, 目录路径, 基文件名, 文件扩展名);
	_makepath(当前路径, 驱动器号, 目录路径, 基文件名, nullptr);
	长度 = strlen(当前路径);
	当前路径[长度] = '_';
	char* const 编号位置 = 当前路径 + 长度 + 1;
	UINT8 文件数目 = 1;
	sprintf(编号位置, "%05u", 文件数目);
	while (PathFileExistsA(当前路径))
		sprintf(编号位置, "%05u", ++文件数目);
	strcpy(编号位置, ".oir");
	HANDLE* const 文件句柄s = (HANDLE*)malloc((sizeof(HANDLE) * 2 + sizeof(LPVOID)) * 文件数目);
	HANDLE* const 映射句柄s = 文件句柄s + 文件数目;
	std::fill_n(文件句柄s, 文件数目 * 2, INVALID_HANDLE_VALUE);
	LPVOID* const 映射指针s = 映射句柄s + 文件数目;
	std::fill_n(映射指针s, 文件数目, nullptr);
	UINT16 SizeX;
	UINT16 SizeY;
	UINT8 SizeC;
	UINT8 SizeZ;
	float 系列间隔;
	UINT8 每帧分块数;
	透明向量<const UINT16*> 块指针向量;
	char* 图像属性 = nullptr;
	float** 通道颜色 = nullptr;
	char** 设备名称 = nullptr;
	uint32_t* 每块像素数 = nullptr;
	Image5D异常 异常 = 读头文件(当前路径, 文件句柄s[0], 映射句柄s[0], 映射指针s[0], 图像属性, SizeX, SizeY, SizeC, SizeZ, 设备名称, 系列间隔, 每帧分块数, 每块像素数, 通道颜色, 块指针向量);
	if (异常.类型 != 操作成功)
	{
		UnmapViewOfFile(映射指针s[0]);
		CloseHandle(映射句柄s[0]);
		CloseHandle(文件句柄s[0]);
		free(图像属性);
		free(通道颜色);
		free(设备名称);
		free(每块像素数);
		free(驱动器号);
		free(文件句柄s);
		throw 异常;
	}
	UINT8 文件序号;
	for (文件序号 = 1; 文件序号 < 文件数目; ++文件序号)
	{
		sprintf(编号位置, "%05u", 文件序号);
		if ((异常 = 读单个文件(当前路径, 文件句柄s[文件序号], 映射句柄s[文件序号], 映射指针s[文件序号], 块指针向量)).类型 != 操作成功)
		{
			UnmapViewOfFile(映射指针s[文件序号]);
			CloseHandle(映射句柄s[文件序号]);
			CloseHandle(文件句柄s[文件序号]);
			break;
		}
	}
	free(驱动器号);
	return new Oir读入器(文件序号, 文件句柄s, 映射句柄s, 映射指针s, 每帧分块数, 每块像素数, SizeX, SizeY, SizeC, SizeZ, 块指针向量.长度 / (每帧分块数 * SizeC * SizeZ), 系列间隔, 通道颜色, 设备名称, 图像属性, 块指针向量.释放所有权());
};
bool Oir读入器::读入像素(UINT16* 写出头TZ, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const
{
	if (TStart + TSize > SizeT || ZStart + ZSize > SizeZ || CStart + CSize > SizeC)
		return false;
	const UINT16* const* 读入头T = 块指针 + (UINT32(TStart) * SizeZ + ZStart) * 每帧分块数 * SizeC + CStart;
	const UINT16* const* const 读入尾T = 读入头T + UINT32(TSize) * SizeZBC;
	const UINT8 读入ZBC = ZSize * SizeBC;
	const UINT32 写出CYX = UINT32(CSize) * SizeYX;
	while (读入头T < 读入尾T)
	{
		const UINT16* const* 读入头ZB = 读入头T;
		const UINT16* const* 读入尾Z = 读入头ZB + 读入ZBC;
		while (读入头ZB < 读入尾Z)
		{
			const UINT32* 块像素头 = 每块像素数;
			const UINT16* const* 读入尾B = 读入头ZB + SizeBC;
			UINT16* 写出头B = 写出头TZ;
			while (读入头ZB < 读入尾B)
			{
				const UINT16* const* 读入头C = 读入头ZB;
				const UINT16* const* 读入尾C = 读入头C + CSize;
				UINT16* 写出头C = 写出头B;
				UINT32 块像素数 = *块像素头;
				while (读入头C < 读入尾C)
				{
					std::copy_n(*读入头C, 块像素数, 写出头C);
					读入头C++;
					写出头C += SizeYX;
				}
				读入头ZB += SizeC;
				写出头B += 块像素数;
				块像素头++;
			}
			写出头TZ += 写出CYX;
		}
		读入头T += SizeZBC;
	}
	return true;
}
bool Oir读入器::读入像素(UINT16* 写出头, UINT16 TStart, UINT16 TSize, UINT8 C)const
{
	if (TStart + TSize > SizeT || C >= SizeC)
		return false;
	const UINT16* const* 读入头 = 块指针 + UINT32(TStart) * SizeZBC + C;
	const UINT16* const* const 读入尾T = 读入头 + UINT32(TSize) * SizeZBC;
	while (读入头 < 读入尾T)
	{
		const UINT16* const* 读入尾B = 读入头 + SizeBC;
		const UINT32* 块像素头 = 每块像素数;
		while (读入头 < 读入尾B)
		{
			std::copy_n(*读入头, *块像素头, 写出头);
			读入头 += SizeC;
			写出头 += *块像素头;
			块像素头++;
		}
	}
	return true;
}
bool Oir读入器::读入像素(UINT16* 写出头TZ, UINT16 TStart, UINT16 TSize)const
{
	if (TStart + TSize > SizeT)
		return false;
	const UINT16* const* 读入头 = 块指针 + UINT32(TStart) * SizeZBC;
	const UINT16* const* const 读入尾T = 读入头 + UINT32(TSize) * SizeZBC;
	while (读入头 < 读入尾T)
	{
		const UINT16* const* 读入尾B = 读入头 + SizeBC;
		UINT16* 写出头B = 写出头TZ;
		const UINT32* 块像素头 = 每块像素数;
		while (读入头 < 读入尾B)
		{
			const UINT16* const* 读入尾C = 读入头 + SizeC;
			UINT32 块像素数 = *块像素头;
			UINT16* 写出头C = 写出头B;
			while (读入头 < 读入尾C)
			{
				std::copy_n(*读入头, 块像素数, 写出头C);
				读入头++;
				写出头C += SizeYX;
			}
			写出头B += 块像素数;
			块像素头++;
		}
		写出头TZ += SizeCYX;
	}
	return true;
}
Oir读入器::~Oir读入器()
{
	for (UINT8 a = 0; a < 文件数目; ++a)
	{
		UnmapViewOfFile(映射指针[a]);
		CloseHandle(映射句柄[a]);
		CloseHandle(文件句柄[a]);
	}
	free((void*)文件句柄);
	free((void*)每块像素数);
	free((void*)通道颜色);
	free((void*)设备名称);
	free((void*)图像属性);
	free((void*)块指针);
}