#include "pch.h"
#include "IOmeTiff读写器.h"
#include "异常转换.h"
//只读
API声明(OpenRead)
{
	String 字符串 = GetString(inputs[1]);
	outputs[0] = 数组工厂.createScalar((UINT64)IOmeTiff读写器::只读打开((LPCWSTR)字符串.c_str()));
}
//读写
API声明(OpenRW)
{
	String 字符串 = GetString(inputs[1]);
	outputs[0] = 数组工厂.createScalar((UINT64)IOmeTiff读写器::读写打开((LPCWSTR)字符串.c_str()));
}
//只写
API声明(Create)
{
	const String 字符串 = GetString(inputs[1]);
	颜色* const 通道颜色 = (颜色*)TypedArray<UINT32>(std::move(inputs[8])).release().release();
	try
	{
		outputs[0] = 数组工厂.createScalar((UINT64)IOmeTiff读写器::覆盖创建((LPCWSTR)字符串.c_str(), (像素类型)Get数值<UINT8>(inputs[2]), Get数值<UINT16>(inputs[3]), Get数值<UINT16>(inputs[4]), Get数值<UINT8>(inputs[5]), Get数值<UINT8>(inputs[6]), Get数值<UINT16>(inputs[7]), 通道颜色, (维度顺序)Get数值<UINT8>(inputs[9])));
	}
	catch (Image5D异常 异常)
	{
		free(通道颜色);
		throw 异常;
	}
}
API声明(PixelType)
{
	if (inputs.size() > 2)
		outputs[0] = 异常转换(Get对象指针<IOmeTiff读写器>(inputs[1])->PixelType((像素类型)Get数值<UINT8>(inputs[2])));
	else
		outputs[0] = 数组工厂.createScalar((UINT8)Get对象指针<const IOmeTiff读写器>(inputs[1])->PixelType());
}
API声明(DimensionOrder)
{
	if (inputs.size() > 2)
		outputs[0] = 异常转换(Get对象指针<IOmeTiff读写器>(inputs[1])->DimensionOrder((维度顺序)Get数值<UINT8>(inputs[2])));
	else
		outputs[0] = 数组工厂.createScalar((UINT8)Get对象指针<const IOmeTiff读写器>(inputs[1])->DimensionOrder());
}
API声明(SizeP)
{
	outputs[0] = 数组工厂.createScalar(Get对象指针<const IOmeTiff读写器>(inputs[1])->SizeP());
}
#define 尺寸属性(类型,字段) API声明(字段)\
{\
	if (inputs.size() > 2)\
		outputs[0] = 异常转换(Get对象指针<IOmeTiff读写器>(inputs[1])->字段(Get数值<类型>(inputs[2])));\
	else\
		outputs[0] = 数组工厂.createScalar(Get对象指针<const IOmeTiff读写器>(inputs[1])->字段());\
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
		颜色* const 通道颜色 = (颜色*)TypedArray<UINT32>(std::move(inputs[2])).release().release();
		try
		{
			outputs[0] = 异常转换(Get对象指针<IOmeTiff读写器>(inputs[1])->通道颜色(通道颜色));
		}
		catch (Image5D异常 异常)
		{
			free(通道颜色);
			throw 异常;
		}
	}
	else
	{
		const IOmeTiff读写器* 对象指针 = Get对象指针<const IOmeTiff读写器>(inputs[1]);
		const UINT8 SizeC = 对象指针->SizeC();
		outputs[0] = 数组工厂.createArrayFromBuffer({ 3,SizeC }, GetBuffer((UINT8*)对象指针->通道颜色(), SizeC * 3));
	}
}
API声明(ImageDescription)
{
	if (inputs.size() > 2)
	{
		char* 输入缓冲 = GetUtf8(inputs[2]);
		try
		{
			outputs[0] = 异常转换(Get对象指针<IOmeTiff读写器>(inputs[1])->图像描述(输入缓冲));
		}
		catch (Image5D异常 异常)
		{
			free(输入缓冲);
			throw 异常;
		}
	}
	else
		outputs[0] = GetCharArray(Get对象指针<const IOmeTiff读写器>(inputs[1])->图像描述());
}
API声明(ModifyParameters)
{
	颜色* 通道颜色 = (颜色*)TypedArray<UINT32>(std::move(inputs[8])).release().release();
	//如果有异常直接跳出，不会泄露内存
	char* 文件名 = GetUtf8(inputs[9]);
	try
	{
		outputs[0] = 异常转换(Get对象指针<IOmeTiff读写器>(inputs[1])->修改参数((像素类型)Get数值<UINT8>(inputs[2]), Get数值<UINT16>(inputs[3]), Get数值<UINT16>(inputs[4]), Get数值<UINT8>(inputs[5]), Get数值<UINT8>(inputs[6]), Get数值<UINT8>(inputs[7]),通道颜色 , (维度顺序)Get数值<UINT8>(inputs[8]), 文件名));
	}
	catch (Image5D异常 异常)
	{
		free(通道颜色);
		free(文件名);
		throw 异常;
	}
	free(文件名);
}
#define 类型化读写(API名) API声明(API名)\
{\
	const IOmeTiff读写器* 对象指针 = Get对象指针<const IOmeTiff读写器>(inputs[1]);\
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
template <typename T>
concept NumberType = std::is_arithmetic<T>::value;
template<NumberType T>
inline void ReadPixelsI(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const UINT16 SizeX = 对象指针->SizeX();
	const UINT16 SizeY = 对象指针->SizeY();
	const UINT32 ISize = Get数值<UINT32>(inputs[3]);
	buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>((UINT64)ISize * SizeX * SizeY);
	const Image5D异常 异常 = 对象指针->读入像素((char*)缓冲.get(), Get数值<UINT32>(inputs[2]), ISize);
	if (异常.类型 == 操作成功)
		outputs[0] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,ISize }, std::move(缓冲));
	else
		outputs[0] = 异常转换(异常);
}
类型化读写(ReadPixelsI);
template<NumberType T>
inline void ReadPixelsTZC(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const UINT16 TSize = Get数值<UINT16>(inputs[3]);
	const UINT8 ZSize = Get数值<UINT8>(inputs[5]);
	const UINT8 CSize = Get数值<UINT8>(inputs[7]);
	const UINT16 SizeX = 对象指针->SizeX();
	const UINT16 SizeY = 对象指针->SizeY();
	buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>((UINT64)TSize * ZSize * CSize * SizeX * SizeY);
	const Image5D异常 异常 = 对象指针->读入像素((char*)缓冲.get(), Get数值<UINT16>(inputs[2]), TSize, Get数值<UINT8>(inputs[4]), ZSize, Get数值<UINT8>(inputs[6]), CSize);
	if (异常.类型 == 操作成功)
		outputs[0] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,CSize,ZSize,TSize }, std::move(缓冲));
	else
		outputs[0] = 异常转换(异常);
}
类型化读写(ReadPixelsTZC);
template<NumberType T>
inline void ReadPixelsT(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const UINT16 TSize = Get数值<UINT16>(inputs[3]);
	const UINT16 SizeX = 对象指针->SizeX();
	const UINT16 SizeY = 对象指针->SizeY();
	const UINT8 SizeC = 对象指针->SizeC();
	const UINT8 SizeZ = 对象指针->SizeZ();
	buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>((UINT64)TSize * SizeZ * SizeC * SizeX * SizeY);
	const Image5D异常 异常 = 对象指针->读入像素((char*)缓冲.get(), Get数值<UINT16>(inputs[2]), TSize);
	if (异常.类型 == 操作成功)
		outputs[0] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,SizeC,SizeZ,TSize }, std::move(缓冲));
	else
		outputs[0] = 异常转换(异常);
}
类型化读写(ReadPixelsT);
template<NumberType T>
inline void WritePixelsI(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const buffer_ptr_t<T>缓冲 = TypedArray<T>(std::move(inputs[2])).release();
	outputs[0] = 异常转换(对象指针->写出像素((char*)缓冲.get(), Get数值<UINT32>(inputs[3]), Get数值<UINT32>(inputs[4])));
}
类型化读写(WritePixelsI);
template<NumberType T>
inline void WritePixelsTZC(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const buffer_ptr_t<T>缓冲 = TypedArray<T>(std::move(inputs[2])).release();
	outputs[0] = 异常转换(对象指针->写出像素((char*)缓冲.get(), Get数值<UINT16>(inputs[3]), Get数值<UINT16>(inputs[4]), Get数值<UINT8>(inputs[5]), Get数值<UINT8>(inputs[6]), Get数值<UINT8>(inputs[7]), Get数值<UINT8>(inputs[8])));
}
类型化读写(WritePixelsTZC);
template<NumberType T>
inline void WritePixelsT(const IOmeTiff读写器* 对象指针, ArgumentList& outputs, ArgumentList& inputs)
{
	const buffer_ptr_t<T>缓冲 = TypedArray<T>(std::move(inputs[2])).release();
	outputs[0] = 异常转换(对象指针->写出像素((char*)缓冲.get(), Get数值<UINT16>(inputs[3]), Get数值<UINT16>(inputs[4])));
}
类型化读写(WritePixelsT);
API声明(PixelPointer)
{
	const IOmeTiff读写器* 对象指针 = Get对象指针<const IOmeTiff读写器>(inputs[1]);
	if (inputs.size() < 5)
		outputs[0] = 数组工厂.createScalar((UINT64)对象指针->像素指针(Get数值<UINT32>(inputs[2])));
	else
		outputs[0] = 数组工厂.createScalar((UINT64)对象指针->像素指针(Get数值<UINT16>(inputs[2]), Get数值<UINT8>(inputs[3]), Get数值<UINT8>(inputs[4])));
}
API声明(Close)
{
	delete Get对象指针<IOmeTiff读写器>(inputs[1]);
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