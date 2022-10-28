#pragma once
void 哈希初始化()noexcept;
void 哈希输入(ULONG 字节数, PUCHAR 数据)noexcept;
void 哈希输出(ULONG 缓冲区大小, PUCHAR 哈希值)noexcept;
void 哈希释放()noexcept;