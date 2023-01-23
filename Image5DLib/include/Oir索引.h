#pragma once
#include <stdint.h>
#include "哈希计算器.h"
namespace Image5D
{
	//以下结构体将被写入Oir索引文件，必须紧密pack，否则字段空隙内在Release模式下会填充未定义值，计算哈希会出错。
	struct 设备颜色
	{
		char 设备名称[8];
		float 红;
		float 绿;
		float 蓝;
		float 不透明度;
	};
	class Oir索引
	{
		//依赖字段

		uint8_t SHA256[哈希长度];
	public:
		//独立字段

		uint16_t SizeX;
		uint16_t SizeY;
		uint8_t SizeC;
		uint8_t SizeZ;
		uint8_t 每帧分块数;
		float 系列间隔;
		//依赖字段

		uint8_t SizeBC;
		uint8_t SizeZBC;
		uint32_t SizeT;
		uint32_t SizeTZBC;
		uint32_t SizeYX;
		uint32_t SizeCYX;
		bool 验证(uint32_t 文件大小)const
#ifndef _DEBUG
			noexcept
#endif
			;
		//需要正确设置每帧分块数和SizeC
		uint32_t 计算文件大小()const noexcept { return (char*)((设备颜色*)((uint32_t*)(this + 1) + 每帧分块数) + SizeC) - (char*)this; }
		//需要正确设置每帧分块数和SizeC
		void Get变长成员(uint32_t*& 每块像素数, 设备颜色*& 通道颜色, uint64_t*& 块偏移)const noexcept
		{
			每块像素数 = (uint32_t*)(this + 1);
			通道颜色 = (设备颜色*)(每块像素数 + 每帧分块数);
			块偏移 = (uint64_t*)(通道颜色 + SizeC);
		}
		//需要正确设置所有独立字段
		void 计算依赖字段(uint32_t 块总数)noexcept;
		//需要正确设置所有字段
		void 哈希签名(uint32_t 文件大小)
#ifndef _DEBUG
			noexcept
#endif
		{
			哈希计算((char*)this + 哈希长度, 文件大小 - 哈希长度, SHA256);
		}
	};
}