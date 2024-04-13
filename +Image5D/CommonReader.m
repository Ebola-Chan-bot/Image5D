classdef CommonReader<handle
    properties(GetAccess=protected,SetAccess=immutable,Transient)
		%不同于C++，MATLAB类即使构造出错也会析构，所以必须先置0
		Pointer(1,1)uint64=0
	end
	properties(Dependent,Abstract)
		%图像宽度
		SizeX
		%图像高度
		SizeY
		%颜色通道数
		SizeC
		%Z层数
		SizeZ
		%采样时点数
		SizeT
		%像素字节数
		SizeP
		%像素类型
		PixelType
		%各通道颜色
		ChannelColors
		%维度顺序
		DimensionOrder
	end
	methods(Access=protected)
		function obj = CommonReader(Pointer)
			obj.Pointer=Pointer;
		end
	end
	methods(Static)
		function obj=Open(FilePath)
			[~,~,Extension]=fileparts(FilePath);
			switch Extension
				case ".oir"
					obj=Image5D.OirReader(FilePath);
				case ".tif"
					obj=Image5D.OmeTiffRWer.OpenRead(FilePath);
				otherwise
					Image5D.Exceptions.Unexpected_file_extension.Throw;
			end
		end
	end
end

