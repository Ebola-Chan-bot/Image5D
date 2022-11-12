#include "pch.h"
#include "哈希计算器.h"
#include <bcrypt.h>
BCRYPT_ALG_HANDLE 算法提供者句柄;
void 哈希初始化()noexcept
{
	BCryptOpenAlgorithmProvider(&算法提供者句柄, BCRYPT_SHA256_ALGORITHM, NULL, BCRYPT_HASH_REUSABLE_FLAG);
}
void 哈希计算(const void* 输入字节, uint32_t 输入长度, void* 输出字节)noexcept
{//哈希句柄只能一次性使用
	BCRYPT_HASH_HANDLE 哈希句柄;
	BCryptCreateHash(算法提供者句柄, &哈希句柄, NULL, 0, NULL, 0, BCRYPT_HASH_REUSABLE_FLAG);
	BCryptHashData(哈希句柄, (PUCHAR)输入字节, 输入长度, 0);
	BCryptFinishHash(哈希句柄, (PUCHAR)输出字节, 32, 0);
	BCryptDestroyHash(哈希句柄);
}