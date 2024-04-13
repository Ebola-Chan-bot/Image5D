classdef CommonReader<handle
	%通用格式读入器，可用于读入tif或oir图像文件
	%此对象不能直接构造。使用静态方法Open获取对象。
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
			%打开文件
			%支持tif和oir扩展名，根据扩展名自动选择OmeTiffRWer或OirReader实现读入器
			%# 语法
			% ```
			% obj=Image5D.CommonReader.Open(FilePath);
			% ```
			%# 输入参数
			% FilePath(1,1)string，要打开的文件路径。可以是tif或oir扩展名。OIR序列只需要输入头文件路径。
			%# 返回值
			% obj(1,1)CommonReader，通用读入器。根据文件扩展名，也可以当作OirReader或OmeTiffRWer使用。
			%See also Image5D.OirReader Image5D.OmeTiffRWer
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
	methods(Abstract)
		%读入指定范围内的像素值。
		%# 语法
		% ```
		% Pixels=obj.ReadPixels;
		% %读入所有像素值
		%
		% Pixels=obj.ReadPixels(TStart,TSize);
		% %读入指定时间范围内的像素值
		%
		% Pixels=obj.ReadPixels(TStart,TSize,ZStart,ZSize,CStart,CSize);
		% %读入指定范围内的像素值
		% ```
		%# 示例
		% ```
		% Pixels=obj.ReadPixelsT(0,obj.SizeT,0,obj.SizeZ,0,obj.SizeC);
		% %读入全部像素值。注意，指定维度参数的顺序永远为TZC，无论图像实际的维度顺序为何。
		%
		% imshow(Pixels(:,:,1,1,1)',[]);
		% %显示首帧。注意imshow假定维度顺序为YX，而返回像素值维度顺序为XY，所以需要转置。此外imshow一般只接受uint8或浮点类型像素值，需要加一个[]参数才能正常显示
		%  除此之外的像素值类型。
		% %注意，Pixels的维度顺序取决于DimensionOrder属性
		% ```
		%# 输入参数
		% TStart(1,1)uint32，要读入的起始时点，从0开始
		% TSize(1,1)uint32，要读入的时点个数
		% ZStart(1,1)uint8，要读入的起始Z层，从0开始
		% ZSize(1,1)uint8，要读入的Z层个数
		% CStart(1,1)uint8，要读入的起始通道，从0开始
		% CSize(1,1)uint8，要读入的通道个数
		%# 返回值
		% Pixels(:,:,:,:,:)，像素值，数据类型取决于PixelType属性，维度顺序取决于DimensionOrder属性
		Pixels=ReadPixels(obj,TStart,TSize,varargin)
	end
end