#pragma once
#include "Tiff属性读入器.h"
#include "Tiff标准.h"
#include <vector>
class Tiff读入器 :public Tiff属性读入器
{
protected:
	using IFD数组 = std::vector<const char*>;
	IFD数组 IFD像素指针;
	//必须初始化，以免随机指针
	Tiff读入器(文件指针& 文件, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, std::string& 图像描述, IFD数组& IFD像素指针) :Tiff属性读入器(文件, iPixelType, iSizeX, iSizeY, IFD像素指针.size(), 图像描述), IFD像素指针(std::move(IFD像素指针)) {}
public:
	void 读入像素(char* 缓冲区)const override;
	void 读入像素(char* 缓冲区, UINT32 IStart, UINT32 ISize)const override;
	virtual ~Tiff读入器() {}
	template<Tiff版本 V>
	static Tiff读入器* 只读打开(文件指针& 文件);
};