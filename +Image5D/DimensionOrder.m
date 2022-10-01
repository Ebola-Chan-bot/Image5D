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
	methods
		function SP=SizePermute(obj)
			%取得该维度顺序的重排序数
			%OmeTiffRWer的标准维度顺序是XYCZT。对于其它维度顺序，取各维size时需要进行重排。例如：
			%```
			% Sizes=size(Image,DimensionOrder.SizePermute);
			%```
			%本函数返回维度顺序相对于XYCZT的重排序数，可直接用于size等顺序索引操作。
			%See also Image5D.OmeTiffRWer.Create size
			persistent Permutes
			if isempty(Permutes)
				Permutes=[1,2,3,4,5;1,2,3,5,4;1,2,4,3,5;1,2,4,5,3;1,2,5,3,4;1,2,5,4,3];
			end
			SP=Permutes(uint8(obj)+1,:);
		end
	end
end