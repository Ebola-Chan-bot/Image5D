#include "pch.h"
#include "Tiff属性读入器.h"
#include <stdlib.h>
#include "像素类型尺寸.h"
Tiff属性读入器::Tiff属性读入器(HANDLE 文件句柄, HANDLE 映射句柄, LPVOID 映射指针, LPVOID 尾指针, 像素类型 iPixelType, UINT16 iSizeX, UINT16 iSizeY, UINT32 iSizeI, char* 图像描述) :文件句柄(文件句柄), 映射句柄(映射句柄), 映射指针(映射指针), 尾指针(尾指针), iPixelType(iPixelType), iSizeX(iSizeX), iSizeY(iSizeY), iSizeI(iSizeI), i图像描述(图像描述), iSizeP(像素类型尺寸[(UINT8)iPixelType]), SizePXY(UINT32(iSizeP)* iSizeX* iSizeY) {}
Tiff属性读入器::~Tiff属性读入器()
{
	free(i图像描述);
	UnmapViewOfFile(映射指针);
	CloseHandle(映射句柄);
	CloseHandle(文件句柄);
}