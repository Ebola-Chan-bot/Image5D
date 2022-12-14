#pragma once
#include<stdint.h>
namespace Image5D
{
	//必须在模块加载时调用唯一一次的初始化函数
	void 哈希初始化()noexcept;
	constexpr uint8_t 哈希长度 = 32;
	void 哈希计算(const void* 输入字节, uint32_t 输入长度, void* 输出字节)noexcept;
}