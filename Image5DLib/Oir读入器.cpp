#include "Oir读入器.h"
#include <Shlwapi.h>
#include <stdio.h>
#include <numeric>
#include <algorithm>
using namespace Image5D;
enum class Oir基块类型 :uint32_t
{
	元数据,
	帧属性,
	未知,
	UID,
	像素,
	空,
};
enum class 元数据类型 :uint32_t
{
	fileinfo = 1,
	lsmimage,
	annotation,
	overlay,
	lut,
};
#pragma pack(push,4)
struct Oir文件头
{
	char OLYMPUSFORMAT[16] = { 'O','L','Y','M','P','U','S','R','A','W','F','O','R','M','A','T' };
	uint32_t 未知字段[4] = { 12,0,1,2 };
	uint64_t 文件大小;
	uint64_t 索引位置;
};
struct Oir基块
{
	uint32_t 长度;
	Oir基块类型 类型;
};
struct UID块 :Oir基块
{
	uint32_t 前像素块长度;
	uint32_t 后像素块长度;
	uint32_t UID长度;
};
struct 元数据块
{
	元数据类型 类型;
	uint32_t 未知字段[8];
	uint32_t 长度;
};
#pragma pack(pop)
struct 通道设备
{
	const char* 通道;
	const char* 设备;
	uint8_t 顺序;
};
using namespace pugi;
Oir读入器::Oir读入器(LPCWSTR 头文件路径)
{
	const uint16_t 长度 = wcslen(头文件路径) - 4;
	//make_unique会对内存初始化，不用于分配POD数组
	const std::unique_ptr<wchar_t[]> 字符缓冲 = std::make_unique_for_overwrite<wchar_t[]>(长度 + 7);
	std::copy_n(头文件路径, 长度, 字符缓冲.get());
	wchar_t* const 分隔符位置 = 字符缓冲.get() + 长度;
	*分隔符位置 = L'_';
	wchar_t* const 编号位置 = 分隔符位置 + 1;
	文件列表.push_back(std::make_unique<文件控制块>(头文件路径));
	uint64_t 总映射空间 = 文件列表.back()->粒度大小;
	while (true)
	{
		swprintf(编号位置, L"%05u", 文件列表.size());
		try
		{
			文件列表.push_back(std::make_unique<文件控制块>(字符缓冲.get()));
		}
		catch (Image5D异常)
		{
			break;
		}
		总映射空间 += 文件列表.back()->粒度大小;
	}
	void* 尾指针 = VirtualAlloc(nullptr, 总映射空间 + 分配粒度, MEM_RESERVE, PAGE_READONLY);
	VirtualFree(尾指针, 0, MEM_RELEASE);
	for (const std::unique_ptr<文件控制块>& 文件 : 文件列表)
	{
		try
		{
			文件->映射指针(尾指针);
		}
		catch (const Image5D异常& 异常)
		{
			throw Image5D异常(Oir连续映射失败, 异常.Win32错误代码);
		}
		尾指针 = (char*)尾指针 + 文件->粒度大小;
	}
	wcscpy(分隔符位置, L".Oir索引");
	try
	{
		//直接throw;无法捕获
		索引文件 = std::make_unique<文件映射>(字符缓冲.get(), true);
		索引文件->映射指针(nullptr);
		索引 = (Oir索引*)索引文件->映射指针();
		if (sizeof(索引) > 索引文件->文件大小() || !索引->验证(索引文件->文件大小()))
			throw 0;
		uint64_t* 块偏移;
		索引->Get变长成员(每块像素数, i通道颜色, 块偏移);
		const uint32_t 块总数 = uint32_t(索引->SizeZBC) * 索引->SizeT;
		if (!块总数)
			throw 0;
		if ((char*)(块偏移 + 块总数) > (char*)索引文件->映射指针() + 索引文件->文件大小())
			throw 0;
		const char* const 映射指针 = (char*)文件列表[0]->映射指针();
		if (映射指针 + 块偏移[块总数 - 1] > 尾指针)
			throw 0;
		for (const uint64_t* const 块偏移尾 = 块偏移 + 块总数; 块偏移 < 块偏移尾; ++块偏移)
		{
			const uint16_t* 指针 = (const uint16_t*)(映射指针 + *块偏移);
			if (指针 < 尾指针)
				块指针.push_back(指针);
			else
				throw 0;
		}
	}
	catch (...)
	{
		索引文件.reset();
		索引文件 = std::make_unique<文件映射>(字符缓冲.get(), 1ll); //这里必须1ll，否则重载不明确
		文件列表类::const_iterator 当前文件 = 文件列表.cbegin();
		const Oir文件头* 文件头 = (Oir文件头*)(*当前文件)->映射指针();
		const char* const 全局文件头 = (char*)文件头;
		const void* 尾指针 = (char*)文件头 + (*当前文件)->文件大小();
		if (文件头 + 1 > 尾指针)
			throw Image5D异常(Oir文件头不完整);
		const uint64_t* 基块索引 = (uint64_t*)((char*)文件头 + 文件头->索引位置 + 4);
		const Oir基块* 基块指针;
		do 
		{
			if (基块索引 + 1 > 尾指针)
				throw Image5D异常(Oir基块索引错误);
			if ((基块指针 = (Oir基块*)((char*)文件头 + *(基块索引++)))+1 > 尾指针)
				throw Image5D异常(Oir基块索引错误);
		} while (基块指针->类型 != Oir基块类型::帧属性);//此循环用于跳过REF块
		if (基块索引 + 1 > 尾指针)
			throw Image5D异常(Oir基块索引错误);
		const UID块* UID块指针 = (UID块*)((char*)文件头 + *(基块索引++));
		const char* 字符串 = (char*)(UID块指针 + 1);
		if (字符串 > 尾指针)
			throw Image5D异常(UID块不完整);
		std::vector<uint32_t> 每块像素数向量;
		const bool 有Z = *字符串 == 'z';
		const uint16_t* 像素指针;
		do
		{
			if (基块索引 + 1 > 尾指针)
				throw Image5D异常(Oir基块索引错误);
			if ((像素指针 = (uint16_t*)((基块指针 = (Oir基块*)((char*)文件头 + *(基块索引++))) + 1)) > 尾指针)
				throw Image5D异常(Oir基块索引错误);
			每块像素数向量.push_back(基块指针->长度 / 2);
			if ((char*)像素指针 + 基块指针->长度 > 尾指针)
				throw Image5D异常(像素块不完整);
			块指针.push_back(像素指针);
			if ((基块指针 = (Oir基块*)((char*)文件头 + *(基块索引++))) + 1 > 尾指针)
				throw Image5D异常(Oir基块索引错误);
		} while (基块指针->类型 == Oir基块类型::UID);
		//这里的每块像素数可能有C通道上的重复，没关系，后面得到SizeC以后会消掉
		const 元数据块* 元数据块指针 = (元数据块*)(基块指针 + 1);
		if ((字符串 = (char*)(元数据块指针 + 1)) > 尾指针)
			throw Image5D异常(Oir元数据块不完整);
		if ((字符串 = (char*)((元数据块指针 = (元数据块*)(字符串 + 元数据块指针->长度)) + 1)) > 尾指针)//跳过fileinfo
			throw Image5D异常(Oir元数据块不完整);
		uint32_t 长度 = 元数据块指针->长度;
		if ((元数据块指针 = (元数据块*)(字符串 + 长度)) > 尾指针)
			throw Image5D异常(Oir图像属性出界);
		xml_parse_status 解析结果;
		xml_document 图像属性文档;
		if ((解析结果 = 图像属性文档.load_buffer(字符串, 长度).status) != xml_parse_status::status_ok)
			throw Image5D异常(图像属性解析失败, 解析结果);
		xml_node 节点 = 图像属性文档.child("lsmimage:imageProperties");
		if (!节点)
			throw Image5D异常(图像属性未定义);
		xml_node 父节点 = 节点.child("commonimage:acquisition");
		if (!父节点)
			throw Image5D异常(图像采集未定义);
		if (!(节点 = 父节点.child("commonimage:imagingParam")))
			throw Image5D异常(成像参数未定义);
		xml_attribute 节点属性;
		xml_text 节点文本;
		Oir索引 新索引;
		if (有Z)
		{
			新索引.SizeZ = 0;
			for (xml_node 节点 : 节点.children("commonparam:axis"))
				if ((节点属性 = 节点.attribute("paramEnable")) && 节点属性.as_bool())
				{
					if (!((节点 = 节点.child("commonparam:maxSize")) && (节点文本 = 节点.text())))
						throw Image5D异常(Z层尺寸未定义);
					新索引.SizeZ = 节点文本.as_uint();
					break;
				}
			if (!新索引.SizeZ)
				throw Image5D异常(Z层轴未定义);
		}
		else
			新索引.SizeZ = 1;
		if (!(节点 = 父节点.child("commonimage:phase")))
			throw Image5D异常(图像相位未定义);
		std::vector<通道设备> 通道设备向量;
		uint8_t 通道组个数 = 0;
		for (xml_node 节点 : 节点.children("commonphase:group"))
		{
			通道组个数++;
			xml_node 通道;
			if (!(通道 = 节点.child("commonphase:channel")))
				throw Image5D异常(相位通道未定义);
			if (!(节点属性 = 通道.attribute("enable")))
				throw Image5D异常(通道enable未定义);
			if (!节点属性.as_bool())
				continue;
			通道设备向量.emplace_back();
			通道设备& 通道设备对象 = 通道设备向量.back();
			if (!(节点属性 = 通道.attribute("id")))
				throw Image5D异常(通道id未定义);
			通道设备对象.通道 = 节点属性.as_string();
			if (!(节点属性 = 通道.attribute("order")))
				throw Image5D异常(通道order未定义);
			通道设备对象.顺序 = 节点属性.as_uint();
			if (!((节点 = 通道.child("commonphase:deviceName")) && (节点文本 = 节点.text())))
				throw Image5D异常(通道设备名未定义);
			通道设备对象.设备 = 节点文本.as_string();
		}
		if (通道设备向量.empty())
			throw Image5D异常(通道未定义);
		const uint8_t SizeC = 新索引.SizeC = 通道设备向量.size();
		std::sort(通道设备向量.begin(), 通道设备向量.end(), [](const 通道设备& 对象1, const 通道设备& 对象2) {return 对象1.顺序 <= 对象2.顺序; });
		std::unique_ptr<设备颜色[]> 通道颜色 = std::make_unique_for_overwrite<设备颜色[]>(SizeC);
		for (uint8_t C = 0; C < SizeC; ++C)
			if (strcpy_s(通道颜色[C].设备名称, 通道设备向量[C].设备))
				throw Image5D异常(设备名称太长);
		if (!(节点 = 父节点.child("commonimage:configuration")))
			throw Image5D异常(图像配置未定义);
		if (!((节点 = 节点.child("lsmimage:scannerType")) && (节点文本 = 节点.text())))
			throw Image5D异常(扫描类型未定义);
		字符串 = 节点文本.as_string();
		新索引.SizeX = 0, 新索引.SizeY = 0, 新索引.系列间隔 = 0;
		for (xml_node 节点 : 父节点.children("lsmimage:scannerSettings"))
		{
			if (!(节点属性 = 节点.attribute("type")))
				throw Image5D异常(扫描类型未定义);
			if (strcmp(节点属性.value(), 字符串))
				continue;
			const xml_node 扫描参数 = 节点.child("lsmimage:param");
			if (!扫描参数)
				throw Image5D异常(扫描参数未定义);
			const xml_node 图像尺寸 = 扫描参数.child("lsmparam:imageSize");
			if (!图像尺寸)
				throw Image5D异常(图像尺寸未定义);
			if (!((节点 = 图像尺寸.child("commonparam:width")) && (节点文本 = 节点.text())))
				throw Image5D异常(扫描宽度未定义);
			新索引.SizeX = 节点文本.as_uint();
			if (!((节点 = 图像尺寸.child("commonparam:height")) && (节点文本 = 节点.text())))
				throw Image5D异常(扫描高度未定义);
			新索引.SizeY = 节点文本.as_uint();
			if (!(节点 = 扫描参数.child("lsmparam:speed")))
				throw Image5D异常(扫描速度未定义);
			if (!(节点 = 节点.child("commonparam:speedInformation")))
				throw Image5D异常(速度信息未定义);
			if (!((节点 = 节点.child("commonparam:seriesInterval")) && (节点文本 = 节点.text())))
				throw Image5D异常(系列间隔未定义);
			新索引.系列间隔 = 节点文本.as_float();
			break;
		}
		if (!(新索引.SizeX && 新索引.SizeY && 新索引.系列间隔))
			throw Image5D异常(扫描设置未定义);
		//不能复用图像解析文档，会损坏通道设备指针的通道
		xml_document LUT文档;
		if ((字符串 = (char*)(元数据块指针 + 1)) > 尾指针)
			throw Image5D异常(Oir元数据块不完整);
		if ((字符串 = (char*)((元数据块指针 = (元数据块*)(字符串 + 元数据块指针->长度)) + 1)) > 尾指针)//跳过annotation
			throw Image5D异常(Oir元数据块不完整);
		元数据块指针 = (元数据块*)(字符串 + 元数据块指针->长度);//跳过overlay
		const uint32_t* 长度指针 = (uint32_t*)(元数据块指针 + 1);
		for (uint8_t C1 = 0; C1 < 通道组个数; ++C1)
		{
			const char* const UID头 = (char*)(长度指针 + 1);
			if (UID头 > 尾指针)
				throw Image5D异常(LutUid长度出界);
			const char* const UID尾 = UID头 + *长度指针;
			if ((长度指针 = (uint32_t*)UID尾) > 尾指针)
				throw Image5D异常(LutUid出界);
			if ((字符串 = (char*)(长度指针 + 1)) > 尾指针)
				throw Image5D异常(LutXml长度出界);
			if ((长度指针 = (uint32_t*)(字符串 + (长度 = *长度指针))) > 尾指针)
				throw Image5D异常(LutXml出界);
			//确保即使没有找到也能正确跳到下一个LutXml指针位置
			for (uint8_t C2 = 0; C2 < SizeC; ++C2)
				if (std::equal(UID头, UID尾, 通道设备向量[C2].通道))
				{
					if ((解析结果 = LUT文档.load_buffer(字符串, 长度).status) != xml_parse_status::status_ok)
						throw Image5D异常(LutXml解析失败, 解析结果);
					if (!(父节点 = LUT文档.child("lut:LUT")))
						throw Image5D异常(查找表未定义);
					设备颜色& 通道 = 通道颜色[C2];
					if (!((节点 = 父节点.child("lut:red")) && (节点 = 节点.child("lut:contrast")) && (节点文本 = 节点.text())))
						throw Image5D异常(红色分量未定义);
					通道.红 = 节点文本.as_float();
					if (!((节点 = 父节点.child("lut:green")) && (节点 = 节点.child("lut:contrast")) && (节点文本 = 节点.text())))
						throw Image5D异常(绿色分量未定义);
					通道.绿 = 节点文本.as_float();
					if (!((节点 = 父节点.child("lut:blue")) && (节点 = 节点.child("lut:contrast")) && (节点文本 = 节点.text())))
						throw Image5D异常(蓝色分量未定义);
					通道.蓝 = 节点文本.as_float();
					if (!((节点 = 父节点.child("lut:alpha")) && (节点 = 节点.child("lut:contrast")) && (节点文本 = 节点.text())))
						throw Image5D异常(不透明度未定义);
					通道.不透明度 = 节点文本.as_float();
					break;
				}
		}
		const uint8_t 每层像素块数 = 每块像素数向量.size();
		const uint8_t 每层基块数 = (每层像素块数 + 1) * 2;
		基块索引 += 1;
		while (基块索引 + 每层基块数 < 尾指针)
		{
			for (uint8_t a = 0; a < 每层像素块数; ++a)
				块指针.push_back((uint16_t*)((Oir基块*)((char*)文件头 + *(基块索引 += 2)) + 1));
			基块索引 += 2;
		}
		const 文件列表类::const_iterator 文件结束 = 文件列表.cend();
#ifdef _DEBUG
		uint8_t 文件序号 = 0;
#endif
		while (++当前文件 < 文件结束)
		{
#ifdef _DEBUG
			if (++文件序号 == 80)
				文件序号 = 文件序号;
#endif
			文件头 = (Oir文件头*)(*当前文件)->映射指针();
			尾指针 = (char*)文件头 + (*当前文件)->文件大小();
			if (文件头 + 1 > 尾指针)
				[[unlikely]] continue;
			基块索引 = (uint64_t*)((char*)文件头 + 文件头->索引位置 + 4);
			do 
			{
				if (基块索引 + 1 > 尾指针)
					[[unlikely]] goto 绝对跳出;
			}
			while (((Oir基块*)((char*)文件头 + *(基块索引++)))->类型 != Oir基块类型::帧属性);//跳过REF块
			if ((基块索引--) + 每层基块数 > 尾指针)
				[[unlikely]] continue;
			for (uint8_t a = 0; a < 每层像素块数; ++a)
				块指针.push_back((uint16_t*)((Oir基块*)((char*)文件头 + *(基块索引 += 2)) + 1));
			基块索引 += 3;
			while (基块索引 + 每层基块数 < 尾指针)
			{
				for (uint8_t a = 0; a < 每层像素块数; ++a)
					块指针.push_back((uint16_t*)((Oir基块*)((char*)文件头 + *(基块索引 += 2)) + 1));
				基块索引 += 2;
			}
		绝对跳出:;
		}
		新索引.每帧分块数 = 每层像素块数 / SizeC;
		const uint32_t 块总数 = 块指针.size();
		const uint32_t 文件大小 = 新索引.计算文件大小() + 块总数 * sizeof(const uint16_t*);
		索引文件->文件大小(文件大小);
		索引文件->映射指针(nullptr);
		*(索引 = (Oir索引*)索引文件->映射指针()) = 新索引;
		索引->计算依赖字段(块总数);
		块指针.resize(索引->SizeTZBC);
		uint64_t* 块偏移;
		索引->Get变长成员(每块像素数, i通道颜色, 块偏移);
		std::vector<uint32_t>::const_iterator 块像素头 = 每块像素数向量.cbegin();
		uint32_t* 每块像素头 = 每块像素数;
		//第0块像素数就是真正的第0块像素数，而第1块实际上可能还是第0块（如果索引->SizeC>1）
		for (const uint32_t* const 每块像素尾 = 每块像素头 + 索引->每帧分块数; 每块像素头 < 每块像素尾; 块像素头 += SizeC)
			*(每块像素头++) = *块像素头;
		std::copy_n(通道颜色.get(), SizeC, i通道颜色);
		try
		{
			for (const uint16_t* 指针 : 块指针)
				*(块偏移++) = (char*)指针 - 全局文件头;
		}
		catch (...)
		{
			throw Image5D异常(索引块偏移写出失败);
		}
		索引->哈希签名(文件大小);
		if (!索引->验证(文件大小))
			throw Image5D异常(调试断点);
	}
}
void Oir读入器::读入像素(uint16_t* 写出头TZ, uint32_t TStart, uint32_t TSize, uint8_t ZStart, uint8_t ZSize, uint8_t CStart, uint8_t CSize)const
{
	if (TStart + TSize > 索引->SizeT || ZStart + ZSize > 索引->SizeZ || CStart + CSize > 索引->SizeC)
		throw 越界异常;
	const uint16_t* const* 读入头T = 块指针.data() + (TStart * 索引->SizeZ + ZStart) * 索引->每帧分块数 * 索引->SizeC + CStart;
	const uint16_t* const* const 读入尾T = 读入头T + TSize * 索引->SizeZBC;
	const uint16_t 读入ZBC = (uint16_t)ZSize * 索引->SizeBC;
	const uint32_t 写出CYX = uint32_t(CSize) * 索引->SizeYX;
	try
	{
		while (读入头T < 读入尾T)
		{
			const uint16_t* const* 读入头ZB = 读入头T;
			const uint16_t* const* 读入尾Z = 读入头ZB + 读入ZBC;
			while (读入头ZB < 读入尾Z)
			{
				const uint32_t* 块像素头 = 每块像素数;
				const uint16_t* const* 读入尾B = 读入头ZB + 索引->SizeBC;
				uint16_t* 写出头B = 写出头TZ;
				while (读入头ZB < 读入尾B)
				{
					const uint16_t* const* 读入头C = 读入头ZB;
					const uint16_t* const* 读入尾C = 读入头C + CSize;
					uint16_t* 写出头C = 写出头B;
					uint32_t 块像素数 = *块像素头;
					while (读入头C < 读入尾C)
					{
						std::copy_n(*读入头C, 块像素数, 写出头C);
						读入头C++;
						写出头C += 索引->SizeYX;
					}
					读入头ZB += 索引->SizeC;
					写出头B += 块像素数;
					块像素头++;
				}
				写出头TZ += 写出CYX;
			}
			读入头T += 索引->SizeZBC;
		}
	}
	catch (...)
	{
		throw 内存异常;
	}
}
void Oir读入器::读入像素(uint16_t* 写出头TZ, uint32_t TStart, uint32_t TSize, uint8_t ZStart, uint8_t ZSize)const
{
	if (TStart + TSize > 索引->SizeT || ZStart + ZSize > 索引->SizeZ)
		throw 越界异常;
	const uint16_t* const* 读入头T = 块指针.data() + (TStart * 索引->SizeZ + ZStart) * 索引->每帧分块数 * 索引->SizeC;
	const uint16_t* const* const 读入尾T = 读入头T + TSize * 索引->SizeZBC;
	const uint16_t 读入ZBC = (uint16_t)ZSize * 索引->SizeBC;
	try
	{
		while (读入头T < 读入尾T)
		{
			const uint16_t* const* 读入头ZB = 读入头T;
			const uint16_t* const* 读入尾Z = 读入头ZB + 读入ZBC;
			while (读入头ZB < 读入尾Z)
			{
				const uint32_t* 块像素头 = 每块像素数;
				const uint16_t* const* 读入尾B = 读入头ZB + 索引->SizeBC;
				uint16_t* 写出头B = 写出头TZ;
				while (读入头ZB < 读入尾B)
				{
					const uint16_t* const* 读入尾C = 读入头ZB + 索引->SizeC;
					uint16_t* 写出头C = 写出头B;
					uint32_t 块像素数 = *块像素头;
					while (读入头ZB < 读入尾C)
					{
						std::copy_n(*(读入头ZB++), 块像素数, 写出头C);
						写出头C += 索引->SizeYX;
					}
					写出头B += 块像素数;
					块像素头++;
				}
				写出头TZ += 索引->SizeCYX;
			}
			读入头T += 索引->SizeZBC;
		}
	}
	catch (...)
	{
		throw 内存异常;
	}
}
void Oir读入器::读入像素(uint16_t* 写出头, uint32_t TStart, uint32_t TSize, uint8_t C)const
{
	if (TStart + TSize > 索引->SizeT || C >= 索引->SizeC)
		throw 越界异常;
	const uint16_t* const* 读入头 = 块指针.data() + TStart * 索引->SizeZBC + C;
	const uint16_t* const* const 读入尾T = 读入头 + TSize * 索引->SizeZBC;
	try
	{
		while (读入头 < 读入尾T)
		{
			const uint16_t* const* 读入尾B = 读入头 + 索引->SizeBC;
			const uint32_t* 块像素头 = 每块像素数;
			while (读入头 < 读入尾B)
			{
				std::copy_n(*读入头, *块像素头, 写出头);
				读入头 += 索引->SizeC;
				写出头 += *块像素头;
				块像素头++;
			}
		}
	}
	catch (...)
	{
		throw 内存异常;
	}
}
void Oir读入器::读入像素(uint16_t* 写出头TZ, uint32_t TStart, uint32_t TSize)const
{
	if (TStart + TSize > 索引->SizeT)
		throw 越界异常;
	const uint16_t* const* 读入头 = 块指针.data() + TStart * 索引->SizeZBC;
	const uint16_t* const* const 读入尾T = 读入头 + TSize * 索引->SizeZBC;
	try
	{
		while (读入头 < 读入尾T)
		{
			const uint16_t* const* 读入尾B = 读入头 + 索引->SizeBC;
			uint16_t* 写出头B = 写出头TZ;
			const uint32_t* 块像素头 = 每块像素数;
			while (读入头 < 读入尾B)
			{
				const uint16_t* const* 读入尾C = 读入头 + 索引->SizeC;
				uint32_t 块像素数 = *块像素头;
				uint16_t* 写出头C = 写出头B;
				while (读入头 < 读入尾C)
				{
					std::copy_n(*读入头, 块像素数, 写出头C);
					读入头++;
					写出头C += 索引->SizeYX;
				}
				写出头B += 块像素数;
				块像素头++;
			}
			写出头TZ += 索引->SizeCYX;
		}
	}
	catch (...)
	{
		throw 内存异常;
	}
}
void Oir读入器::读入像素(uint16_t* 写出头TZ)const
{
	const uint16_t* const* 读入头 = 块指针.data();
	const uint16_t* const* const 读入尾T = 读入头 + 索引->SizeTZBC;
	try
	{
		while (读入头 < 读入尾T)
		{
			const uint16_t* const* 读入尾B = 读入头 + 索引->SizeBC;
			uint16_t* 写出头B = 写出头TZ;
			const uint32_t* 块像素头 = 每块像素数;
			while (读入头 < 读入尾B)
			{
				const uint16_t* const* 读入尾C = 读入头 + 索引->SizeC;
				uint32_t 块像素数 = *块像素头;
				uint16_t* 写出头C = 写出头B;
				while (读入头 < 读入尾C)
				{
					std::copy_n(*读入头, 块像素数, 写出头C);
					读入头++;
					写出头C += 索引->SizeYX;
				}
				写出头B += 块像素数;
				块像素头++;
			}
			写出头TZ += 索引->SizeCYX;
		}
	}
	catch (...)
	{
		throw 内存异常;
	}
}