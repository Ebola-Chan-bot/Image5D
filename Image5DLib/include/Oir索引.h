#pragma once
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
		static constexpr uint8_t 库版本号 = 3;

		//隐藏字段

		uint8_t SHA256[哈希长度];
		uint8_t 文件版本号 = 库版本号;

	public:

		//独立字段

		uint16_t SizeX;
		uint16_t SizeY;
		uint8_t SizeZ;
		uint8_t SizeC;
		uint8_t 每帧分块数;
		uint8_t 文件序列拼接数;
		uint32_t LsmimageXml偏移;
		uint32_t LsmimageXml长度;
		float 系列间隔;
		float Z步长;
		char Z驱动单元类型[6];
		char 创建日期时间[30];

		//依赖字段

		uint8_t SizeBC;
		uint8_t SizeZC;
		uint16_t SizeZBC;
		uint32_t SizeT;
		uint32_t SizeTZBC;
		uint32_t SizeYX;
		uint32_t SizeCYX;
		bool 验证(uint32_t 文件大小)const;

		/*
		变长成员：
		激光透过率：float*SizeZ
		通道颜色：设备颜色*SizeC
		放大电压：uint16_t*SizeZ*SizeC
		每块像素数：uint32_t*每帧分块数
		块偏移：uint64_t*SizeTZBC
		拼接SizeT：uint16*文件序列拼接数
		*/
		//需要正确设置每帧分块数和SizeC
		uint32_t 计算文件大小()const noexcept { return sizeof(Oir索引) + sizeof(float) * SizeZ + sizeof(设备颜色) * SizeC + sizeof(uint16_t) * SizeZ * SizeC + sizeof(uint32_t) * 每帧分块数 + sizeof(uint64_t) * SizeTZBC+sizeof(uint16_t)*文件序列拼接数; }
		//需要正确设置每帧分块数和SizeC
		void Get变长成员(float*& 激光透过率, 设备颜色*& 通道颜色, uint16_t*& 放大电压, uint32_t*& 每块像素数, uint64_t*& 块偏移,uint16_t*&拼接SizeT)const noexcept
		{
			//应避免在此函数中使用依赖字段
			激光透过率 = (float*)(this + 1);
			通道颜色 = (设备颜色*)(激光透过率 + SizeZ);
			放大电压 = (uint16_t*)(通道颜色 + SizeC);
			每块像素数 = (uint32_t*)(放大电压 + SizeZ * SizeC);
			拼接SizeT = (uint16_t*)(每块像素数 + 每帧分块数);
			块偏移 = (uint64_t*)(拼接SizeT + 文件序列拼接数);
		}
		//需要正确设置所有独立字段
		void 计算依赖字段(uint32_t 块总数)noexcept;
		//需要正确设置所有字段
		void 哈希签名(uint32_t 文件大小)noexcept
		{
			哈希计算((char*)this + 哈希长度, 文件大小 - 哈希长度, SHA256);
		}
	};
}