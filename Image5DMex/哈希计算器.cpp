#include "pch.h"
#include "哈希计算器.h"
#include <bcrypt.h>
#include <memory>
BCRYPT_ALG_HANDLE 算法提供者句柄;
BCRYPT_HASH_HANDLE 哈希句柄;
std::unique_ptr<UCHAR[]> 哈希对象;
void 哈希初始化()noexcept
{
	BCryptOpenAlgorithmProvider(&算法提供者句柄, BCRYPT_SHA256_ALGORITHM, NULL, BCRYPT_HASH_REUSABLE_FLAG);
	DWORD 对象长度;
	ULONG 属性长度;
	BCryptGetProperty(算法提供者句柄, BCRYPT_OBJECT_LENGTH, (PUCHAR)&对象长度, sizeof(对象长度), &属性长度, 0);
	哈希对象 = std::make_unique_for_overwrite<UCHAR[]>(对象长度);
	BCryptCreateHash(算法提供者句柄, &哈希句柄, 哈希对象.get(), 对象长度, NULL, 0, BCRYPT_HASH_REUSABLE_FLAG);
}
void 哈希输入(ULONG 字节数, PUCHAR 数据)noexcept { BCryptHashData(哈希句柄, 数据, 字节数, 0); }
void 哈希输出(ULONG 缓冲区大小, PUCHAR 哈希值)noexcept { BCryptFinishHash(哈希句柄, 哈希值, 缓冲区大小, 0); }
void 哈希释放()noexcept
{
	BCryptDestroyHash(哈希句柄);
	BCryptCloseAlgorithmProvider(算法提供者句柄, 0);
}