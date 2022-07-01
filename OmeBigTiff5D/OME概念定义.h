#pragma once
#include <Image5D.h>
#include "内存优化库.h"
enum class 像素类型 :UINT8
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
union 颜色
{
	//OME标准是用Java实现，不支持无符号整数，就很坑
	INT32 整数值;
#pragma pack(push,1)
	struct
	{
		UINT8 A;
		UINT8 B;
		UINT8 G;
		UINT8 R;
	}RGBA;
#pragma pack(pop)
};
enum 维度顺序 :UINT8
{
	XYCZT,
	XYCTZ,
	XYZCT,
	XYZTC,
	XYTCZ,
	XYTZC,
	缺省
};
//此类只能使用malloc分配的指针，不可使用new分配指针
using 颜色数组 = std::unique_ptr<颜色[], free删除器>;