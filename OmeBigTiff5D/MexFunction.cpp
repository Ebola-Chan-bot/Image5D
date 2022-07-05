#include "pch.h"
#include "IOmeTiff读写器.h"
#include "异常转换.h"
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
	switch (inputs.size())
	{
	case 3:
	{
		const 窄字符数组 图像描述 = 万能转码<char[]>(inputs[2]);
		outputs[0] = 万能转码(IOmeTiff读写器::覆盖创建((wchar_t*)字符串.c_str(), 图像描述.get()));
	}
	break;
	case 9:
	{
		TypedArray<uint32_t>颜色数组(std::move(inputs[5]));
		const uint8_t SizeC = 颜色数组.getNumberOfElements();
		const buffer_ptr_t<uint32_t> 颜色缓冲 = 颜色数组.release();
		outputs[0] = 万能转码(IOmeTiff读写器::覆盖创建((wchar_t*)字符串.c_str(), (像素类型)万能转码<UINT8>(inputs[2]), 万能转码<UINT16>(inputs[3]), 万能转码<UINT16>(inputs[4]), SizeC, 万能转码<UINT8>(inputs[6]), 万能转码<UINT16>(inputs[7]), (颜色*)颜色缓冲.get(), (维度顺序)万能转码<UINT8>(inputs[8])));
	}
	break;
	default:
		throw FeatureNotSupportedException("输入参数个数错误，不支持此重载");
	}
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
		TypedArray<uint32_t> 颜色数组(std::move(inputs[2]));
		if (对象指针->SizeC() > 颜色数组.getNumberOfElements())
			throw Image5D异常(颜色数与SizeC不匹配);
		const buffer_ptr_t<uint32_t> 颜色缓冲 = 颜色数组.release();
		对象指针->通道颜色((颜色*)颜色缓冲.get());
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
	IOmeTiff读写器* 对象指针 = 万能转码<IOmeTiff读写器*>(inputs[1]);
	TypedArray<uint32_t>颜色数组(std::move(inputs[8]));
	const bool 指定值 = 颜色数组.getNumberOfElements();
	const buffer_ptr_t<uint32_t>颜色缓冲 = 颜色数组.release();
	const 颜色* const 颜色指针 = 指定值 ? (颜色*)颜色缓冲.get() : nullptr;
	const 窄字符数组 文件名 = 万能转码<char[]>(inputs[10]);
	const char* 文件名指针 = 文件名.get();
	if (!*文件名指针)
		文件名指针 = nullptr;
	对象指针->修改参数((像素类型)万能转码<UINT8>(inputs[2]), 万能转码<UINT16>(inputs[3]), 万能转码<UINT16>(inputs[4]), 万能转码<UINT8>(inputs[5]), 万能转码<UINT8>(inputs[6]), 万能转码<UINT16>(inputs[7]), 颜色指针, (维度顺序)万能转码<UINT8>(inputs[9]), 文件名指针);
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
inline void ReadPixels(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	switch (inputs.size())
	{
	case 2:
	{
		buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>((UINT64)对象指针->SizeI() * 对象指针->SizeX() * 对象指针->SizeY());
		对象指针->读入像素((char*)缓冲.get());
		outputs[0] = 数组工厂.createArrayFromBuffer({ 对象指针->SizeX(),对象指针->SizeY(),对象指针->SizeI() }, std::move(缓冲));
	}
	break;
	case 4:
	{
		const UINT16 TSize = 万能转码<UINT16>(inputs[3]);
		buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>((UINT64)TSize * 对象指针->SizeZ() * 对象指针->SizeC() * 对象指针->SizeX() * 对象指针->SizeY());
		对象指针->读入像素((char*)缓冲.get(), 万能转码<UINT16>(inputs[2]), TSize);
		outputs[0] = 数组工厂.createArrayFromBuffer(CZT重排(对象指针->DimensionOrder(), 对象指针->SizeX(), 对象指针->SizeY(), 对象指针->SizeC(), 对象指针->SizeZ(), TSize), std::move(缓冲));
	}
	break;
	case 8:
	{
		const UINT16 TSize = 万能转码<UINT16>(inputs[3]);
		const UINT8 ZSize = 万能转码<UINT8>(inputs[5]);
		const UINT8 CSize = 万能转码<UINT8>(inputs[7]);
		buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>((UINT64)TSize * ZSize * CSize * 对象指针->SizeX() * 对象指针->SizeY());
		对象指针->读入像素((char*)缓冲.get(), 万能转码<UINT16>(inputs[2]), TSize, 万能转码<UINT8>(inputs[4]), ZSize, 万能转码<UINT8>(inputs[6]), CSize);
		outputs[0] = 数组工厂.createArrayFromBuffer(CZT重排(对象指针->DimensionOrder(), 对象指针->SizeX(), 对象指针->SizeY(), CSize, ZSize, TSize), std::move(缓冲));
	}
	break;
	default:
		throw FeatureNotSupportedException("输入参数个数错误，不支持此重载");
	}
}
类型化读写(ReadPixels);
template<数值 T>
inline void ReadPixelsI(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const UINT32 ISize = 万能转码<UINT32>(inputs[3]);
	buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>((UINT64)ISize * 对象指针->SizeX() * 对象指针->SizeY());
	对象指针->读入像素((char*)缓冲.get(), 万能转码<UINT32>(inputs[2]), ISize);
	outputs[0] = 数组工厂.createArrayFromBuffer({ 对象指针->SizeX(),对象指针->SizeY(),ISize }, std::move(缓冲));
}
类型化读写(ReadPixelsI);
template<数值 T>
inline void WritePixels(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	switch (inputs.size())
	{
	case 3:
	{
		TypedArray<T>写出数组(std::move(inputs[2]));
		if (写出数组.getNumberOfElements() < (size_t)对象指针->SizeT() * 对象指针->SizeZ() * 对象指针->SizeC() * 对象指针->SizeX() * 对象指针->SizeY())
			throw InvalidNumberOfElementsProvidedException("数组元素太少，不够写出");
		const buffer_ptr_t<T>写出缓冲 = 写出数组.release();
		对象指针->写出像素((char*)写出缓冲.get());
		outputs[0] = 成功结构;
	}
	break;
	case 5:
	{
		TypedArray<T>写出数组(std::move(inputs[2]));
		const uint16_t TSize = 万能转码<UINT16>(inputs[4]);
		if (写出数组.getNumberOfElements() < (size_t)TSize * 对象指针->SizeZ() * 对象指针->SizeC() * 对象指针->SizeX() * 对象指针->SizeY())
			throw InvalidNumberOfElementsProvidedException("数组元素太少，不够写出");
		const buffer_ptr_t<T>写出缓冲 = 写出数组.release();
		对象指针->写出像素((char*)写出缓冲.get(), 万能转码<UINT16>(inputs[3]), TSize);
		outputs[0] = 成功结构;
	}
	break;
	case 9:
	{
		TypedArray<T>写出数组(std::move(inputs[2]));
		const uint16_t TSize = 万能转码<UINT16>(inputs[4]);
		const uint8_t ZSize = 万能转码<UINT8>(inputs[6]);
		const uint8_t CSize = 万能转码<UINT8>(inputs[8]);
		if (写出数组.getNumberOfElements() < (size_t)TSize * ZSize * CSize * 对象指针->SizeX() * 对象指针->SizeY())
			throw InvalidNumberOfElementsProvidedException("数组元素太少，不够写出");
		const buffer_ptr_t<T>写出缓冲 = 写出数组.release();
		对象指针->写出像素((char*)写出缓冲.get(), 万能转码<UINT16>(inputs[3]), TSize, 万能转码<UINT8>(inputs[5]), ZSize, 万能转码<UINT8>(inputs[7]), CSize);
		outputs[0] = 成功结构;
	}
	break;
	default:
		throw FeatureNotSupportedException("输入参数个数错误，不支持此重载");
	}
}
类型化读写(WritePixels);
template<数值 T>
inline void WritePixelsI(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	TypedArray<T>写出数组(std::move(inputs[2]));
	const uint32_t ISize = 万能转码<UINT32>(inputs[4]);
	if (写出数组.getNumberOfElements() < (size_t)ISize * 对象指针->SizeX() * 对象指针->SizeY())
		throw InvalidNumberOfElementsProvidedException("数组元素太少，不够写出");
	const buffer_ptr_t<T>写出缓冲 = 写出数组.release();
	对象指针->写出像素((char*)写出缓冲.get(), 万能转码<UINT32>(inputs[3]), ISize);
	outputs[0] = 成功结构;
}
类型化读写(WritePixelsI);
API声明(PixelPointer)
{
	const IOmeTiff读写器* 对象指针 = 万能转码<const IOmeTiff读写器*>(inputs[1]);
	char* 像素指针;
	size_t 容量;
	switch (inputs.size())
	{
	case 2:
		对象指针->像素指针(像素指针, 容量);
		break;
	case 3:
		对象指针->像素指针(万能转码<UINT16>(inputs[2]), 像素指针, 容量);
		break;
	case 5:
		对象指针->像素指针(万能转码<UINT16>(inputs[2]), 万能转码<UINT8>(inputs[3]), 万能转码<UINT8>(inputs[4]), 像素指针, 容量);
		break;
	default:
		throw FeatureNotSupportedException("输入参数个数错误，不支持此重载");
	}
	outputs[0] = 万能转码(像素指针);
	if (outputs.size() > 1)
		outputs[1] = 万能转码(容量);
}
API声明(PixelPointerI)
{
	const IOmeTiff读写器* 对象指针 = 万能转码<const IOmeTiff读写器*>(inputs[1]);
	char* 像素指针;
	size_t 容量;
	对象指针->像素指针(万能转码<UINT32>(inputs[2]), 像素指针, 容量);
	outputs[0] = 万能转码(像素指针);
	if (outputs.size() > 1)
		outputs[1] = 万能转码(容量);
}
API声明(ReadToPointer)
{
	const IOmeTiff读写器* const 对象指针 = 万能转码<IOmeTiff读写器*>(inputs[1]);
	char* const 输出指针 = (char*)万能转码<void*>(inputs[2]);
	const size_t 输出容量 = 万能转码<size_t>(inputs[3]);
	const size_t SizePXY = 对象指针->SizeP() * 对象指针->SizeX() * 对象指针->SizeY();
	switch (inputs.size())
	{
	case 4:
		if (SizePXY * 对象指针->SizeT() * 对象指针->SizeZ() * 对象指针->SizeC() > 输出容量)
			throw NumberOfElementsExceedsMaximumException("目标内存溢出");
		对象指针->读入像素(输出指针);
		outputs[0] = 成功结构;
		break;
	case 6:
	{
		const uint16_t TSize = 万能转码<uint16_t>(inputs[5]);
		if (SizePXY * TSize * 对象指针->SizeZ() * 对象指针->SizeC() > 输出容量)
			throw NumberOfElementsExceedsMaximumException("目标内存溢出");
		对象指针->读入像素(输出指针, 万能转码<uint16_t>(inputs[4]), TSize);
		outputs[0] = 成功结构;
	}
	break;
	case 10:
	{
		const uint16_t TSize = 万能转码<uint16_t>(inputs[5]);
		const uint8_t ZSize = 万能转码<uint8_t>(inputs[7]);
		const uint8_t CSize = 万能转码<uint8_t>(inputs[9]);
		if (SizePXY * TSize * ZSize * CSize > 输出容量)
			throw NumberOfElementsExceedsMaximumException("目标内存溢出");
		对象指针->读入像素(输出指针, 万能转码<uint16_t>(inputs[4]), TSize, 万能转码<uint8_t>(inputs[6]), ZSize, 万能转码<uint8_t>(inputs[8]), CSize);
		outputs[0] = 成功结构;
	}
	break;
	default:
		throw FeatureNotSupportedException("输入参数个数错误，不支持此重载");
	}
}
API声明(ReadToPointerI)
{
	const IOmeTiff读写器* const 对象指针 = 万能转码<IOmeTiff读写器*>(inputs[1]);
	char* const 输出指针 = (char*)万能转码<void*>(inputs[2]);
	const size_t 输出容量 = 万能转码<size_t>(inputs[3]);
	const size_t SizePXY = 对象指针->SizeP() * 对象指针->SizeX() * 对象指针->SizeY();
	const uint32_t ISize = 万能转码<uint32_t>(inputs[5]);
	if (SizePXY * ISize > 输出容量)
		throw NumberOfElementsExceedsMaximumException("目标内存溢出");
	对象指针->读入像素(输出指针, 万能转码<uint32_t>(inputs[4]), ISize);
	outputs[0] = 成功结构;
}
API声明(WriteFromPointer)
{
	const IOmeTiff读写器* const 对象指针 = 万能转码<IOmeTiff读写器*>(inputs[1]);
	const char* const 输入指针 = (char*)万能转码<void*>(inputs[2]);
	const size_t 输入容量 = 万能转码<size_t>(inputs[3]);
	const size_t SizePXY = 对象指针->SizeP() * 对象指针->SizeX() * 对象指针->SizeY();
	switch (inputs.size())
	{
	case 4:
		if (SizePXY * 对象指针->SizeT() * 对象指针->SizeZ() * 对象指针->SizeC() > 输入容量)
			throw InvalidNumberOfElementsProvidedException("输入指针内存太小，不够写出");
		对象指针->写出像素(输入指针);
		outputs[0] = 成功结构;
		break;
	case 6:
	{
		const uint16_t TSize = 万能转码<uint16_t>(inputs[5]);
		if (SizePXY * TSize * 对象指针->SizeZ() * 对象指针->SizeC() > 输入容量)
			throw InvalidNumberOfElementsProvidedException("输入指针内存太小，不够写出");
		对象指针->写出像素(输入指针, 万能转码<uint16_t>(inputs[4]), TSize);
		outputs[0] = 成功结构;
	}
	break;
	case 10:
	{
		const uint16_t TSize = 万能转码<uint16_t>(inputs[5]);
		const uint8_t ZSize = 万能转码<uint8_t>(inputs[7]);
		const uint8_t CSize = 万能转码<uint8_t>(inputs[9]);
		if (SizePXY * TSize * ZSize * CSize > 输入容量)
			throw InvalidNumberOfElementsProvidedException("输入指针内存太小，不够写出");
		对象指针->写出像素(输入指针, 万能转码<uint16_t>(inputs[4]), TSize, 万能转码<uint8_t>(inputs[6]), ZSize, 万能转码<uint8_t>(inputs[8]), CSize);
		outputs[0] = 成功结构;
	}
	break;
	default:
		throw FeatureNotSupportedException("输入参数个数错误，不支持此重载");
	}
}
API声明(WriteFromPointerI)
{
	const IOmeTiff读写器* const 对象指针 = 万能转码<IOmeTiff读写器*>(inputs[1]);
	const char* const 输入指针 = (char*)万能转码<void*>(inputs[2]);
	const size_t 输入容量 = 万能转码<size_t>(inputs[3]);
	const size_t SizePXY = 对象指针->SizeP() * 对象指针->SizeX() * 对象指针->SizeY();
	const uint32_t ISize = 万能转码<uint32_t>(inputs[5]);
	if (SizePXY * ISize > 输入容量)
		throw InvalidNumberOfElementsProvidedException("输入指针内存太小，不够写出");
	对象指针->写出像素(输入指针, 万能转码<uint32_t>(inputs[4]), ISize);
	outputs[0] = 成功结构;
}
API声明(Close)
{
	delete 万能转码<IOmeTiff读写器*>(inputs[1]);
	outputs[0] = 成功结构;
}
void MexFunction::operator()(ArgumentList& outputs, ArgumentList& inputs)
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
		ReadPixels,
		ReadPixelsI,
		WritePixels,
		WritePixelsI,
		PixelPointer,
		PixelPointerI,
		ReadToPointer,
		ReadToPointerI,
		WriteFromPointer,
		WriteFromPointerI,
		Close
	};
	try
	{
		API调用;
	}
	catch (Image5D异常 异常)
	{
		outputs[0] = 异常转换(异常);
		异常输出补全(outputs);
	}
}