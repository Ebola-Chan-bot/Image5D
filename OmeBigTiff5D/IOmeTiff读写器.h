#pragma once
#include <string>
#include "OME概念定义.h"
constexpr Image5D异常 未实现(未实现的方法);
class IOmeTiff读写器
{
public:
	virtual 像素类型 PixelType()const { throw 未实现; }
	virtual UINT8 SizeP()const { throw 未实现; }
	virtual UINT16 SizeX()const { throw 未实现; }
	virtual UINT16 SizeY()const { throw 未实现; }
	virtual UINT32 SizeI()const { throw 未实现; }
	virtual const char* 图像描述()const { throw 未实现; }
	virtual UINT8 SizeC()const { throw 未实现; }
	virtual UINT8 SizeZ()const { throw 未实现; }
	virtual UINT16 SizeT()const { throw 未实现; }
	virtual 维度顺序 DimensionOrder()const { throw 未实现; }
	virtual const 颜色* 通道颜色()const { throw 未实现; }
	virtual const char* 文件名()const { throw 未实现; }

	virtual void PixelType(像素类型) { throw 未实现; }
	virtual void SizeX(UINT16) { throw 未实现; }
	virtual void SizeY(UINT16) { throw 未实现; }
	virtual void SizeI(UINT32) { throw 未实现; }
	virtual void SizeC(UINT8) { throw 未实现; }
	virtual void SizeZ(UINT8) { throw 未实现; }
	virtual void SizeT(UINT16) { throw 未实现; }
	//读写器无法检查输入颜色指针是否指向足够长的内存段（至少应有SizeC长），由调用方保证指针正确。
	virtual void 通道颜色(const 颜色*) { throw 未实现; }
	virtual void DimensionOrder(维度顺序) { throw 未实现; }
	//图像描述中的文件名字段将覆盖对象中的文件名属性，而不会修改图像描述
	virtual void 图像描述(std::string&&) { throw 未实现; }
	virtual void 文件名(const char*) { throw 未实现; }
	/*
	请使用本方法同时修改多个参数，不要逐一调用每个参数的修改方法，那样会导致多次不必要的文件重构。每个参数自己的修改方法，本质上也是调用了本方法实现的，并没有专门的优化。
	0、nullptr、缺省值表示不修改该参数。注意，枚举类参数必需设为缺省以表示不做修改：缺省值不是0。这个规则同样适用于每个参数自己的修改方法。
	读写器无法检查输入颜色指针是否指向足够长的内存段（至少应有SizeC长），由调用方保证指针正确。
	*/
	virtual void 修改参数(像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, const 颜色* 通道颜色, 维度顺序 DimensionOrder, const char* 文件名) { throw 未实现; }
	virtual void 读入像素(char* 缓冲区)const { throw 未实现; }
	virtual void 读入像素(char* 缓冲区, UINT32 IStart, UINT32 ISize)const { throw 未实现; }
	virtual void 读入像素(char* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const { throw 未实现; }
	virtual void 读入像素(char* 缓冲区, UINT16 TStart, UINT16 TSize)const { throw 未实现; }
	virtual void 写出像素(const char* 缓冲区)const { throw 未实现; }
	virtual void 写出像素(const char* 缓冲区, UINT32 IStart, UINT32 ISize)const { throw 未实现; }
	virtual void 写出像素(const char* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const { throw 未实现; }
	virtual void 写出像素(const char* 缓冲区, UINT16 TStart, UINT16 TSize)const { throw 未实现; }
	virtual void 像素指针(char*& 指针, size_t& 容量)const { throw 未实现; }
	virtual void 像素指针(UINT32 I, char*& 指针, size_t& 容量)const { throw 未实现; }
	virtual void 像素指针(UINT16 T, char*& 指针, size_t& 容量)const { throw 未实现; }
	virtual void 像素指针(UINT16 T, UINT8 Z, UINT8 C, char*& 指针, size_t& 容量)const { throw 未实现; }
	template <字符 T>
	static IOmeTiff读写器* 只读打开(const T* 文件路径);
	template<字符 T>
	static IOmeTiff读写器* 读写打开(const T* 文件路径);
	template<字符 T>
	static IOmeTiff读写器* 覆盖创建(const T* 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, const 颜色* 通道颜色, 维度顺序 DimensionOrder);
	//将根据路径中的文件名修改图像描述中的文件名字段。
	template<字符 T>
	static IOmeTiff读写器* 覆盖创建(const T* 文件路径, const char* 图像描述);
	virtual ~IOmeTiff读写器() {}
};