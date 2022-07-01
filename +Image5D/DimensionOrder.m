 classdef DimensionOrder<uint8
	%OME TIFF 规范定义了6种维度顺序，再加上一种DEFAULT表示缺省值
	enumeration
		XYCZT(0)
		XYCTZ(1)
		XYZCT(2)
		XYZTC(3)
		XYTCZ(4)
		XYTZC(5)
		%此项仅在OmeTiffRWer.ModifyParameters方法中使用，表示不修改维度顺序。
		DEFAULT(6)
	end
end