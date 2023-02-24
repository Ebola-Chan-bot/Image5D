#pragma once
#include "Tiff属性读入器.h"
#include "Tiff标准.h"
#include <vector>
namespace Image5D
{
	using IFD数组 = std::vector<const void*>;
	class Tiff读入器 :public Tiff属性读入器
	{
	protected:
		IFD数组 IFD像素指针;
		void 像素拷贝(void* 写出头, const void* 读入头, uint32_t 帧数)const;
		bool 字节反转;
		//必须初始化，以免随机指针
	public:
		void 读入像素(void* 缓冲区)const override;
		void 读入像素I(void* 缓冲区, uint32_t IStart, uint32_t ISize)const override;
		virtual ~Tiff读入器() {}
		static Tiff读入器* 只读打开(文件指针&& 文件);
		Tiff读入器(文件指针&& 文件, 像素类型 iPixelType, uint16_t iSizeX, uint16_t iSizeY, std::string&& 图像描述, IFD数组&& IFD像素指针, bool 字节反转) :Tiff属性读入器(std::move(文件), iPixelType, iSizeX, iSizeY, IFD像素指针.size(), std::move(图像描述)), IFD像素指针(std::move(IFD像素指针)), 字节反转(字节反转&& iSizeP > 1) {}
	};
}