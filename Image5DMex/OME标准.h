#pragma once
#include<stdint.h>
#include <memory>
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