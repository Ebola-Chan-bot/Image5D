#include "pch.h"
#include <Mex实现.h>
#include <Mex工具.h>
#include "Oir读入器.h"
API声明(CreateOirReader)
{
	const String 文件路径 = 万能转码<String>(inputs[1]);
	outputs[1] = 万能转码(new Oir读入器((wchar_t*)文件路径.c_str()));
}
API声明(DestroyOirReader)
{
	delete 万能转码<Oir读入器*>(inputs[1]);
}
API声明(SizeX)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->SizeX());
}
API声明(SizeY)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->SizeY());
}
API声明(SizeC)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->SizeC());
}
API声明(SizeZ)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->SizeZ());
}
API声明(SizeT)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->SizeT());
}
API声明(SeriesInterval)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->系列间隔());
}
API声明(DeviceColors)
{
	const Oir读入器* const 指针 = 万能转码<Oir读入器*>(inputs[1]);
	const uint8_t SizeC = 指针->SizeC();
	const uint8_t 颜色尺寸 = SizeC * 3;
	buffer_ptr_t<float> 颜色缓冲 = 数组工厂.createBuffer<float>(颜色尺寸);
	float* 颜色缓冲头 = 颜色缓冲.get();
	StringArray 设备缓冲 = 数组工厂.createArray<MATLABString>({ SizeC });
	TypedIterator<MATLABString> 设备缓冲头 = 设备缓冲.begin();
	const 设备颜色* 通道颜色 = 指针->通道颜色();
	for (const 设备颜色* const 通道尾 = 通道颜色 + SizeC; 通道颜色 < 通道尾; 通道颜色++)
	{
		*(设备缓冲头++) = 万能转码<MATLABString>(通道颜色->设备名称);
		*(颜色缓冲头++) = 通道颜色->红;
		*(颜色缓冲头++) = 通道颜色->绿;
		*(颜色缓冲头++) = 通道颜色->蓝;
	}
	outputs[1] = 设备缓冲;
	outputs[2] = 数组工厂.createArrayFromBuffer({ 3,SizeC }, std::move(颜色缓冲));
}
API声明(ReadPixels)
{
	const Oir读入器* const 指针 = 万能转码<Oir读入器*>(inputs[1]);
	const uint16_t SizeX = 指针->SizeX();
	const uint16_t SizeY = 指针->SizeY();
	switch (inputs.size())
	{
	case 2:
	{
		const uint8_t SizeZ = 指针->SizeZ();
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(uint64_t(指针->SizeT()) * SizeZ * 指针->SizeC() * SizeY * SizeX);
		指针->读入像素(缓冲.get());
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,指针->SizeC(),SizeZ,指针->SizeT() }, std::move(缓冲));
	}
	break;
	case 4:
	{
		const uint8_t SizeZ = 指针->SizeZ();
		const uint16_t TSize = 万能转码<uint16_t>(inputs[3]);
		const uint8_t SizeC = 指针->SizeC();
		const uint64_t 尺寸 = uint64_t(TSize) * SizeZ * SizeC * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint16_t>(inputs[2]), TSize);
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,SizeC,SizeZ,TSize }, std::move(缓冲));
	}
	break;
	case 5:
	{
		const uint8_t SizeZ = 指针->SizeZ();
		const uint16_t TSize = 万能转码<uint16_t>(inputs[3]);
		const uint64_t 尺寸 = uint64_t(TSize) * SizeZ * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint16_t>(inputs[2]), TSize, 万能转码<uint8_t>(inputs[4]));
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,1,SizeZ,TSize }, std::move(缓冲));
	}
	break;
	case 6:
	{
		const uint8_t SizeC = 指针->SizeC();
		const uint16_t TSize = 万能转码<uint16_t>(inputs[3]);
		const uint8_t ZSize = 万能转码<uint8_t>(inputs[5]);
		const uint64_t 尺寸 = uint64_t(TSize) * ZSize * SizeC * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint16_t>(inputs[2]), TSize, 万能转码<uint8_t>(inputs[4]), ZSize);
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,SizeC,ZSize,TSize }, std::move(缓冲));
	}
	break;
	case 8:
	{
		const uint16_t TSize = 万能转码<uint16_t>(inputs[3]);
		const uint8_t ZSize = 万能转码<uint8_t>(inputs[5]);
		const uint8_t CSize = 万能转码<uint8_t>(inputs[7]);
		const uint64_t 尺寸 = uint64_t(TSize) * ZSize * CSize * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint16_t>(inputs[2]), TSize, 万能转码<uint8_t>(inputs[4]), ZSize, 万能转码<uint8_t>(inputs[6]), CSize);
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,CSize,ZSize,TSize }, std::move(缓冲)); 
	}
		break;
	default:
		throw 参数异常;
	}
}
API声明(ReadToPointer)
{
	const Oir读入器* const 对象指针 = 万能转码<Oir读入器*>(inputs[1]);
	uint16_t* const 输出指针 = 万能转码<uint16_t*>(inputs[2]);
	const size_t 输出容量 = 万能转码<size_t>(inputs[3]);
	const size_t SizePXY = 2 * 对象指针->SizeX() * 对象指针->SizeY();
	switch (inputs.size())
	{
	case 4:
		if (SizePXY * 对象指针->SizeT() * 对象指针->SizeZ() * 对象指针->SizeC() > 输出容量)
			throw 内存溢出;
		对象指针->读入像素(输出指针);
		break;
	case 6:
	{
		const uint16_t TSize = 万能转码<uint16_t>(inputs[5]);
		if (SizePXY * TSize * 对象指针->SizeZ() * 对象指针->SizeC() > 输出容量)
			throw 内存溢出;
		对象指针->读入像素(输出指针, 万能转码<uint16_t>(inputs[4]), TSize);
	}
	break;
	case 7:
	{
		const uint16_t TSize = 万能转码<uint16_t>(inputs[5]);
		if (SizePXY * TSize * 对象指针->SizeZ() > 输出容量)
			throw 内存溢出;
		对象指针->读入像素(输出指针, 万能转码<uint16_t>(inputs[4]), TSize, 万能转码<uint8_t>(inputs[6]));
	}
	break;
	case 8:
	{
		const uint16_t TSize = 万能转码<uint16_t>(inputs[5]);
		const uint8_t ZSize = 万能转码<uint8_t>(inputs[7]);
		if (SizePXY * TSize * ZSize * 对象指针->SizeC() > 输出容量)
			throw 内存溢出;
		对象指针->读入像素(输出指针, 万能转码<uint16_t>(inputs[4]), TSize, 万能转码<uint8_t>(inputs[6]), ZSize);
	}
	break;
	case 10:
	{
		const uint16_t TSize = 万能转码<uint16_t>(inputs[5]);
		const uint8_t ZSize = 万能转码<uint8_t>(inputs[7]);
		const uint8_t CSize = 万能转码<uint8_t>(inputs[9]);
		if (SizePXY * TSize * ZSize * CSize > 输出容量)
			throw 内存溢出;
		对象指针->读入像素(输出指针, 万能转码<uint16_t>(inputs[4]), TSize, 万能转码<uint8_t>(inputs[6]), ZSize, 万能转码<uint8_t>(inputs[8]), CSize);
	}
	break;
	default:
		throw 参数异常;
	}
}
void MexFunction::operator()(ArgumentList& outputs, ArgumentList& inputs)
{
	API索引
	{
		CreateOirReader,
		DestroyOirReader,
		SizeX,
		SizeY,
		SizeC,
		SizeZ,
		SizeT,
		SeriesInterval,
		DeviceColors,
		ReadPixels,
		ReadToPointer,
	};
	try
	{
		API调用;
		outputs[0] = 成功结构;
	}
	catch (Image5D异常& 异常)
	{
		outputs[0] = 异常;
		异常输出补全(outputs);
	}
}