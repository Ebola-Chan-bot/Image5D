#pragma once
#include"Image5D异常.h"
#include"OME标准.h"
namespace Image5D
{
	//只有符合OB5标准的TIFF文件可以编辑，否则为只读。
	class IOmeTiff读写器
	{
	public:
		virtual 像素类型 PixelType()const { throw Image5D::Exception::Method_unimplemented; }
		virtual uint8_t SizeP()const { throw Image5D::Exception::Method_unimplemented; }
		virtual uint16_t SizeX()const { throw Image5D::Exception::Method_unimplemented; }
		virtual uint16_t SizeY()const { throw Image5D::Exception::Method_unimplemented; }
		virtual uint32_t SizeI()const { throw Image5D::Exception::Method_unimplemented; }
		virtual const char* 图像描述()const { throw Image5D::Exception::Method_unimplemented; }
		virtual uint8_t SizeC()const { throw Image5D::Exception::Method_unimplemented; }
		virtual uint8_t SizeZ()const { throw Image5D::Exception::Method_unimplemented; }
		virtual uint32_t SizeT()const { throw Image5D::Exception::Method_unimplemented; }
		virtual 维度顺序 DimensionOrder()const { throw Image5D::Exception::Method_unimplemented; }
		virtual const 颜色* 通道颜色()const { throw Image5D::Exception::Method_unimplemented; }
		virtual const char* 文件名()const { throw Image5D::Exception::Method_unimplemented; }

		virtual void PixelType(像素类型) { throw Image5D::Exception::Method_unimplemented; }
		virtual void SizeX(uint16_t) { throw Image5D::Exception::Method_unimplemented; }
		virtual void SizeY(uint16_t) { throw Image5D::Exception::Method_unimplemented; }
		virtual void SizeI(uint32_t) { throw Image5D::Exception::Method_unimplemented; }
		virtual void SizeC(uint8_t) { throw Image5D::Exception::Method_unimplemented; }
		virtual void SizeZ(uint8_t) { throw Image5D::Exception::Method_unimplemented; }
		virtual void SizeT(uint32_t) { throw Image5D::Exception::Method_unimplemented; }
		//读写器无法检查输入颜色指针是否指向足够长的内存段（至少应有SizeC长），由调用方保证指针正确。
		virtual void 通道颜色(const 颜色*) { throw Image5D::Exception::Method_unimplemented; }
		virtual void DimensionOrder(维度顺序) { throw Image5D::Exception::Method_unimplemented; }
		//图像描述中的文件名字段将覆盖对象中的文件名属性，而不会修改图像描述
		virtual void 图像描述(std::string&&) { throw Image5D::Exception::Method_unimplemented; }
		virtual void 文件名(const char*) { throw Image5D::Exception::Method_unimplemented; }
		/*
		请使用本方法同时修改多个参数，不要逐一调用每个参数的修改方法，那样会导致多次不必要的文件重构。每个参数自己的修改方法，本质上也是调用了本方法实现的，并没有专门的优化。
		0、nullptr、缺省值表示不修改该参数。注意，枚举类参数必需设为缺省以表示不做修改：缺省值不是0。这个规则同样适用于每个参数自己的修改方法。
		读写器无法检查输入颜色指针是否指向足够长的内存段（至少应有SizeC长），由调用方保证指针正确。
		*/
		virtual void 修改参数(像素类型 PixelType, uint16_t SizeX, uint16_t SizeY, uint8_t SizeC, uint8_t SizeZ, uint32_t SizeT, const 颜色* 通道颜色, 维度顺序 DimensionOrder, const char* 文件名) { throw Image5D::Exception::Method_unimplemented; }
		virtual void 读入像素(void* 缓冲区)const { throw Image5D::Exception::Method_unimplemented; }
		virtual void 读入像素I(void* 缓冲区, uint32_t IStart, uint32_t ISize)const { throw Image5D::Exception::Method_unimplemented; }
		virtual void 读入像素(void* 缓冲区, uint32_t TStart, uint32_t TSize, uint8_t ZStart, uint8_t ZSize, uint8_t CStart, uint8_t CSize)const { throw Image5D::Exception::Method_unimplemented; }
		virtual void 读入像素(void* 缓冲区, uint32_t TStart, uint32_t TSize)const { throw Image5D::Exception::Method_unimplemented; }
		virtual void 写出像素(const void* 缓冲区)const { throw Image5D::Exception::Method_unimplemented; }
		virtual void 写出像素I(const void* 缓冲区, uint32_t IStart, uint32_t ISize)const { throw Image5D::Exception::Method_unimplemented; }
		virtual void 写出像素(const void* 缓冲区, uint32_t TStart, uint32_t TSize, uint8_t ZStart, uint8_t ZSize, uint8_t CStart, uint8_t CSize)const { throw Image5D::Exception::Method_unimplemented; }
		virtual void 写出像素(const void* 缓冲区, uint32_t TStart, uint32_t TSize)const { throw Image5D::Exception::Method_unimplemented; }
		//OB5专用，获取像素数组的起始指针，以及允许读写的字节数。此指针直接指向磁盘文件，相比于写出像素，可以减少一次拷贝。
		virtual void 像素指针(void*& 指针, size_t& 容量)const { throw Image5D::Exception::Method_unimplemented; }
		//OB5专用，获取指定IFD的起始指针，以及允许读写的字节数。此指针直接指向磁盘文件，相比于写出像素，可以减少一次拷贝。
		virtual void 像素指针I(uint32_t I, void*& 指针, size_t& 容量)const { throw Image5D::Exception::Method_unimplemented; }
		//OB5专用，获取指定时点的起始指针，以及允许读写的字节数。此指针直接指向磁盘文件，相比于写出像素，可以减少一次拷贝。
		virtual void 像素指针(uint32_t T, void*& 指针, size_t& 容量)const { throw Image5D::Exception::Method_unimplemented; }
		//OB5专用，获取指定TZC位置的起始指针，以及允许读写的字节数。此指针直接指向磁盘文件，相比于写出像素，可以减少一次拷贝。
		virtual void 像素指针(uint32_t T, uint8_t Z, uint8_t C, void*& 指针, size_t& 容量)const { throw Image5D::Exception::Method_unimplemented; }
		static IOmeTiff读写器* 只读打开(LPCWSTR 文件路径);
		static IOmeTiff读写器* 读写打开(LPCWSTR 文件路径);
		static IOmeTiff读写器* 覆盖创建(LPCWSTR 文件路径, 像素类型 PixelType, uint16_t SizeX, uint16_t SizeY, uint8_t SizeC, uint8_t SizeZ, uint32_t SizeT, const 颜色* 通道颜色, 维度顺序 DimensionOrder);
		//将根据路径中的文件名修改图像描述中的文件名字段。
		static IOmeTiff读写器* 覆盖创建(LPCWSTR 文件路径, const char* 图像描述);
		virtual ~IOmeTiff读写器() {}
	};
}