#pragma once
#include <stdint.h>
struct 设备颜色
{
	char 设备名称[8];
	float 红;
	float 绿;
	float 蓝;
};
constexpr UINT8 哈希长度 = 32;
class Oir索引
{
	//依赖字段

	UCHAR SHA256[哈希长度];
public:
	//独立字段

	UINT8 每帧分块数;
	UINT16 SizeX;
	UINT16 SizeY;
	UINT8 SizeC;
	UINT8 SizeZ;
	UINT16 SizeT;
	float 系列间隔;
	//依赖字段

	UINT8 SizeBC;
	UINT8 SizeZBC;
	//此属性只能外部赋值
	uint32_t SizeTZBC;
	UINT32 SizeYX;
	UINT32 SizeCYX;
	bool 验证(ULONG 文件大小)const noexcept;
	//需要正确设置每帧分块数和SizeC
	size_t 计算文件大小()const noexcept{ return (char*)((设备颜色*)((UINT32*)(this + 1) + 每帧分块数) + SizeC) - (char*)this; }
	//需要正确设置每帧分块数和SizeC
	void Get变长成员(UINT32*& 每块像素数, 设备颜色*& 通道颜色, UINT64*& 块偏移)const noexcept;
	//需要正确设置SizeT以外的所有独立字段。此方法不能计算SizeTZBC。
	void 计算依赖字段()noexcept;
	//需要正确设置所有字段
	void 哈希签名(ULONG 文件大小)noexcept;
};