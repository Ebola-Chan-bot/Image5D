// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "哈希计算器.h"
#include<Windows.h>
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Image5D::哈希初始化();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		//C++语法要求必须有一条break
		break;
	}
	return TRUE;
}

