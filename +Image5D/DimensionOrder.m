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
	methods(Static)
		function PO=PermuteOrder(From,To)
			%计算从From维度顺序转换到To维度顺序的重排序数
			%# 语法
			% ```
			% PO=Image5D.DimensionOrder.PermuteOrder(From,To);
			% ```
			%# 示例
			% ```
			% %假设Reader是一个Image5D.CommonReader，具有不确定的维度顺序，需要将其标准化为XYCZT顺序
			% Video=Reader.ReadPixels(0,100,0,2,0,2);
			% %参数顺序始终是TZC，无论Reader的维度顺序为何
			%
			% %将读取的数据重排序为XYCZT顺序
			% Video=permute(Video,Image5D.DimensionOrder.PermuteOrder(Reader.DimensionOrder,Image5D.DimensionOrder.XYCZT));
			% ```
			%# 输入参数
			% From(1,1)Image5D.DimensionOrder，源维度顺序
			% To(1,1)Image5D.DimensionOrder，目标维度顺序
			%See also permute Image5D.CommonReader
			[~,PO]=ismember(char(To),char(From));
		end
	end
	methods
		function SP=SizePermute(obj)
			%取得该维度顺序的重排序数
			% 此方法已弃用，将在未来版本删除，请改用PermuteOrder方法。
			%OmeTiffRWer的标准维度顺序是XYCZT。对于其它维度顺序，取各维size时需要进行重排。例如：
			%```
			% Sizes=size(Image,DimensionOrder.SizePermute);
			%```
			%本函数返回维度顺序相对于XYCZT的重排序数，可直接用于size等顺序索引操作。
			%See also Image5D.OmeTiffRWer.Create size Image5D.DimensionOrder.PermuteOrder
			Image5D.Exception.Method_deprecated.Warn('SizePermute将在未来版本删除，请改用<a href="matlab:helpwin Image5D.DimensionOrder.PermuteOrder">PermuteOrder</a>方法。');
			persistent Permutes
			if isempty(Permutes)
				Permutes=[1,2,3,4,5;1,2,3,5,4;1,2,4,3,5;1,2,4,5,3;1,2,5,3,4;1,2,5,4,3];
			end
			SP=Permutes(uint8(obj)+1,:);
		end
	end
end