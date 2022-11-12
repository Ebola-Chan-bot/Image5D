#pragma once
#include<stdint.h>
void 哈希初始化()noexcept;
constexpr UINT8 哈希长度 = 32;
void 哈希计算(const void* 输入字节, uint32_t 输入长度, void* 输出字节)noexcept;