#include <vector>
#include "Image5D异常.h"
#include "OmeXml基本解析.h"
void Image5D::OmeXml基本解析(xml_node Pixels, uint8_t& iSizeC, uint8_t& iSizeZ, uint32_t& iSizeT, 维度顺序& iDimensionOrder, 像素类型& iPixelType, 颜色数组& iChannelColors)
{
	xml_attribute 属性 = Pixels.attribute("SizeC");
	if (!属性)
		throw Image5D异常(缺少SizeC属性);
	if (!(iSizeC = 属性.as_uint()))
		throw Image5D异常(SizeC为0);
	if (!(属性 = Pixels.attribute("SizeZ")))
		throw Image5D异常(缺少SizeZ属性);
	if (!(iSizeZ = 属性.as_uint()))
		throw Image5D异常(SizeZ为0);
	if (!(属性 = Pixels.attribute("SizeT")))
		throw Image5D异常(缺少SizeT属性);
	if (!(iSizeT = 属性.as_uint()))
		throw Image5D异常(SizeT为0);
	if (!(属性 = Pixels.attribute("DimensionOrder")))
		throw Image5D异常(缺少DimensionOrder属性);
	INT8 序号 = 寻找字符串(属性.as_string(), 维度顺序字符串);
	if (序号 < 0)
		throw Image5D异常(维度顺序无效);
	iDimensionOrder = (维度顺序)序号;
	if (!(属性 = Pixels.attribute("Type")))
		throw Image5D异常(缺少Type属性);
	序号 = 寻找字符串(属性.as_string(), 像素类型字符串);
	if (序号 < 0)
		throw Image5D异常(像素类型无效);
	iPixelType = (像素类型)序号;
	std::vector<xml_node>ChannelBuffer;
	for (xml_node 节点 : Pixels.children("Channel"))
		ChannelBuffer.push_back(节点);
	if (ChannelBuffer.size() != iSizeC)
		throw Image5D异常(颜色数与SizeC不匹配);
	iChannelColors.reset((颜色*)malloc(sizeof(颜色) * iSizeC));
	for (UINT8 C = 0; C < iSizeC; ++C)
		iChannelColors[C].整数值 = (属性 = ChannelBuffer[C].attribute("Color")) ? 属性.as_int() : -1;
}