#pragma once
#include<vector>
#include"文件控制块.h"
#include"Oir索引.h"
namespace Image5D
{
	using 文件列表类 = std::vector<std::unique_ptr<文件控制块>>;
	using 块指针类 = std::vector<const uint16_t*>;
	struct Oir读入器
	{
		uint16_t SizeX()const noexcept { return 索引->SizeX; }
		uint16_t SizeY()const noexcept { return 索引->SizeY; }
		uint8_t SizeC()const noexcept { return 索引->SizeC; }
		uint8_t SizeZ()const noexcept { return 索引->SizeZ; }
		uint32_t SizeT()const noexcept { return 索引->SizeT; }
		//拍摄时间周期的毫秒数
		float 系列间隔()const noexcept { return 索引->系列间隔; }
		//相邻Z层间距
		float Z步长()const noexcept { return 索引->Z步长; }
		const char* Z驱动单元类型()const noexcept { return 索引->Z驱动单元类型; }
		//数组尺寸为SizeC
		const 设备颜色* 通道颜色()const noexcept { return i通道颜色; }
		void LsmimageXml(const char*& 指针, uint32_t& 长度) const noexcept
		{
			指针 = (char*)文件列表[0]->映射指针() + 索引->LsmimageXml偏移; 
			长度 = 索引->LsmimageXml长度;
		}
		//数组尺寸为SizeZ
		const float* 激光透过率()const noexcept { return i激光透过率; }
		//数组尺寸为SizeZ*SizeC，维度顺序先Z后C
		const uint16_t* 放大电压()const noexcept { return i放大电压; }
		void 读入像素(uint16_t* 缓冲区, uint32_t TStart, uint32_t TSize, uint8_t ZStart, uint8_t ZSize, uint8_t CStart, uint8_t CSize)const;
		void 读入像素(uint16_t* 缓冲区, uint32_t TStart, uint32_t TSize, uint8_t ZStart, uint8_t ZSize)const;
		void 读入像素(uint16_t* 缓冲区, uint32_t TStart, uint32_t TSize, uint8_t C)const;
		void 读入像素(uint16_t* 缓冲区, uint32_t TStart, uint32_t TSize)const;
		void 读入像素(uint16_t* 缓冲区)const;
		Oir读入器(LPCWSTR 头文件路径);

	protected:
		文件列表类 文件列表;
		std::unique_ptr<文件映射> 索引文件;
		块指针类 块指针;

		//以下均为索引文件内部指针，随索引文件一同释放

		Oir索引* 索引;
		uint32_t* 每块像素数;
		设备颜色* i通道颜色;
		float* i激光透过率;
		uint16_t* i放大电压;
		void 创建新索引(const wchar_t* 字符缓冲);
	};
}