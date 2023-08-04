#include<Oir读入器.h>
#include<Mex工具.h>
using namespace Image5D;
using namespace Mex工具;
API声明(Oir_CreateReader)
{
	const String 文件路径 = 万能转码<String>(inputs[1]);
	outputs[1] = 万能转码(new Oir读入器((LPCWSTR)文件路径.c_str()));
}
API声明(Oir_DeleteReader)
{
	__try
	{
		delete 万能转码<Oir读入器*>(inputs[1]);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {}
}
API声明(Oir_SizeX)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->SizeX());
}
API声明(Oir_SizeY)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->SizeY());
}
API声明(Oir_SizeC)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->SizeC());
}
API声明(Oir_SizeZ)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->SizeZ());
}
API声明(Oir_SizeT)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->SizeT());
}
API声明(Oir_SeriesInterval)
{
	outputs[1] = 万能转码(万能转码<const Oir读入器*>(inputs[1])->系列间隔());
}
API声明(Oir_DeviceColors)
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
		float* 分量头 = 颜色缓冲头++;
		*分量头 = 通道颜色->红;
		*(分量头 += SizeC) = 通道颜色->绿;
		*(分量头 + SizeC) = 通道颜色->蓝;
	}
	outputs[1] = 设备缓冲;
	outputs[2] = 数组工厂.createArrayFromBuffer({ SizeC,3 }, std::move(颜色缓冲));
}
API声明(Oir_ReadPixels)
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
		const uint32_t TSize = 万能转码<uint32_t>(inputs[3]);
		const uint8_t SizeC = 指针->SizeC();
		const uint64_t 尺寸 = uint64_t(TSize) * SizeZ * SizeC * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint32_t>(inputs[2]), TSize);
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,SizeC,SizeZ,TSize }, std::move(缓冲));
	}
	break;
	case 5:
	{
		const uint8_t SizeZ = 指针->SizeZ();
		const uint32_t TSize = 万能转码<uint32_t>(inputs[3]);
		const uint64_t 尺寸 = uint64_t(TSize) * SizeZ * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint32_t>(inputs[2]), TSize, 万能转码<uint8_t>(inputs[4]));
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,1,SizeZ,TSize }, std::move(缓冲));
	}
	break;
	case 6:
	{
		const uint8_t SizeC = 指针->SizeC();
		const uint32_t TSize = 万能转码<uint32_t>(inputs[3]);
		const uint8_t ZSize = 万能转码<uint8_t>(inputs[5]);
		const uint64_t 尺寸 = uint64_t(TSize) * ZSize * SizeC * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint32_t>(inputs[2]), TSize, 万能转码<uint8_t>(inputs[4]), ZSize);
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,SizeC,ZSize,TSize }, std::move(缓冲));
	}
	break;
	case 8:
	{
		const uint32_t TSize = 万能转码<uint32_t>(inputs[3]);
		const uint8_t ZSize = 万能转码<uint8_t>(inputs[5]);
		const uint8_t CSize = 万能转码<uint8_t>(inputs[7]);
		const uint64_t 尺寸 = uint64_t(TSize) * ZSize * CSize * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		指针->读入像素(缓冲.get(), 万能转码<uint32_t>(inputs[2]), TSize, 万能转码<uint8_t>(inputs[4]), ZSize, 万能转码<uint8_t>(inputs[6]), CSize);
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,CSize,ZSize,TSize }, std::move(缓冲));
	}
	break;
	default:
		throw 参数异常;
	}
}
API声明(Oir_ReadToPointer)
{
	const Oir读入器* const 对象指针 = 万能转码<Oir读入器*>(inputs[1]);
	uint16_t* const 输出指针 = 万能转码<uint16_t*>(inputs[2]);
	const size_t 输出容量 = 万能转码<size_t>(inputs[3]);
	const uint32_t SizePXY = 2 * 对象指针->SizeX() * 对象指针->SizeY();
	switch (inputs.size())
	{
	case 4:
		if ((uint64_t)SizePXY * 对象指针->SizeT() * 对象指针->SizeZ() * 对象指针->SizeC() > 输出容量)
			throw 内存溢出;
		对象指针->读入像素(输出指针);
		break;
	case 6:
	{
		const uint32_t TSize = 万能转码<uint32_t>(inputs[5]);
		if ((uint64_t)SizePXY * TSize * 对象指针->SizeZ() * 对象指针->SizeC() > 输出容量)
			throw 内存溢出;
		对象指针->读入像素(输出指针, 万能转码<uint32_t>(inputs[4]), TSize);
	}
	break;
	case 7:
	{
		const uint32_t TSize = 万能转码<uint32_t>(inputs[5]);
		if ((uint64_t)SizePXY * TSize * 对象指针->SizeZ() > 输出容量)
			throw 内存溢出;
		对象指针->读入像素(输出指针, 万能转码<uint32_t>(inputs[4]), TSize, 万能转码<uint8_t>(inputs[6]));
	}
	break;
	case 8:
	{
		const uint32_t TSize = 万能转码<uint32_t>(inputs[5]);
		const uint8_t ZSize = 万能转码<uint8_t>(inputs[7]);
		if ((uint64_t)SizePXY * TSize * ZSize * 对象指针->SizeC() > 输出容量)
			throw 内存溢出;
		对象指针->读入像素(输出指针, 万能转码<uint32_t>(inputs[4]), TSize, 万能转码<uint8_t>(inputs[6]), ZSize);
	}
	break;
	case 10:
	{
		const uint32_t TSize = 万能转码<uint32_t>(inputs[5]);
		const uint8_t ZSize = 万能转码<uint8_t>(inputs[7]);
		const uint8_t CSize = 万能转码<uint8_t>(inputs[9]);
		if ((uint64_t)SizePXY * TSize * ZSize * CSize > 输出容量)
			throw 内存溢出;
		对象指针->读入像素(输出指针, 万能转码<uint32_t>(inputs[4]), TSize, 万能转码<uint8_t>(inputs[6]), ZSize, 万能转码<uint8_t>(inputs[8]), CSize);
	}
	break;
	default:
		throw 参数异常;
	}
}
API声明(Oir_LsmimageXml)
{
	const Oir读入器* const 对象指针 = 万能转码<Oir读入器*>(inputs[1]);
	const char* 指针;
	uint32_t 长度;
	对象指针->LsmimageXml(指针, 长度);
	//LsmimageXml中不会包含非ASCII字符，因此可以直接隐式转换，无需重新编码
	outputs[1] = 万能转码<CharArray>(指针, { 1,长度 });
}
API声明(Oir_LaserTransmissivity)
{
	const Oir读入器* const 对象指针 = 万能转码<Oir读入器*>(inputs[1]);
	outputs[1] = 万能转码(对象指针->激光透过率(), { 1,对象指针->SizeZ() });
}
API声明(Oir_PmtVoltage)
{
	const Oir读入器* const 对象指针 = 万能转码<Oir读入器*>(inputs[1]);
	outputs[1] = 万能转码(对象指针->放大电压(), { 对象指针->SizeC(),对象指针->SizeZ() });
}