#include "Tiff读入器.h"
#include "OmeBigTiff5D.h"
using namespace Image5D;
IOmeTiff读写器* IOmeTiff读写器::只读打开(LPCWSTR 文件路径)
{
	文件指针 映射 = std::make_unique<文件映射>(文件路径, true);
	映射->映射指针(nullptr);
	try { return OmeBigTiff5D::只读打开(std::move(映射)); }
	catch (const Image5D异常&) {}
	return Tiff读入器::只读打开<基本>(映射);
}

//以下实现引用了OmeBigTiff5D，不能放在头文件中，否则会造成循环包含
IOmeTiff读写器* IOmeTiff读写器::读写打开(LPCWSTR 文件路径) { return OmeBigTiff5D::读写打开(std::make_unique<文件映射>(文件路径, false)); }
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(LPCWSTR 文件路径, 像素类型 PixelType, uint16_t SizeX, uint16_t SizeY, uint8_t SizeC, uint8_t SizeZ, uint32_t SizeT, const 颜色* 通道颜色, 维度顺序 DimensionOrder) { return OmeBigTiff5D::覆盖创建(文件路径, PixelType, SizeX, SizeY, SizeC, SizeZ, SizeT, 通道颜色, DimensionOrder); }
IOmeTiff读写器* IOmeTiff读写器::覆盖创建(LPCWSTR 文件路径, const char* 图像描述) { return OmeBigTiff5D::覆盖创建(文件路径, 图像描述); }