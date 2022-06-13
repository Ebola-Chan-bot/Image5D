#include "pch.h"
#include "异常转换.h"
StructArray 异常转换(const Image5D异常& 异常)
{
	StructArray 异常结构 = 数组工厂.createStructArray({ 1,1 }, { "Type","Code" });
	//异常结构[0]不能单独拎出来做多次字段编辑
	异常结构[0]["Type"] = 数组工厂.createScalar(UINT8(异常.类型));
	异常结构[0]["Code"] = 数组工厂.createScalar(UINT8(异常.Win32错误代码));
	return 异常结构;
}