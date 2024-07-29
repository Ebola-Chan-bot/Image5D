#include "OmeXml基本解析.h"
#include "Image5D异常.h"
#include <vector>
void Image5D::OmeXml基本解析(xml_node Pixels, uint8_t& iSizeC, uint8_t& iSizeZ, uint32_t& iSizeT, 维度顺序& iDimensionOrder, 像素类型& iPixelType, 颜色数组& iChannelColors)
{
	xml_attribute 属性 = Pixels.attribute("SizeC");
	if (!属性)
		throw Exception::Missing_SizeC_attribute;
	if (!(iSizeC = 属性.as_uint()))
		throw Exception::SizeC_is_0;
	if (!(属性 = Pixels.attribute("SizeZ")))
		throw Exception::Missing_SizeZ_attribute;
	if (!(iSizeZ = 属性.as_uint()))
		throw Exception::SizeZ_is_0;
	if (!(属性 = Pixels.attribute("SizeT")))
		throw Exception::Missing_SizeT_attribute;
	if (!(iSizeT = 属性.as_uint()))
		throw Exception::Missing_SizeT_attribute;
	if (!(属性 = Pixels.attribute("DimensionOrder")))
		throw Exception::The_DimensionOrder_attribute_is_missing;
	INT8 序号 = 寻找字符串(属性.as_string(), 维度顺序字符串);
	if (序号 < 0)
		throw Exception::The_dimensional_order_is_invalid;
	iDimensionOrder = (维度顺序)序号;
	if (!(属性 = Pixels.attribute("Type")))
		throw Exception::Missing_the_Type_attribute;
	序号 = 寻找字符串(属性.as_string(), 像素类型字符串);
	if (序号 < 0)
		throw Exception::The_pixel_type_is_invalid;
	iPixelType = (像素类型)序号;
	std::vector<xml_node>ChannelBuffer;
	for (xml_node 节点 : Pixels.children("Channel"))
		ChannelBuffer.push_back(节点);
	if (ChannelBuffer.size() != iSizeC)
		throw Exception::The_color_number_does_not_match_SizeC;
	iChannelColors.reset((颜色*)malloc(sizeof(颜色) * iSizeC));
	for (UINT8 C = 0; C < iSizeC; ++C)
		iChannelColors[C].整数值 = (属性 = ChannelBuffer[C].attribute("Color")) ? 属性.as_int() : -1;
}