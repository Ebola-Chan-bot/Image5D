#pragma once
#include <Image5D.h>
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
constexpr Image5D异常 未实现{ .类型 = 未实现的方法 };
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

	virtual Image5D异常 PixelType(像素类型) { return 未实现; }
	virtual Image5D异常 SizeX(UINT16) { return 未实现; }
	virtual Image5D异常 SizeY(UINT16) { return 未实现; }
	virtual Image5D异常 SizeI(UINT32) { return 未实现; }
	virtual Image5D异常 SizeC(UINT8) { return 未实现; }
	virtual Image5D异常 SizeZ(UINT8) { return 未实现; }
	virtual Image5D异常 SizeT(UINT16) { return 未实现; }
	//如无异常，读写器将取得所有权并负责释放；如有异常，所有权返还给调用方。
	virtual Image5D异常 通道颜色(颜色*) { return 未实现; }
	virtual Image5D异常 DimensionOrder(维度顺序) { return 未实现; }
	/*
	如无异常，读写器取得所有权并负责释放；如有异常，所有权返还给调用方。
	图像描述中的文件名字段将覆盖对象中的文件名属性，而不会修改图像描述
	*/
	virtual Image5D异常 图像描述(char*) { return 未实现; }
	virtual Image5D异常 文件名(const char*) { return 未实现; }
	/*
	请使用本方法同时修改多个参数，不要逐一调用每个参数的修改方法，那样会导致多次不必要的文件重构。每个参数自己的修改方法，本质上也是调用了本方法实现的，并没有专门的优化。
	如无异常，读写器取得通道颜色所有权并负责释放；如有异常，所有权返还给调用方。
	0、nullptr、缺省值表示不修改该参数。注意，枚举类参数必需设为缺省以表示不做修改：缺省值不是0。这个规则同样适用于每个参数自己的修改方法。
	*/
	virtual Image5D异常 修改参数(像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 颜色* 通道颜色, 维度顺序 DimensionOrder, const char* 文件名) { return 未实现; }
	virtual Image5D异常 读入像素(char* 缓冲区, UINT32 IStart, UINT32 ISize)const { return 未实现; }
	virtual Image5D异常 读入像素(char* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const { return 未实现; }
	virtual Image5D异常 读入像素(char* 缓冲区, UINT16 TStart, UINT16 TSize)const { return 未实现; }
	virtual Image5D异常 写出像素(const char* 缓冲区, UINT32 IStart, UINT32 ISize)const { return 未实现; }
	virtual Image5D异常 写出像素(const char* 缓冲区, UINT16 TStart, UINT16 TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const { return 未实现; }
	virtual Image5D异常 写出像素(const char* 缓冲区, UINT16 TStart, UINT16 TSize)const { return 未实现; }
	virtual char* 像素指针(UINT32 I)const { throw 未实现; }
	virtual char* 像素指针(UINT16 T, UINT8 Z, UINT8 C)const { throw 未实现; }
	static const IOmeTiff读写器* 只读打开(LPCSTR 文件路径);
	static const IOmeTiff读写器* 只读打开(LPCWSTR 文件路径);
	static IOmeTiff读写器* 读写打开(LPCSTR 文件路径);
	static IOmeTiff读写器* 读写打开(LPCWSTR 文件路径);
	//如无异常，读写器将取得颜色指针所有权并负责释放；如有异常，所有权返还给调用方。
	static IOmeTiff读写器* 覆盖创建(LPCSTR 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 颜色* 通道颜色, 维度顺序 DimensionOrder);
	//如无异常，读写器将取得颜色指针所有权并负责释放；如有异常，所有权返还给调用方。
	static IOmeTiff读写器* 覆盖创建(LPCWSTR 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 颜色* 通道颜色, 维度顺序 DimensionOrder);
	/*
	如无异常，读写器将取得图像描述指针所有权并负责释放；如有异常，所有权返还给调用方。
	将根据路径中的文件名修改图像描述中的文件名字段。
	*/
	static IOmeTiff读写器* 覆盖创建(LPCSTR 文件路径, char* 图像描述);
	/*
	如无异常，读写器将取得图像描述指针所有权并负责释放；如有异常，所有权返还给调用方。
	将根据路径中的文件名修改图像描述中的文件名字段。如有异常，则不会修改。
	*/
	static IOmeTiff读写器* 覆盖创建(LPCWSTR 文件路径, char* 图像描述);

	virtual ~IOmeTiff读写器() {}
};