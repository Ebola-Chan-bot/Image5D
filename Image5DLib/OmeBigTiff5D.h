#pragma once
#include "Tiff属性读入器.h"
#include "Ome属性读入器.h"
#include <pugixml.hpp>
using namespace pugi;
namespace Image5D
{
	class OmeBigTiff5D :public Tiff属性读入器, public Ome属性读入器
	{
		//独立参数：静态构造输入并集

		const char* i文件名; //指向文档内，无需释放
		xml_node Pixels;

		//构造参数：非输入，但构造过程必需

		xml_document 图像描述文档;
		const char* 唯一标识符; //指向文档内，无需释放
		char* 像素头; //指向文件内，无需释放

		//预计算参数：无关构造，读写中频繁使用

		UINT64 源Size10;
		UINT64 源Size210;
		OmeBigTiff5D(文件指针& 文件, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, std::string& 图像描述, UINT8 SizeC, UINT8 SizeZ, uint32_t SizeT, 维度顺序 DimensionOrder, 颜色数组& 通道颜色, const char* 文件名, xml_node Pixels, xml_document& 图像描述文档, const char* 唯一标识符, char* 像素头);
	public:
		void PixelType(像素类型)override;
		void SizeX(UINT16)override;
		void SizeY(UINT16)override;
		void SizeC(UINT8)override;
		void SizeZ(UINT8)override;
		void SizeT(uint32_t)override;
		void 通道颜色(const 颜色*)override;
		void DimensionOrder(维度顺序)override;
		//图像描述中的文件名字段将覆盖对象中的文件名属性，而不会修改图像描述
		void 图像描述(std::string&&)override;
		const char* 文件名()const { return i文件名; }
		void 文件名(const char*);
		void 修改参数(像素类型 PT, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, uint32_t SizeT, const 颜色* 通道颜色, 维度顺序 DO, const char* 文件名)override;
		void 读入像素(void* 缓冲区)const override;
		void 读入像素I(void* 缓冲区, UINT32 IStart, UINT32 ISize)const override;
		void 读入像素(void* 缓冲区, uint32_t TStart, uint32_t TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const override;
		void 读入像素(void* 缓冲区, uint32_t TStart, uint32_t TSize)const override { return 读入像素(缓冲区, TStart, TSize, 0, iSizeZ, 0, iSizeC); }
		void 写出像素(const void* 缓冲区)const override;
		void 写出像素I(const void* 缓冲区, UINT32 IStart, UINT32 ISize)const override;
		void 写出像素(const void* 缓冲区, uint32_t TStart, uint32_t TSize, UINT8 ZStart, UINT8 ZSize, UINT8 CStart, UINT8 CSize)const override;
		void 写出像素(const void* 缓冲区, uint32_t TStart, uint32_t TSize)const override { return 写出像素(缓冲区, TStart, TSize, 0, iSizeZ, 0, iSizeC); }
		void 像素指针(void*& 指针, size_t& 容量)const override;
		void 像素指针I(UINT32 I, void*& 指针, size_t& 容量)const override;
		void 像素指针(uint32_t T, void*& 指针, size_t& 容量)const override;
		void 像素指针(uint32_t T, UINT8 Z, UINT8 C, void*& 指针, size_t& 容量)const override;
		static OmeBigTiff5D* 只读打开(文件指针&& 文件);
		static OmeBigTiff5D* 读写打开(文件指针&& 文件);
		static OmeBigTiff5D* 覆盖创建(LPCWSTR 文件路径, 像素类型 PixelType, UINT16 SizeX, UINT16 SizeY, UINT8 SizeC, UINT8 SizeZ, uint32_t SizeT, const 颜色* 通道颜色, 维度顺序 DimensionOrder);
		static OmeBigTiff5D* 覆盖创建(LPCWSTR 文件路径, const char* 图像描述);
		virtual ~OmeBigTiff5D() {}
	};
}