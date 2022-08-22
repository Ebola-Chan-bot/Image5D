#include "pch.h"
#include "Image5D异常.h"
Image5D异常::operator StructArray()const
{
		StructArray 返回 = 数组工厂.createStructArray({ 1 }, { "ExceptionType","InnerException","ErrorCode" });
		返回[0]["ExceptionType"] = 数组工厂.createScalar<uint8_t>(异常类型);
		返回[0]["InnerException"] = 数组工厂.createScalar<uint8_t>(内部异常);
		返回[0]["ErrorCode"] = 数组工厂.createScalar(错误代码);
		return 返回;
}
const StructArray 成功结构 = 成功异常;