#pragma once
#include "IOmeTiff读写器.h"
class Ome属性读入器 :virtual public IOmeTiff读写器
{
protected:
	UINT8 iSizeC;
	UINT8 iSizeZ;
	UINT16 iSizeT;
	维度顺序 iDimensionOrder;
	颜色数组 i通道颜色;
	Ome属性读入器(UINT8 SizeC, UINT8 SizeZ, UINT16 SizeT, 维度顺序 DimensionOrder, 颜色数组& 通道颜色) :iSizeC(SizeC), iSizeZ(SizeZ), iSizeT(SizeT), iDimensionOrder(DimensionOrder), i通道颜色(std::move(通道颜色)) {}
public:
	UINT8 SizeC()const override { return iSizeC; }
	UINT8 SizeZ()const override { return iSizeZ; }
	UINT16 SizeT()const override { return iSizeT; }
	维度顺序 DimensionOrder()const override { return iDimensionOrder; }
	const 颜色* 通道颜色()const override { return i通道颜色.get(); }
	virtual ~Ome属性读入器() {}
};