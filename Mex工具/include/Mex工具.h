#pragma once
#include <mex.hpp>
#include <mexAdapter.hpp>
#include <algorithm>
using namespace matlab::data;
using namespace matlab::mex;
extern ArrayFactory 数组工厂;
template <typename T>
inline T Get标量(Array& 输入数组)
{
	return TypedArray<T>(std::move(输入数组))[0];
}
template <typename T>
requires std::is_arithmetic<T>::value
T Get数值(Array& 输入数组)
{
	switch (输入数组.getType())
	{
	case ArrayType::UINT8:
		return T(Get标量<UINT8>(输入数组));
	case ArrayType::UINT16:
		return T(Get标量<UINT16>(输入数组));
	case ArrayType::UINT32:
		return T(Get标量<UINT32>(输入数组));
	case ArrayType::UINT64:
		return T(Get标量<UINT64>(输入数组));
	case ArrayType::INT8:
		return T(Get标量<INT8>(输入数组));
	case ArrayType::INT16:
		return T(Get标量<INT16>(输入数组));
	case ArrayType::INT32:
		return T(Get标量<INT32>(输入数组));
	case ArrayType::INT64:
		return T(Get标量<INT64>(输入数组));
	case ArrayType::SINGLE:
		return T(Get标量<float>(输入数组));
	case ArrayType::DOUBLE:
		return T(Get标量<double>(输入数组));
	default:
		throw InvalidArrayTypeException("不是数值类型");
	}
}
template <typename T>
inline T* Get对象指针(Array& 输入数组)
{
	return (T*)Get标量<UINT64>(输入数组);
}
template <typename T>
inline buffer_ptr_t<T> GetBuffer(const T* 指针, UINT64 尺寸)noexcept
{
	buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>(尺寸);
	std::copy_n(指针, 尺寸, 缓冲.get());
	return 缓冲;
}
String GetString(Array& 输入数组);
//调用方负责释放
char* GetUtf8(Array& 输入数组);
StringArray GetStringVector(uint8_t 个数, const char* const* 数组)noexcept;
CharArray GetCharArray(const char* 字符串)noexcept;
#define API声明(函数名) void 函数名(ArgumentList& outputs,ArgumentList& inputs)
#define API索引 constexpr void (*(API[]))(ArgumentList&, ArgumentList&) =
#define API调用 API[Get数值<UINT8>(inputs[0])](outputs, inputs);
class MexFunction :public Function
{
public:
	void operator()(ArgumentList outputs, ArgumentList inputs);
};