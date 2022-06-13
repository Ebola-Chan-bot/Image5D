#pragma once
#include "Image5D.h"
class Oir读入器
{
	const UINT8 文件数目;
	const HANDLE* const 文件句柄;
	const HANDLE* const 映射句柄;
	const LPVOID* const 映射指针;
	const UINT8 每帧分块数;
	const UINT32* const 每块像素数;
	const UINT16*const* const 块指针;

	const UINT8 SizeBC;
	const UINT8 SizeZBC;
	const UINT32 SizeYX;
	const UINT32 SizeCYX;
	Oir读入器(UINT8 文件数目, const HANDLE* 文件句柄, const HANDLE* 映射句柄, const LPVOID* 映射指针, UINT8 每帧分块数, const UINT32* 每块像素数, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, float 系列间隔, const float* const* 通道颜色, const char* const* 设备名称, const char* 图像属性, const UINT16* const* 块指针) :文件数目(文件数目), 文件句柄(文件句柄), 映射句柄(映射句柄), 映射指针(映射指针), 每帧分块数(每帧分块数), 每块像素数(每块像素数), SizeX(SizeX), SizeY(SizeY), SizeC(SizeC), SizeZ(SizeZ), SizeT(SizeT), 系列间隔(系列间隔), 通道颜色(通道颜色), 设备名称(设备名称), 图像属性(图像属性), 块指针(块指针), SizeBC(每帧分块数* SizeC), SizeZBC(SizeBC* SizeZ), SizeYX(UINT32(SizeY)* SizeX), SizeCYX(SizeC* SizeYX) {}
public:
	const UINT16 SizeX;
	const UINT16 SizeY;
	const UINT8 SizeC;
	const UINT8 SizeZ;
	const UINT16 SizeT;
	const float 系列间隔;
	const float* const* const 通道颜色;
	const char* const* const 设备名称;
	const char* const 图像属性;
	bool 读入像素(UINT16* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const;
	bool 读入像素(UINT16* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 C)const;
	bool 读入像素(UINT16* 缓冲区, UINT16 TStart, UINT16 TSize)const;
	static Oir读入器* 创建(LPCSTR 头文件路径);
	~Oir读入器();
};