#include "pch.h"
#include "哈希.h"
#include <stdlib.h>
哈希::哈希()noexcept
{
	BCryptOpenAlgorithmProvider(&算法提供者句柄, BCRYPT_SHA256_ALGORITHM, NULL, BCRYPT_HASH_REUSABLE_FLAG);
	DWORD 对象长度;
	ULONG 属性长度;
	BCryptGetProperty(算法提供者句柄, BCRYPT_OBJECT_LENGTH, (PUCHAR)&对象长度, sizeof(对象长度), &属性长度, 0);
	BCryptCreateHash(算法提供者句柄, &哈希句柄, 哈希对象 = (PUCHAR)malloc(对象长度), 对象长度, NULL, 0, BCRYPT_HASH_REUSABLE_FLAG);
}
哈希::~哈希()noexcept
{
	BCryptDestroyHash(哈希句柄);
	free(哈希对象);
	BCryptCloseAlgorithmProvider(算法提供者句柄, 0);
}