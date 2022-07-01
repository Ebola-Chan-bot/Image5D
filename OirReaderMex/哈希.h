#pragma once
#include <bcrypt.h>
class 哈希
{
	BCRYPT_ALG_HANDLE 算法提供者句柄;
	BCRYPT_HASH_HANDLE 哈希句柄;
	PUCHAR 哈希对象;
public:
	哈希()noexcept;
	void 输入(ULONG 字节数, PUCHAR 数据)const noexcept{ BCryptHashData(哈希句柄, 数据, 字节数, 0); }
	void 输出(ULONG 缓冲区大小, PUCHAR 哈希值)const noexcept{ BCryptFinishHash(哈希句柄, 哈希值, 缓冲区大小, 0); }
	~哈希()noexcept;
};

