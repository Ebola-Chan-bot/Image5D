#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Oir索引.h"
#include "文件控制块.h"
#include "内存优化库.h"
using namespace std;
using 文件列表类 = vector<unique_ptr<文件控制块>>;
using 块指针类 = vector<const uint16_t*>;
class Oir读入器
{
	文件列表类 文件列表;
	unique_ptr<文件映射> 索引文件;
	Oir索引* 索引;//指向索引文件，无需释放
	UINT32* 每块像素数;//指向索引文件，无需释放
	块指针类 块指针;
	设备颜色* i通道颜色;//指向索引文件，无需释放
public:
	UINT16 SizeX()const noexcept{ return 索引->SizeX; }
	UINT16 SizeY()const noexcept { return 索引->SizeY; }
	UINT8 SizeC()const noexcept { return 索引->SizeC; }
	UINT8 SizeZ()const noexcept { return 索引->SizeZ; }
	UINT16 SizeT()const noexcept { return 索引->SizeT; }
	float 系列间隔()const noexcept { return 索引->系列间隔; }
	const 设备颜色* 通道颜色()const noexcept { return i通道颜色; }
	void 读入像素(UINT16* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const;
	void 读入像素(UINT16* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 C)const;
	void 读入像素(UINT16* 缓冲区, UINT16 TStart, UINT16 TSize)const;
	void 读入像素(uint16_t* 缓冲区)const;
	Oir读入器(LPCSTR 头文件路径);
	Oir读入器(LPCWSTR 头文件路径);
	//禁止拷贝，以免意外的析构
	Oir读入器(const Oir读入器&) = delete;
};