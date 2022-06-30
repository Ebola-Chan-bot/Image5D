#include "pch.h"
#include "IOmeTiff读写器.h"
#include "异常转换.h"
const StructArray 成功结构 = 异常转换(成功);
//只读
API声明(OpenRead)
{
	const String 字符串 = 万能转码<String>(inputs[1]);
	outputs[0] = 万能转码(IOmeTiff读写器::只读打开((wchar_t*)字符串.c_str()));
}
//读写
API声明(OpenRW)
{
	const String 字符串= 万能转码<String>(inputs[1]);
	outputs[0] = 万能转码(IOmeTiff读写器::读写打开((wchar_t*)字符串.c_str()));
}
//只写
API声明(Create)
{
	const String 字符串 = 万能转码<String>(inputs[1]);
	const buffer_ptr_t<uint32_t> 颜色数组 = TypedArray<UINT32>(std::move(inputs[8])).release();
	outputs[0] = 万能转码(IOmeTiff读写器::覆盖创建((wchar_t*)字符串.c_str(), (像素类型)万能转码<UINT8>(inputs[2]), 万能转码<UINT16>(inputs[3]), 万能转码<UINT16>(inputs[4]), 万能转码<UINT8>(inputs[5]), 万能转码<UINT8>(inputs[6]), 万能转码<UINT16>(inputs[7]), (颜色*)颜色数组.get(), (维度顺序)万能转码<UINT8>(inputs[9])));
}
API声明(PixelType)
{
	if (inputs.size() > 2)
	{
		万能转码<IOmeTiff读写器*>(inputs[1])->PixelType((像素类型)万能转码<UINT8>(inputs[2]));
		outputs[0] = 成功结构;
	}
	else
		outputs[0] = 万能转码((UINT8)万能转码<const IOmeTiff读写器*>(inputs[1])->PixelType());
}
API声明(DimensionOrder)
{
	if (inputs.size() > 2)
	{
		万能转码<IOmeTiff读写器*>(inputs[1])->DimensionOrder((维度顺序)万能转码<UINT8>(inputs[2]));
		outputs[0] = 成功结构;
	}
	else
		outputs[0] = 万能转码((UINT8)万能转码<const IOmeTiff读写器*>(inputs[1])->DimensionOrder());
}
API声明(SizeP)
{
	outputs[0] = 万能转码(万能转码<const IOmeTiff读写器*>(inputs[1])->SizeP());
}
#define 尺寸属性(类型,字段) API声明(字段)\
{\
	if (inputs.size() > 2)\
	{\
		万能转码<IOmeTiff读写器*>(inputs[1])->字段(万能转码<类型>(inputs[2]));\
		outputs[0] = 成功结构;\
	}\
	else\
		outputs[0] = 万能转码(万能转码<const IOmeTiff读写器*>(inputs[1])->字段());\
}
尺寸属性(UINT16, SizeX);
尺寸属性(UINT16, SizeY);
尺寸属性(UINT32, SizeI);
尺寸属性(UINT8, SizeC);
尺寸属性(UINT8, SizeZ);
尺寸属性(UINT16, SizeT);
API声明(ChannelColors)
{
	if (inputs.size() > 2)
	{
		IOmeTiff读写器* 对象指针 = 万能转码<IOmeTiff读写器*>(inputs[1]);
		const TypedArray<uint32_t> 颜色数组(std::move(inputs[2]));
		if (对象指针->SizeC() > 颜色数组.getNumberOfElements())
			throw Image5D异常(SizeC与颜色数不匹配);
		const buffer_ptr_t<uint32_t> 颜色数组 = TypedArray<UINT32>(.release();
		万能转码<IOmeTiff读写器*>(inputs[1])->通道颜色((颜色*)颜色数组.get());
		outputs[0] = 成功结构;
	}
	else
	{
		const IOmeTiff读写器* 对象指针 = 万能转码<const IOmeTiff读写器*>(inputs[1]);
		const UINT8 SizeC = 对象指针->SizeC();
		outputs[0] = 数组工厂.createArrayFromBuffer({ SizeC,1 }, 万能转码((UINT32*)对象指针->通道颜色(), SizeC));
	}
}
API声明(FileName)
{
	if (inputs.size() > 2)
	{
		窄字符数组 字符串 = 万能转码<char[]>(inputs[2]);
		万能转码<IOmeTiff读写器*>(inputs[1])->文件名(字符串.get());
		outputs[0] = 成功结构;
	}
	else
		outputs[0] = 万能转码<CharArray>(万能转码<const IOmeTiff读写器*>(inputs[1])->文件名());
}
API声明(ImageDescription)
{
	if (inputs.size() > 2)
	{
		const 窄字符数组 图像描述 = 万能转码<char[]>(inputs[2]);
		万能转码<IOmeTiff读写器*>(inputs[1])->图像描述(图像描述.get());
		outputs[0] = 成功结构;
	}
	else
		outputs[0] = 万能转码<CharArray>(万能转码<const IOmeTiff读写器*>(inputs[1])->图像描述());
}
API声明(ModifyParameters)
{
	const 窄字符数组 文件名 = 万能转码<char[]>(inputs[9]);
	const buffer_ptr_t<uint32_t> 颜色数组 = TypedArray<UINT32>(std::move(inputs[8])).release();
	万能转码<IOmeTiff读写器*>(inputs[1])->修改参数((像素类型)万能转码<UINT8>(inputs[2]), 万能转码<UINT16>(inputs[3]), 万能转码<UINT16>(inputs[4]), 万能转码<UINT8>(inputs[5]), 万能转码<UINT8>(inputs[6]), 万能转码<UINT16>(inputs[7]), (颜色*)颜色数组.get(), (维度顺序)万能转码<UINT8>(inputs[8]), 文件名.get());
	outputs[0] = 成功结构;
}
#define 类型化读写(API名) API声明(API名)\
{\
	const IOmeTiff读写器* 对象指针 = 万能转码<const IOmeTiff读写器*>(inputs[1]);\
	switch (对象指针->PixelType())\
	{\
	case 像素类型::UINT8:\
		API名<UINT8>(对象指针, outputs, inputs);\
		break;\
	case 像素类型::UINT16:\
		API名<UINT16>(对象指针, outputs, inputs);\
		break;\
	case 像素类型::UINT32:\
		API名<UINT32>(对象指针, outputs, inputs);\
		break;\
	case 像素类型::INT8:\
		API名<INT8>(对象指针, outputs, inputs);\
		break;\
	case 像素类型::INT16:\
		API名<INT16>(对象指针, outputs, inputs);\
		break;\
	case 像素类型::INT32:\
		API名<INT32>(对象指针, outputs, inputs);\
		break;\
	case 像素类型::FLOAT:\
		API名<FLOAT>(对象指针, outputs, inputs);\
		break;\
	case 像素类型::DOUBLE:\
		API名<double>(对象指针, outputs, inputs);\
		break;\
	}\
}
template<数值 T>
inline void ReadPixelsI(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const UINT16 SizeX = 对象指针->SizeX();
	const UINT16 SizeY = 对象指针->SizeY();
	const UINT32 ISize = 万能转码<UINT32>(inputs[3]);
	buffer_ptr_t<uint16_t>缓冲 = 数组工厂.createBuffer<uint16_t>((UINT64)ISize * SizeX * SizeY);
	对象指针->读入像素((char*)缓冲.get(), 万能转码<UINT32>(inputs[2]), ISize);
	outputs[0] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,ISize }, std::move(缓冲));
}
类型化读写(ReadPixelsI);
ArrayDimensions CZT重排(维度顺序 DO, uint16_t X, uint16_t Y, uint8_t C, uint8_t Z, uint16_t T)
{
	switch (DO)
	{
	case XYCZT:
		return { X,Y,C,Z,T };
	case XYCTZ:
		return { X,Y,C,T,Z };
	case XYZCT:
		return { X,Y,Z,C,T };
	case XYZTC:
		return { X,Y,Z,T,C };
	case XYTCZ:
		return { X,Y,T,C,Z };
	case XYTZC:
		return { X,Y,T,Z,C };
	}
}
template<数值 T>
inline void ReadPixelsTZC(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const UINT16 TSize = 万能转码<UINT16>(inputs[3]);
	const UINT8 ZSize = 万能转码<UINT8>(inputs[5]);
	const UINT8 CSize = 万能转码<UINT8>(inputs[7]);
	const UINT16 SizeX = 对象指针->SizeX();
	const UINT16 SizeY = 对象指针->SizeY();
	buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>((UINT64)TSize * ZSize * CSize * SizeX * SizeY);
	对象指针->读入像素((char*)缓冲.get(), 万能转码<UINT16>(inputs[2]), TSize, 万能转码<UINT8>(inputs[4]), ZSize, 万能转码<UINT8>(inputs[6]), CSize);
	outputs[0] = 数组工厂.createArrayFromBuffer(CZT重排(对象指针->DimensionOrder(), SizeX, SizeY, CSize, ZSize, TSize), std::move(缓冲));
}
类型化读写(ReadPixelsTZC);
template<数值 T>
inline void ReadPixelsT(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const UINT16 TSize = 万能转码<UINT16>(inputs[3]);
	const UINT16 SizeX = 对象指针->SizeX();
	const UINT16 SizeY = 对象指针->SizeY();
	const UINT8 SizeC = 对象指针->SizeC();
	const UINT8 SizeZ = 对象指针->SizeZ();
	buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>((UINT64)TSize * SizeZ * SizeC * SizeX * SizeY);
	对象指针->读入像素((char*)缓冲.get(), 万能转码<UINT16>(inputs[2]), TSize);
	outputs[0] = 数组工厂.createArrayFromBuffer(CZT重排(对象指针->DimensionOrder(), SizeX, SizeY, SizeC, SizeZ, TSize), std::move(缓冲));
}
类型化读写(ReadPixelsT);
template<数值 T>
inline void WritePixelsI(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const buffer_ptr_t<T>缓冲 = TypedArray<T>(std::move(inputs[2])).release();
	对象指针->写出像素((char*)缓冲.get(), 万能转码<UINT32>(inputs[3]), 万能转码<UINT32>(inputs[4]));
	outputs[0] = 成功结构;
}
类型化读写(WritePixelsI);
template<数值 T>
inline void WritePixelsTZC(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const buffer_ptr_t<T>缓冲 = TypedArray<T>(std::move(inputs[2])).release();
	对象指针->写出像素((char*)缓冲.get(), 万能转码<UINT16>(inputs[3]), 万能转码<UINT16>(inputs[4]), 万能转码<UINT8>(inputs[5]), 万能转码<UINT8>(inputs[6]), 万能转码<UINT8>(inputs[7]), 万能转码<UINT8>(inputs[8]));
	outputs[0] = 成功结构;
}
类型化读写(WritePixelsTZC);
template<数值 T>
inline void WritePixelsT(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const buffer_ptr_t<T>缓冲 = TypedArray<T>(std::move(inputs[2])).release();
	对象指针->写出像素((char*)缓冲.get(), 万能转码<UINT16>(inputs[3]), 万能转码<UINT16>(inputs[4]));
	outputs[0] = 成功结构;
}
类型化读写(WritePixelsT);
API声明(PixelPointer)
{
	const IOmeTiff读写器* 对象指针 = 万能转码<const IOmeTiff读写器*>(inputs[1]);
	if (inputs.size() < 5)
		outputs[0] = 万能转码(对象指针->像素指针(万能转码<UINT32>(inputs[2])));
	else
		outputs[0] = 万能转码(对象指针->像素指针(万能转码<UINT16>(inputs[2]), 万能转码<UINT8>(inputs[3]), 万能转码<UINT8>(inputs[4])));
}
API声明(Close)
{
	delete 万能转码<IOmeTiff读写器*>(inputs[1]);
	outputs[0] = 成功结构;
}
void MexFunction::operator()(ArgumentList outputs, ArgumentList inputs)
{
	API索引
	{
		OpenRead,
		OpenRW,
		Create,
		PixelType,
		DimensionOrder,
		SizeP,
		SizeX,
		SizeY,
		SizeI,
		SizeC,
		SizeZ,
		SizeT,
		ChannelColors,
		FileName,
		ImageDescription,
		ModifyParameters,
		ReadPixelsI,
		ReadPixelsTZC,
		ReadPixelsT,
		WritePixelsI,
		WritePixelsTZC,
		WritePixelsT,
		PixelPointer,
		Close
	};
	try
	{
		API调用;
	}
	catch (Image5D异常 异常)
	{
		outputs[0] = 异常转换(异常);
	}
}