#include "pch.h"
#include <Shlwapi.h>
#include <stdio.h>
#include <numeric>
#include "Oir读入器.h"
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
constexpr const char* 字符串尾(const char* 字符串)
{
	while (*字符串)
		字符串++;
	return 字符串;
}
constexpr const char* XML标头 = "<?xml version=\"1.0\" encoding=\"ASCII\"?>\r\n";
constexpr const char* XML标头尾 = 字符串尾(XML标头);
constexpr uint8_t XML标头长度 = XML标头尾 - XML标头;
void 扫描XML块(const char*& s1指针, const void*& 尾指针, 文件列表类::const_iterator& 文件头, const 文件列表类::const_iterator& 文件尾)
{
	while ((s1指针 = std::search(s1指针, (const char*)尾指针, XML标头, XML标头尾)) >= 尾指针)
		if (++文件头 < 文件尾)
			尾指针 = (s1指针 = (char*)(*文件头)->映射指针()) + (*文件头)->文件大小();
		else
			throw;
}
using namespace pugi;
Oir读入器::Oir读入器(LPCWSTR 头文件路径)
{
	const size_t 长度 = wcslen(头文件路径) + 1;
	//make_unique会对内存初始化，不用于分配POD数组
	const std::unique_ptr<wchar_t[]> 字符缓冲 = std::make_unique_for_overwrite<wchar_t[]>(长度 * 5 + 6);
	const LPWSTR 驱动器号 = 字符缓冲.get();
	const LPWSTR 目录路径 = 驱动器号 + 长度;
	const LPWSTR 基文件名 = 目录路径 + 长度;
	const LPWSTR 文件扩展名 = 基文件名 + 长度;
	const LPWSTR 当前路径 = 文件扩展名 + 长度;
	_wsplitpath(头文件路径, 驱动器号, 目录路径, 基文件名, 文件扩展名);
	_wmakepath(当前路径, 驱动器号, 目录路径, 基文件名, nullptr);
	const LPWSTR 分隔符位置 = 当前路径 + wcslen(当前路径);
	wcscpy(分隔符位置, 文件扩展名);
	const LPWSTR 编号位置 = 分隔符位置 + 1;
	文件列表.push_back(std::make_unique<文件控制块>(当前路径));
	UINT64 总映射空间 = 文件列表[0]->文件大小();
	*分隔符位置 = L'_';
	UINT8 文件数目 = 1;
	while (true)
	{
		swprintf(编号位置, L"%05u", 文件数目++);
		try
		{
			文件列表.push_back(std::make_unique<文件控制块>(当前路径));
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
		尾指针 = (char*)尾指针+文件->粒度大小;
	}
	wcscpy(分隔符位置, L".Oir索引");
	try
	{
		//直接throw;无法捕获
		索引文件 = std::make_unique<文件映射>(当前路径, true);
		索引文件->映射指针(nullptr);
		索引 = (Oir索引*)索引文件->映射指针();
		if (sizeof(索引) > 索引文件->文件大小() || !索引->验证(索引文件->文件大小()))
			throw 0;
		UINT64* 块偏移;
		索引->Get变长成员(每块像素数, i通道颜色, 块偏移);
		const UINT32 块总数 = UINT32(索引->SizeZBC) * 索引->SizeT;
		if (!块总数)
			throw 0;
		if ((char*)(块偏移 + 块总数) > (char*)索引文件->映射指针() + 索引文件->文件大小())
			throw 0;
		const char* const 映射指针 = (char*)文件列表[0]->映射指针();
		if (映射指针 + 块偏移[块总数 - 1] > 尾指针)
			throw 0;
		for (const UINT64* const 块偏移尾 = 块偏移 + 块总数; 块偏移 < 块偏移尾; ++块偏移)
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
		索引文件 = std::make_unique<文件映射>(当前路径, 1ll);
		constexpr const char* 图像属性标头 = "<lsmimage";
		constexpr const char* 图像标头尾 = 字符串尾(图像属性标头);
		constexpr const char* 查找表标头 = "<lut";
		constexpr const char* 查找表标头尾 = 字符串尾(查找表标头);
		constexpr const char* 帧属性标头 = "<lsmframe";
		constexpr const char* 帧标头尾 = 字符串尾(帧属性标头);
		constexpr uint8_t UUID长度 = 36;
		const 文件控制块& 当前文件 = *文件列表[0];
		const char* const 映射指针 = (char*)当前文件.映射指针();
		const UID块* UID块指针 = (UID块*)((char*)映射指针 + 96);
		//虽然内存映射文件是连续的，但分配粒度导致的文件之间存在空隙不可访问，因此必须用尾指针加以限制
		const void* 尾指针 = 映射指针 + 当前文件.文件大小();
		if (UID块指针 + 1 > 尾指针)
			throw Image5D异常(文件不包含块);
		const 像素块* 像素块指针;
		while (UID块指针->Check == 3)
		{
			//说明有REF图块，需要跳过
			像素块指针 = (像素块*)((char*)(UID块指针 + 1) + UID块指针->UID长度);
			if (像素块指针 + 1 > 尾指针)
				throw Image5D异常(REF块不完整);
			UID块指针 = (UID块*)((char*)(像素块指针 + 1) + 像素块指针->像素长度);
			if (UID块指针 + 1 > 尾指针)
				throw Image5D异常(REF块不完整);
		}
		const char* s1指针;
		if ((s1指针 = std::search((char*)UID块指针, (char*)尾指针, XML标头, XML标头尾)) >= 尾指针)
			throw Image5D异常(找不到帧标头);
		while (!std::equal(帧属性标头, 帧标头尾, s1指针 + XML标头长度))
			if ((s1指针 = std::search(s1指针 += *((uint32_t*)s1指针 - 1), (const char*)尾指针, XML标头, XML标头尾)) >= 尾指针) //必须带等号，否则可能无限循环
				throw Image5D异常(找不到帧标头);
		UID块指针 = (UID块*)(s1指针 + *((uint32_t*)s1指针 - 1));
		const char* UID字符串 = (char*)(UID块指针 + 1);
		if (UID字符串 > 尾指针)
			throw Image5D异常(UID块不完整);
		if (UID块指针->Check != 3)
			throw Image5D异常(空的像素块);
		std::vector<uint32_t> 每块像素数向量;
		const bool 有Z = UID字符串[0] == 'z';
		while (UID块指针->Check == 3)
		{
			像素块指针 = (像素块*)(UID字符串 + UID块指针->UID长度);
			s1指针 = (char*)(像素块指针 + 1);
			if (s1指针 > 尾指针)
				throw Image5D异常(像素块不完整);
			每块像素数向量.push_back(像素块指针->像素长度 / 2);
			块指针.push_back((uint16_t*)s1指针);
			UID块指针 = (UID块*)(s1指针 + 像素块指针->像素长度);
			UID字符串 = (char*)(UID块指针 + 1);
			if (UID字符串 > 尾指针)
				throw Image5D异常(UID块不完整);
		}
		if ((s1指针 = std::search((char*)UID块指针, (char*)尾指针, XML标头, XML标头尾)) >= 尾指针)
			throw Image5D异常(找不到图像标头);
		while (!std::equal(图像属性标头, 图像标头尾, s1指针 + XML标头长度))
			if ((s1指针 = std::search(s1指针 += *((uint32_t*)s1指针 - 1), (const char*)尾指针, XML标头, XML标头尾)) >= 尾指针) //必须带等号，否则可能无限循环
				throw Image5D异常(找不到图像标头);
		uint32_t 长度;
		if (s1指针 + (长度 = *((uint32_t*)s1指针 - 1)) > 尾指针)
			throw Image5D异常(图像属性不完整);
		xml_parse_status 解析结果;
		xml_document 图像属性文档;
		if ((解析结果 = 图像属性文档.load_buffer(s1指针, 长度).status) != xml_parse_status::status_ok)
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
					//这个属性值只是设定的Z层数，实际拍摄时可能关闭了Z，只拍1层。必须再检查commonparam:z
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
		const char* 属性值;
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
			通道设备向量.push_back(通道设备());
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
		const UINT8 SizeC = 新索引.SizeC = 通道设备向量.size();
		std::sort(通道设备向量.begin(), 通道设备向量.end(), [](const 通道设备& 对象1, const 通道设备& 对象2) {return 对象1.顺序 <= 对象2.顺序; });
		std::unique_ptr<设备颜色[]> 通道颜色 = std::make_unique_for_overwrite<设备颜色[]>(SizeC);
		for (uint8_t C = 0; C < SizeC; ++C)
			if (strcpy_s(通道颜色[C].设备名称, 通道设备向量[C].设备))
				throw Image5D异常(设备名称太长);
		if (!(节点 = 父节点.child("commonimage:configuration")))
			throw Image5D异常(图像配置未定义);
		if (!((节点 = 节点.child("lsmimage:scannerType")) && (节点文本 = 节点.text())))
			throw Image5D异常(扫描类型未定义);
		属性值 = 节点文本.as_string();
		新索引.SizeX = 0, 新索引.SizeY = 0, 新索引.系列间隔 = 0;
		for (xml_node 节点 : 父节点.children("lsmimage:scannerSettings"))
		{
			if (!(节点属性 = 节点.attribute("type")))
				throw Image5D异常(扫描类型未定义);
			if (strcmp(节点属性.value(), 属性值))
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
		for (uint8_t C1 = 0; C1 < 通道组个数; ++C1)
		{
			if ((s1指针 = std::search(s1指针 + 长度, (const char*)尾指针, XML标头, XML标头尾)) >= 尾指针)
				throw Image5D异常(找不到查找表);
			while (!std::equal(查找表标头, 查找表标头尾, s1指针 + XML标头长度))
				if ((s1指针 = std::search(s1指针 += *((uint32_t*)s1指针 - 1), (const char*)尾指针, XML标头, XML标头尾)) >= 尾指针) //必须带等号，否则可能无限循环
					throw Image5D异常(找不到查找表);
			长度 = *((uint32_t*)s1指针 - 1);
			if ((解析结果 = LUT文档.load_buffer(s1指针, 长度).status) != xml_parse_status::status_ok)
				throw Image5D异常(找不到查找表, 解析结果);
			if (!(父节点 = LUT文档.child("lut:LUT")))
				throw Image5D异常(查找表未定义);
			属性值 = (char*)((uint32_t*)s1指针 - 1) - UUID长度;
			for (uint8_t C2 = 0; C2 < SizeC; ++C2)
				if (std::equal(属性值, 属性值 + UUID长度, 通道设备向量[C2].通道))
				{
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
		if ((s1指针 = std::search(s1指针 + 长度, (const char*)尾指针, XML标头, XML标头尾)) >= 尾指针)
			throw Image5D异常(找不到帧标头);
		文件列表类::const_iterator 文件头 = 文件列表.cbegin();
		const 文件列表类::const_iterator 文件尾 = 文件列表.cend();
		while (true)
		{
			try
			{
				while (!std::equal(帧属性标头, 帧标头尾, s1指针 + XML标头长度))
					扫描XML块(s1指针 += *((uint32_t*)s1指针 - 1), 尾指针, 文件头, 文件尾);
				const UID块* UID块指针 = (UID块*)(s1指针 + *((uint32_t*)s1指针 - 1));
				if (UID块指针 + 1 > 尾指针)
					break;
#ifdef _DEBUG
				bool 中断 = false;
#endif
				while (UID块指针->Check == 3)
				{
#ifdef _DEBUG
					if (中断)
						throw Image5D异常(调试断点);
					const char* UID字符串 = (char*)(UID块指针 + 1);
					if (UID字符串[1] == '9' && UID字符串[2] == '9' && UID字符串[3] == '9')
						中断 = false;
#endif
					const 像素块* 像素块指针 = (像素块*)((char*)(UID块指针 + 1) + UID块指针->UID长度);
					s1指针 = (char*)(像素块指针 + 1);
					UID块指针 = (UID块*)(s1指针 + 像素块指针->像素长度);
					if (UID块指针 > 尾指针)
						throw;
					块指针.push_back((uint16_t*)s1指针);
					if (UID块指针 + 1 > 尾指针)
						throw;
				}
				扫描XML块(s1指针 = (char*)UID块指针, 尾指针, 文件头, 文件尾);
			}
			catch (...)
			{
				break;
			}
		}
		新索引.每帧分块数 = 每块像素数向量.size() / SizeC;
		const UINT32 块总数 = 块指针.size();
		const size_t 文件大小 = 新索引.计算文件大小() + 块总数 * sizeof(const UINT16*);
		索引文件->文件大小(文件大小);
		索引文件->映射指针(nullptr);
		*(索引 = (Oir索引*)索引文件->映射指针()) = 新索引;
		索引->计算依赖字段(块总数);
		块指针.resize(索引->SizeTZBC);
		UINT64* 块偏移;
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
				*(块偏移++) = (char*)指针 - 映射指针;
		}
		catch (...)
		{
			throw Image5D异常(索引块偏移写出失败);
		}
		索引->哈希签名(文件大小);
	}
}
void Oir读入器::读入像素(UINT16* 写出头TZ, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const
{
	if (TStart + TSize > 索引->SizeT || ZStart + ZSize > 索引->SizeZ || CStart + CSize > 索引->SizeC)
		throw 越界异常;
	const UINT16* const* 读入头T = 块指针.data() + (UINT32(TStart) * 索引->SizeZ + ZStart) * 索引->每帧分块数 * 索引->SizeC + CStart;
	const UINT16* const* const 读入尾T = 读入头T + UINT32(TSize) * 索引->SizeZBC;
	const UINT8 读入ZBC = ZSize * 索引->SizeBC;
	const UINT32 写出CYX = UINT32(CSize) * 索引->SizeYX;
	try
	{
		while (读入头T < 读入尾T)
		{
			const UINT16* const* 读入头ZB = 读入头T;
			const UINT16* const* 读入尾Z = 读入头ZB + 读入ZBC;
			while (读入头ZB < 读入尾Z)
			{
				const UINT32* 块像素头 = 每块像素数;
				const UINT16* const* 读入尾B = 读入头ZB + 索引->SizeBC;
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
void Oir读入器::读入像素(UINT16* 写出头TZ, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize)const
{
	if (TStart + TSize > 索引->SizeT || ZStart + ZSize > 索引->SizeZ)
		throw 越界异常;
	const UINT16* const* 读入头T = 块指针.data() + (UINT32(TStart) * 索引->SizeZ + ZStart) * 索引->每帧分块数 * 索引->SizeC;
	const UINT16* const* const 读入尾T = 读入头T + UINT32(TSize) * 索引->SizeZBC;
	const UINT8 读入ZBC = ZSize * 索引->SizeBC;
	try
	{
		while (读入头T < 读入尾T)
		{
			const UINT16* const* 读入头ZB = 读入头T;
			const UINT16* const* 读入尾Z = 读入头ZB + 读入ZBC;
			while (读入头ZB < 读入尾Z)
			{
				const UINT32* 块像素头 = 每块像素数;
				const UINT16* const* 读入尾B = 读入头ZB + 索引->SizeBC;
				UINT16* 写出头B = 写出头TZ;
				while (读入头ZB < 读入尾B)
				{
					const UINT16* const* 读入尾C = 读入头ZB + 索引->SizeC;
					UINT16* 写出头C = 写出头B;
					UINT32 块像素数 = *块像素头;
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
void Oir读入器::读入像素(UINT16* 写出头, UINT16 TStart, UINT16 TSize, UINT8 C)const
{
	if (TStart + TSize > 索引->SizeT || C >= 索引->SizeC)
		throw 越界异常;
	const UINT16* const* 读入头 = 块指针.data() + UINT32(TStart) * 索引->SizeZBC + C;
	const UINT16* const* const 读入尾T = 读入头 + UINT32(TSize) * 索引->SizeZBC;
	try
	{
		while (读入头 < 读入尾T)
		{
			const UINT16* const* 读入尾B = 读入头 + 索引->SizeBC;
			const UINT32* 块像素头 = 每块像素数;
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
void Oir读入器::读入像素(UINT16* 写出头TZ, UINT16 TStart, UINT16 TSize)const
{
	if (TStart + TSize > 索引->SizeT)
		throw 越界异常;
	const UINT16* const* 读入头 = 块指针.data() + UINT32(TStart) * 索引->SizeZBC;
	const UINT16* const* const 读入尾T = 读入头 + UINT32(TSize) * 索引->SizeZBC;
	try
	{
		while (读入头 < 读入尾T)
		{
			const UINT16* const* 读入尾B = 读入头 + 索引->SizeBC;
			UINT16* 写出头B = 写出头TZ;
			const UINT32* 块像素头 = 每块像素数;
			while (读入头 < 读入尾B)
			{
				const UINT16* const* 读入尾C = 读入头 + 索引->SizeC;
				UINT32 块像素数 = *块像素头;
				UINT16* 写出头C = 写出头B;
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
void Oir读入器::读入像素(UINT16* 写出头TZ)const
{
	const UINT16* const* 读入头 = 块指针.data();
	const UINT16* const* const 读入尾T = 读入头 + 索引->SizeTZBC;
	try
	{
		while (读入头 < 读入尾T)
		{
			const UINT16* const* 读入尾B = 读入头 + 索引->SizeBC;
			UINT16* 写出头B = 写出头TZ;
			const UINT32* 块像素头 = 每块像素数;
			while (读入头 < 读入尾B)
			{
				const UINT16* const* 读入尾C = 读入头 + 索引->SizeC;
				UINT32 块像素数 = *块像素头;
				UINT16* 写出头C = 写出头B;
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