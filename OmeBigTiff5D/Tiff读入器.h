#pragma once
#include "Tiff属性读入器.h"
#include "Tiff版本.h"
class Tiff读入器 :public Tiff属性读入器
{
protected:
	const char** const IFD像素指针;
	//必须初始化，以免随机指针
	Tiff读入器(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, char* 图像描述, const char** const IFD像素指针) :Tiff属性读入器(文件句柄, 映射句柄, 映射指针, 尾指针, iPixelType, iSizeX, iSizeY, iSizeI, 图像描述), IFD像素指针(IFD像素指针) {}
public:
	Image5D异常 读入像素(char* 缓冲区, UINT32 IStart, UINT32 ISize)const override;
	virtual ~Tiff读入器();
	template<版本 V>
	static const Tiff读入器* 只读打开(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针);
};