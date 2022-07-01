#pragma once
#include<pugixml.hpp>
#include<stdint.h>
#include"OME概念定义.h"
#include <string.h>
constexpr const char* 维度顺序字符串[] = { "XYCZT","XYCTZ","XYZCT","XYZTC","XYTCZ","XYTZC" };
constexpr const char* 像素类型字符串[] = { "uint8","uint16","uint32","int8","int16","int32","float","double" };
template <UINT8 范围长度>
INT8 寻找字符串(const char* 目标, const char* const(&范围)[范围长度])
{
	for (UINT8 位置 = 0; 位置 < 范围长度; ++位置)
		if (!strcmp(目标, 范围[位置]))
			return 位置;
	return -1;
}
using namespace pugi;
void OmeXml基本解析(xml_node Pixels, uint8_t& iSizeC, uint8_t& iSizeZ, uint16_t& iSizeT, 维度顺序& iDimensionOrder, 像素类型& iPixelType, 颜色数组& iChannelColors);