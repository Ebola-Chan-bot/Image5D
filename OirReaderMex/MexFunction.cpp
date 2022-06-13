#include "pch.h"
#include "异常转换.h"
#include "Oir读入器.h"
API声明(CreateOirReader)
{
	char* 文件路径 = GetUtf8(inputs[1]);
	try
	{
		outputs[0] = 数组工厂.createScalar(uint64_t(Oir读入器::创建(文件路径)));
	}
	catch (Image5D异常 异常)
	{
		free(文件路径);
		throw 异常;
	}
	free(文件路径);
}
API声明(DestroyOirReader)
{
	delete Get对象指针<Oir读入器>(inputs[1]);
}
API声明(SizeX)
{
	outputs[0] = 数组工厂.createScalar(Get对象指针<Oir读入器>(inputs[1])->SizeX);
}
API声明(SizeY)
{
	outputs[0] = 数组工厂.createScalar(Get对象指针<Oir读入器>(inputs[1])->SizeY);
}
API声明(SizeC)
{
	outputs[0] = 数组工厂.createScalar(Get对象指针<Oir读入器>(inputs[1])->SizeC);
}
API声明(SizeZ)
{
	outputs[0] = 数组工厂.createScalar(Get对象指针<Oir读入器>(inputs[1])->SizeZ);
}
API声明(SizeT)
{
	outputs[0] = 数组工厂.createScalar(Get对象指针<Oir读入器>(inputs[1])->SizeT);
}
API声明(系列间隔)
{
	outputs[0] = 数组工厂.createScalar(Get对象指针<Oir读入器>(inputs[1])->系列间隔);
}
API声明(通道颜色)
{
	const Oir读入器* const 指针 = Get对象指针<Oir读入器>(inputs[1]);
	const uint8_t SizeC = 指针->SizeC;
	const uint8_t 尺寸 = SizeC * 3;
	buffer_ptr_t<float> 缓冲 = 数组工厂.createBuffer<float>(尺寸);
	std::copy_n(指针->通道颜色[SizeC], 尺寸, 缓冲.get());
	outputs[0] = 数组工厂.createArrayFromBuffer({ SizeC,3 }, std::move(缓冲), MemoryLayout::ROW_MAJOR);
}
API声明(设备名称)
{
	const Oir读入器* const 指针 = Get对象指针<Oir读入器>(inputs[1]);
	outputs[0] = GetStringVector(指针->SizeC, 指针->设备名称);
}
API声明(图像属性)
{
	outputs[0] = GetCharArray(Get对象指针<Oir读入器>(inputs[1])->图像属性);
}
API声明(读入像素)
{
	const Oir读入器* const 指针 = Get对象指针<Oir读入器>(inputs[1]);
	const uint16_t SizeX = 指针->SizeX;
	const uint16_t SizeY = 指针->SizeY;
	const uint8_t SizeZ = 指针->SizeZ;
	switch (inputs.size())
	{
	case 4:
	{
		const uint16_t TSize = Get数值<UINT16>(inputs[3]);
		const uint8_t SizeC = 指针->SizeC;
		const uint64_t 尺寸 = uint64_t(TSize) * SizeZ * SizeC * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		outputs[0] = 数组工厂.createScalar(指针->读入像素(缓冲.get(), Get数值<uint16_t>(inputs[2]), TSize));
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,SizeC,SizeZ,TSize }, std::move(缓冲));
	}
	break;
	case 5:
	{
		const uint16_t TSize = Get数值<uint16_t>(inputs[3]);
		const uint64_t 尺寸 = uint64_t(TSize) * SizeZ * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		outputs[0] = 数组工厂.createScalar(指针->读入像素(缓冲.get(), Get数值<uint16_t>(inputs[2]), TSize, Get数值<uint8_t>(inputs[4])));
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,1,SizeZ,TSize }, std::move(缓冲));
	}
	break;
	case 8:
		const uint16_t TSize = Get数值<uint16_t>(inputs[3]);
		const uint8_t ZSize = Get数值<uint8_t>(inputs[5]);
		const uint8_t CSize = Get数值<uint8_t>(inputs[7]);
		const uint64_t 尺寸 = uint64_t(TSize) * ZSize * CSize * SizeY * SizeX;
		buffer_ptr_t<uint16_t> 缓冲 = 数组工厂.createBuffer<uint16_t>(尺寸);
		outputs[0] = 数组工厂.createScalar(指针->读入像素(缓冲.get(), Get数值<uint16_t>(inputs[2]), TSize, Get数值<uint8_t>(inputs[4]), ZSize, Get数值<uint8_t>(inputs[6]), CSize));
		outputs[1] = 数组工厂.createArrayFromBuffer({ SizeX,SizeY,CSize,ZSize,TSize }, std::move(缓冲));
		break;
	}
}
void MexFunction::operator()(ArgumentList outputs, ArgumentList inputs)
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
		系列间隔,
		通道颜色,
		设备名称,
		图像属性,
		读入像素
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