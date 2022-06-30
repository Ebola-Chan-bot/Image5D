#pragma once
#include "Tiff属性读入器.h"
#include "Tiff版本.h"
class Tiff读入器 :public Tiff属性读入器
{
protected:
	using IFD数组 = std::unique_ptr<const char* [], free删除器>;
	const IFD数组 IFD像素指针;
	//必须初始化，以免随机指针
	Tiff读入器(文件指针&文件, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, 文本数组&图像描述, IFD数组&IFD像素指针) :Tiff属性读入器(文件, iPixelType, iSizeX, iSizeY, iSizeI, 图像描述), IFD像素指针(std::move(IFD像素指针)) {}
public:
	void 读入像素(char* 缓冲区, UINT32 IStart, UINT32 ISize)const override;
	virtual ~Tiff读入器() {}
	template<版本 V>
	static const Tiff读入器* 只读打开(文件指针&文件);
};