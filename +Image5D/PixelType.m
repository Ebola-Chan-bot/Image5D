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
	methods
		function MT=MatlabType(obj)
			%将像素类型转换为MATLAB数据类型
			%# 语法
			% ```
			% MT=obj.MatlabType;
			% ```
			%# 示例
			% 此方法常用于初始化用于保存像素值的MATLAB数组：
			% ```
			% Reader=Image5D.OmeTiffRWer.OpenRead('T.tif');
			% ImageSample=zeros(Reader.SizeX,Reader.SizeY,Reader.SizeC,Reader.SizeZ,Reader.SizeT,Reader.PixelType.MatlabType);
			% ```
			%# 返回值
			% MT(1,:)char，MATLAB数据类型字符数组，如'uint8''single'等。PixelType.DEFAULT不能转换为MATLAB数据类型。
			switch obj
				case Image5D.PixelType.FLOAT
					MT='single';
				case Image5D.PixelType.DEFAULT
					Image5D.Exception.DEFAULT_PixelType_cannot_convert_to_MATLAB_datatypes.Throw;
				otherwise
					MT=lower(char(obj));
			end
		end
	end
end