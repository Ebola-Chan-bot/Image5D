// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "模块句柄.h"
HMODULE 模块句柄;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    模块句柄 = hModule;
    return TRUE;
}

