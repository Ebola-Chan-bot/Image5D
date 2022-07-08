#include "pch.h"
#include "Tiff属性读入器.h"
#include "像素类型尺寸.h"
#include <vector>
Tiff属性读入器::Tiff属性读入器(文件指针& 文件, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, std::string& 图像描述) :文件(std::move(文件)), iPixelType(iPixelType), iSizeX(iSizeX), iSizeY(iSizeY), iSizeI(iSizeI), i图像描述(std::move(图像描述)), iSizeP(像素类型尺寸[UINT8(iPixelType)]), SizePXY(UINT32(iSizeP)* iSizeX* iSizeY), SizeIPXY(size_t(iSizeI)* SizePXY) {}