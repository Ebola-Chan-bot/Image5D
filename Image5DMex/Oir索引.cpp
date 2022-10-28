#include "pch.h"
#include "Oir索引.h"
#include "哈希计算器.h"
bool Oir索引::验证(ULONG 文件大小)const noexcept
{
	哈希输入(文件大小 - 哈希长度, (PUCHAR)this + 哈希长度);
	UCHAR 哈希值[哈希长度];
	哈希输出(哈希长度, 哈希值);
	return !memcmp(哈希值, SHA256, 哈希长度);
}
void Oir索引::计算依赖字段(uint32_t 块总数)noexcept
{
	SizeBC = 每帧分块数 * SizeC;
	const uint32_t SizeTZ = 块总数 / SizeBC;
	if (SizeTZ < SizeZ)
		SizeZ = SizeTZ;
	SizeT = SizeTZ / SizeZ;
	SizeZBC = SizeZ * SizeBC;
	SizeTZBC = SizeT * SizeZBC;
	SizeYX = (UINT32)SizeY * SizeX;
	SizeCYX = SizeC * SizeYX;
}
void Oir索引::哈希签名(ULONG 文件大小)noexcept
{
	哈希输入(文件大小 - 哈希长度, (PUCHAR)this + 哈希长度);
	哈希输出(哈希长度, SHA256);
}
void Oir索引::Get变长成员(UINT32*& 每块像素数, 设备颜色*& 通道颜色, UINT64*& 块偏移)const noexcept
{
	每块像素数 = (UINT32*)(this + 1);
	通道颜色 = (设备颜色*)(每块像素数 + 每帧分块数);
	块偏移 = (UINT64*)(通道颜色 + SizeC);
}