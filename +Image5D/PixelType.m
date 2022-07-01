classdef PixelType<uint8
	%OME TIFF 规范定义的8种像素类型，加上DEFAULT表示缺省值
	enumeration
		UINT8(0)
		UINT16(1)
		UINT32(2)
		INT8(3)
		INT16(4)
		INT32(5)
		FLOAT(6)
		DOUBLE(7)
		%此项仅在OmeTiffRWer.ModifyParameters方法中使用，表示不修改像素类型。
		DEFAULT(8)
	end
end