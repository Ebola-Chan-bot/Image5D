#pragma once
#include<vector>
#include"文件控制块.h"
#include"Oir索引.h"
namespace Image5D
{
	using 文件列表类 = std::vector<std::unique_ptr<文件控制块>>;
	using 块指针类 = std::vector<const uint16_t*>;
	class Oir读入器
	{
		文件列表类 文件列表;
		std::unique_ptr<文件映射> 索引文件;
		Oir索引* 索引;//指向索引文件，无需释放
		uint32_t* 每块像素数;//指向索引文件，无需释放
		块指针类 块指针;
		设备颜色* i通道颜色;//指向索引文件，无需释放
	public:
		uint16_t SizeX()const noexcept { return 索引->SizeX; }
		uint16_t SizeY()const noexcept { return 索引->SizeY; }
		uint8_t SizeC()const noexcept { return 索引->SizeC; }
		uint8_t SizeZ()const noexcept { return 索引->SizeZ; }
		uint32_t SizeT()const noexcept { return 索引->SizeT; }
		//拍摄时间周期的毫秒数
		float 系列间隔()const noexcept { return 索引->系列间隔; }
		const 设备颜色* 通道颜色()const noexcept { return i通道颜色; }
		void 读入像素(uint16_t* 缓冲区, uint32_t TStart, uint32_t TSize, uint8_t ZStart, uint8_t ZSize, uint8_t CStart, uint8_t CSize)const;
		void 读入像素(uint16_t* 缓冲区, uint32_t TStart, uint32_t TSize, uint8_t ZStart, uint8_t ZSize)const;
		void 读入像素(uint16_t* 缓冲区, uint32_t TStart, uint32_t TSize, uint8_t C)const;
		void 读入像素(uint16_t* 缓冲区, uint32_t TStart, uint32_t TSize)const;
		void 读入像素(uint16_t* 缓冲区)const;
		Oir读入器(LPCWSTR 头文件路径);
	};
}