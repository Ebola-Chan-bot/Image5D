#include "pch.h"
#include "文件控制块.h"
const LONGLONG 分配粒度 = ([]()
	{
		SYSTEM_INFO 系统信息;
GetSystemInfo(&系统信息);
return 系统信息.dwAllocationGranularity;
	})();