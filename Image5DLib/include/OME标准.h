#pragma once
#include<stdint.h>
#include <memory>
namespace Image5D
{
	enum class 像素类型
	{
		UINT8,
		UINT16,
		UINT32,
		//请注意，ImageJ不支持有符号整数型的TIFF文件；但它仍能正确读取，只是会弹出警告。
		INT8,
		INT16,
		INT32,
		FLOAT,
		DOUBLE,
		缺省
	};
	template<typename T>
	constexpr 像素类型 数据类型转像素类型 = 像素类型::缺省;
	template<>
	constexpr 像素类型 数据类型转像素类型<uint8_t> = 像素类型::UINT8;
	template<>
	constexpr 像素类型 数据类型转像素类型<uint16_t> = 像素类型::UINT16;
	template<>
	constexpr 像素类型 数据类型转像素类型<uint32_t> = 像素类型::UINT32;
	template<>
	constexpr 像素类型 数据类型转像素类型<int8_t> = 像素类型::INT8;
	template<>
	constexpr 像素类型 数据类型转像素类型<int16_t> = 像素类型::INT16;
	template<>
	constexpr 像素类型 数据类型转像素类型<int32_t> = 像素类型::INT32;
	template<>
	constexpr 像素类型 数据类型转像素类型<float> = 像素类型::FLOAT;
	template<>
	constexpr 像素类型 数据类型转像素类型<double> = 像素类型::DOUBLE;
	template<像素类型 T>
	struct 像素类型转数据类型_s;
	template<>
	struct 像素类型转数据类型_s<像素类型::UINT8>
	{
		using 数据类型 = uint8_t;
	};
	template<>
	struct 像素类型转数据类型_s<像素类型::UINT16>
	{
		using 数据类型 = uint16_t;
	};
	template<>
	struct 像素类型转数据类型_s<像素类型::UINT32>
	{
		using 数据类型 = uint32_t;
	};
	template<>
	struct 像素类型转数据类型_s<像素类型::INT8>
	{
		using 数据类型 = int8_t;
	};
	template<>
	struct 像素类型转数据类型_s<像素类型::INT16>
	{
		using 数据类型 = int16_t;
	};
	template<>
	struct 像素类型转数据类型_s<像素类型::INT32>
	{
		using 数据类型 = int32_t;
	};
	template<>
	struct 像素类型转数据类型_s<像素类型::FLOAT>
	{
		using 数据类型 = float;
	};
	template<>
	struct 像素类型转数据类型_s<像素类型::DOUBLE>
	{
		using 数据类型 = double;
	};
	template<像素类型 T>
	using 像素类型转数据类型 = 像素类型转数据类型_s<T>;
	constexpr uint8_t 像素类型尺寸[] = { 1,2,4,1,2,4,4,8 };
	union 颜色
	{
		//OME标准是用Java实现，不支持无符号整数，就很坑
		int32_t 整数值;
#pragma pack(push,1)
		struct
		{
			uint8_t A;
			uint8_t B;
			uint8_t G;
			uint8_t R;
		}RGBA;
#pragma pack(pop)
	};
	namespace 常用颜色
	{
		constexpr 颜色 红色{ .RGBA{.A = 0,.B = 0,.G = 0,.R = 255} };
		constexpr 颜色 绿色{ .RGBA{.A = 0,.B = 0,.G = 255,.R = 0} };
		constexpr 颜色 蓝色{ .RGBA{.A = 0,.B = 255,.G = 0,.R = 0} };
		constexpr 颜色 青色{ .RGBA{.A = 0,.B = 255,.G = 255,.R = 0} };
		constexpr 颜色 紫红{ .RGBA{.A = 0,.B = 255,.G = 0,.R = 255} };
		constexpr 颜色 黄色{ .RGBA{.A = 0,.B = 0,.G = 255,.R = 255} };
		constexpr 颜色 白色{ .RGBA{.A = 0,.B = 255,.G = 255,.R = 255} };
	}
	enum 维度顺序
	{
		XYCZT,
		XYCTZ,
		XYZCT,
		XYZTC,
		XYTCZ,
		XYTZC,
		缺省
	};
	using 颜色数组 = std::unique_ptr<颜色[]>;
}