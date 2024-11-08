#include<Oir读入器.h>
#include<Mex工具.hpp>
using namespace Image5D;
using namespace Mex工具;
using namespace matlab::data;
static Oir读入器* 取指针(Array&& 指针标量)
{
	Oir读入器* const 指针 = 万能转码<Oir读入器*>(std::move(指针标量));
	if (对象存在(指针))
		return 指针;
	else
		throw Exception::Invalid_pointer;
}
Mex工具API(Oir_CreateReader)
{
	const String 文件路径 = 万能转码<String>(std::move(输入[1]));
	Oir读入器*const 对象指针 = new Oir读入器((LPCWSTR)文件路径.c_str());
	输出[0] = 万能转码(对象指针);
	自动析构(对象指针);
}
Mex工具API(Oir_DeleteReader)
{
	Oir读入器* const 指针 = 万能转码<Oir读入器*>(std::move(输入[1]));
	if (手动析构(指针))
		delete 指针;
}
Mex工具API(Oir_SizeX)
{
	输出[0] = 万能转码(取指针(std::move(输入[1]))->SizeX());
}
Mex工具API(Oir_SizeY)
{
	输出[0] = 万能转码(取指针(std::move(输入[1]))->SizeY());
}
Mex工具API(Oir_SizeC)
{
	输出[0] = 万能转码(取指针(std::move(输入[1]))->SizeC());
}
Mex工具API(Oir_SizeZ)
{
	输出[0] = 万能转码(取指针(std::move(输入[1]))->SizeZ());
}
Mex工具API(Oir_SizeT)
{
	输出[0] = 万能转码(取指针(std::move(输入[1]))->SizeT());
}
Mex工具API(Oir_SeriesInterval)
{
	输出[0] = MATLAB引擎->feval<Array>("milliseconds", 取指针(std::move(输入[1]))->系列间隔());
}
Mex工具API(Oir_DeviceColors)
{
	const Oir读入器* const 指针 = 取指针(std::move(输入[1]));
	const uint8_t SizeC = 指针->SizeC();
	const uint8_t 颜色尺寸 = SizeC * 4;
	buffer_ptr_t<float> 颜色缓冲 = 数组工厂.createBuffer<float>(颜色尺寸);
	float* 颜色缓冲头 = 颜色缓冲.get();
	StringArray 设备缓冲 = 数组工厂.createArray<MATLABString>({ SizeC });
	TypedIterator<MATLABString> 设备缓冲头 = 设备缓冲.begin();
	const 设备颜色* 通道颜色 = 指针->通道颜色();
	for (const 设备颜色* const 通道尾 = 通道颜色 + SizeC; 通道颜色 < 通道尾; 通道颜色++)
	{
		*(设备缓冲头++) = 万能转码<MATLABString>(通道颜色->设备名称);
		float* 分量头 = 颜色缓冲头++;
		*分量头 = 通道颜色->不透明度;
		*(分量头 += SizeC) = 通道颜色->蓝;
		*(分量头 += SizeC) = 通道颜色->绿;
		*(分量头 + SizeC) = 通道颜色->红;
	}
	输出[0] = 设备缓冲;
	输出[1] = 数组工厂.createArrayFromBuffer({ SizeC,4 }, std::move(颜色缓冲));
}
Mex工具API(Oir_ReadPixels)
{
	const Oir读入器* const 指针 = 取指针(std::move(输入[1]));
	const uint16_t SizeX = 指针->SizeX();
	const uint16_t SizeY = 指针->SizeY();
	switch (输入.size())
	{
	case 2:
	{
		const uint8_t SizeZ = 指针->SizeZ();
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(uint64_t(指针->SizeT()) * SizeZ * 指针->SizeC() * SizeY * SizeX);
		指针->读入像素(缓冲.get());
		输出[0] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,指针->SizeC(),SizeZ,指针->SizeT() }, std::move(缓冲));
	}
	break;
	case 4:
	{
		const uint8_t SizeZ = 指针->SizeZ();
		const uint32_t TSize = 万能转码<uint32_t>(std::move(输入[3]));
		const uint8_t SizeC = 指针->SizeC();
		const uint64_t 尺寸 = uint64_t(TSize) * SizeZ * SizeC * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint32_t>(std::move(输入[2])), TSize);
		输出[0] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,SizeC,SizeZ,TSize }, std::move(缓冲));
	}
	break;
	case 5:
	{
		const uint8_t SizeZ = 指针->SizeZ();
		const uint32_t TSize = 万能转码<uint32_t>(std::move(输入[3]));
		const uint64_t 尺寸 = uint64_t(TSize) * SizeZ * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint32_t>(std::move(输入[2])), TSize, 万能转码<uint8_t>(std::move(输入[4])));
		输出[0] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,1,SizeZ,TSize }, std::move(缓冲));
	}
	break;
	case 6:
	{
		const uint8_t SizeC = 指针->SizeC();
		const uint32_t TSize = 万能转码<uint32_t>(std::move(输入[3]));
		const uint8_t ZSize = 万能转码<uint8_t>(std::move(输入[5]));
		const uint64_t 尺寸 = uint64_t(TSize) * ZSize * SizeC * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint32_t>(std::move(输入[2])), TSize, 万能转码<uint8_t>(std::move(输入[4])), ZSize);
		输出[0] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,SizeC,ZSize,TSize }, std::move(缓冲));
	}
	break;
	case 8:
	{
		const uint32_t TSize = 万能转码<uint32_t>(std::move(输入[3]));
		const uint8_t ZSize = 万能转码<uint8_t>(std::move(输入[5]));
		const uint8_t CSize = 万能转码<uint8_t>(std::move(输入[7]));
		const uint64_t 尺寸 = uint64_t(TSize) * ZSize * CSize * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint32_t>(std::move(输入[2])), TSize, 万能转码<uint8_t>(std::move(输入[4])), ZSize, 万能转码<uint8_t>(std::move(输入[6])), CSize);
		输出[0] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,CSize,ZSize,TSize }, std::move(缓冲));
	}
	break;
	default:
		throw Exception::The_number_of_entered_parameters_is_incorrect;
	}
}
Mex工具API(Oir_ReadToPointer)
{
	const Oir读入器* const 对象指针 = 取指针(std::move(输入[1]));
	uint16_t* const 输出指针 = 万能转码<uint16_t*>(std::move(输入[2]));
	const size_t 输出容量 = 万能转码<size_t>(std::move(输入[3]));
	const uint32_t SizePXY = 2 * 对象指针->SizeX() * 对象指针->SizeY();
	switch (输入.size())
	{
	case 4:
		if ((uint64_t)SizePXY * 对象指针->SizeT() * 对象指针->SizeZ() * 对象指针->SizeC() > 输出容量)
			throw Exception::Output_pointer_memory_overflow;
		对象指针->读入像素(输出指针);
		break;
	case 6:
	{
		const uint32_t TSize = 万能转码<uint32_t>(std::move(输入[5]));
		if ((uint64_t)SizePXY * TSize * 对象指针->SizeZ() * 对象指针->SizeC() > 输出容量)
			throw Exception::Output_pointer_memory_overflow;
		对象指针->读入像素(输出指针, 万能转码<uint32_t>(std::move(输入[4])), TSize);
	}
	break;
	case 7:
	{
		const uint32_t TSize = 万能转码<uint32_t>(std::move(输入[5]));
		if ((uint64_t)SizePXY * TSize * 对象指针->SizeZ() > 输出容量)
			throw Exception::Output_pointer_memory_overflow;
		对象指针->读入像素(输出指针, 万能转码<uint32_t>(std::move(输入[4])), TSize, 万能转码<uint8_t>(std::move(输入[6])));
	}
	break;
	case 8:
	{
		const uint32_t TSize = 万能转码<uint32_t>(std::move(输入[5]));
		const uint8_t ZSize = 万能转码<uint8_t>(std::move(输入[7]));
		if ((uint64_t)SizePXY * TSize * ZSize * 对象指针->SizeC() > 输出容量)
			throw Exception::Output_pointer_memory_overflow;
		对象指针->读入像素(输出指针, 万能转码<uint32_t>(std::move(输入[4])), TSize, 万能转码<uint8_t>(std::move(输入[6])), ZSize);
	}
	break;
	case 10:
	{
		const uint32_t TSize = 万能转码<uint32_t>(std::move(输入[5]));
		const uint8_t ZSize = 万能转码<uint8_t>(std::move(输入[7]));
		const uint8_t CSize = 万能转码<uint8_t>(std::move(输入[9]));
		if ((uint64_t)SizePXY * TSize * ZSize * CSize > 输出容量)
			throw Exception::Output_pointer_memory_overflow;
		对象指针->读入像素(输出指针, 万能转码<uint32_t>(std::move(输入[4])), TSize, 万能转码<uint8_t>(std::move(输入[6])), ZSize, 万能转码<uint8_t>(std::move(输入[8])), CSize);
	}
	break;
	default:
		throw Exception::The_number_of_entered_parameters_is_incorrect;
	}
}
Mex工具API(Oir_LsmimageXml)
{
	const Oir读入器* const 对象指针 = 取指针(std::move(输入[1]));
	const char* 指针;
	uint32_t 长度;
	对象指针->LsmimageXml(指针, 长度);
	//LsmimageXml中不会包含非ASCII字符，因此可以直接隐式转换，无需重新编码
	输出[0] = 万能转码<CharArray>(指针, { 1,长度 });
}
Mex工具API(Oir_LaserTransmissivity)
{
	const Oir读入器* const 对象指针 = 取指针(std::move(输入[1]));
	输出[0] = 万能转码(对象指针->激光透过率(), { 1,对象指针->SizeZ() });
}
Mex工具API(Oir_PmtVoltage)
{
	const Oir读入器* const 对象指针 = 取指针(std::move(输入[1]));
	输出[0] = 万能转码(对象指针->放大电压(), { 对象指针->SizeC(),对象指针->SizeZ() });
}
Mex工具API(Oir_ZDriveUnitType)
{
	const Oir读入器* const 对象指针 = 取指针(std::move(输入[1]));
	输出[0] = 万能转码<CharArray>(对象指针->Z驱动单元类型());
}
Mex工具API(Oir_ConcatenateSizeT)
{
	const Oir读入器* const 对象指针 = 取指针(std::move(输入[1]));
	输出[0] = 万能转码(对象指针->拼接SizeT(), { 对象指针->文件序列拼接数(),1 });
}
Mex工具API(Oir_ChannelColors)
{
	const Oir读入器* const 对象指针 = 取指针(std::move(输入[1]));
	const 设备颜色* 当前通道 = 对象指针->通道颜色();
	TypedArray<uint32_t>颜色数组 = 数组工厂.createArray<uint32_t>({ 对象指针->SizeC(),1 });
	for (uint32_t& 颜色 : 颜色数组)
	{
		uint8_t* 透蓝绿红 = (uint8_t*)&颜色;
		透蓝绿红[0] = 当前通道->不透明度 * 255;
		透蓝绿红[1] = 当前通道->蓝 * 255;
		透蓝绿红[2] = 当前通道->绿 * 255;
		透蓝绿红[3] = 当前通道++->红 * 255;
	}
	输出[0] = 颜色数组;
}
Mex工具API(Oir_CreationDateTime)
{
	std::string 日期时间字符串 = 取指针(std::move(输入[1]))->创建日期时间();
	日期时间字符串[10] = ' ';	
	输出[0] = MATLAB引擎->feval<Array>("datetime", 日期时间字符串.substr(0, 23), "TimeZone", 日期时间字符串.substr(23));
}