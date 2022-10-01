classdef OirReader<handle
	%Olympus OIR 文件快速5D读入器，使用连续内存映射文件实现高速读入
    properties(GetAccess=private,SetAccess=immutable,Transient)
		%不同于C++，MATLAB类即使构造出错也会析构，所以必须先置0
		Pointer(1,1)uint64=0
	end
	properties(Dependent)
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
		%每个采样时点的间隔毫秒数
		SeriesInterval
	end
    methods
		function obj=OirReader(HeaderPath)
			%构造OirReader对象
			%# 语法
			% ```
			% obj=Image5D.OirReader(HeaderPath);
			% ```
			%# 输入参数
			% HeaderPath(1,1)string，头文件路径。OIR文件序列中只有头文件具有扩展名.oir，后随文件无扩展名。只需提供此头文件路径，程序自动检测后随文件。
			%# 提示
			% OIR文件的像素值是在文件中间断存储的数据块，且没有内置索引。为了实现快速读入，首次构造时需要遍历文件建立索引。遍历算法尽可能优化过，但对于大Oir文件仍可能需要等待相当长时间。
			% 建立的索引需要保存在Oir文件同目录下，这样下次可以快速打开，无需重新建立索引。但也因此要求该目录具有写入权限，暂不支持打开只读目录下的Oir文件。
			obj.Pointer=Image5D.internal.OirReaderAPI.CreateOirReader.Call(HeaderPath);
		end
		function delete(obj)
			%删除OirReader对象。
			%删除OirReader对象变量时会自动关闭相关文件，无需手动操作。
			Image5D.internal.OirReaderAPI.DestroyOirReader.Call(obj.Pointer);
		end
		function Size=get.SizeX(obj)
			Size=Image5D.internal.OirReaderAPI.SizeX.Call(obj.Pointer);
		end
		function Size=get.SizeY(obj)
			Size=Image5D.internal.OirReaderAPI.SizeY.Call(obj.Pointer);
		end
		function Size=get.SizeC(obj)
			Size=Image5D.internal.OirReaderAPI.SizeC.Call(obj.Pointer);
		end
		function Size=get.SizeZ(obj)
			Size=Image5D.internal.OirReaderAPI.SizeZ.Call(obj.Pointer);
		end
		function Size=get.SizeT(obj)
			Size=Image5D.internal.OirReaderAPI.SizeT.Call(obj.Pointer);
		end
		function SI=get.SeriesInterval(obj)
			SI=Image5D.internal.OirReaderAPI.SeriesInterval.Call(obj.Pointer);
		end
		function [Devices,Colors]=DeviceColors(obj)
			%取各通道的采样设备和颜色信息
			%# 语法
			% ```
			% [Devices,Colors]=obj.DeviceColors;
			% ```
			%# 返回值
			% Devices(:,1)string，依次返回每个通道的采样设备名称
			% Colors(3,:)single，各通道颜色。第1维依次排列红、绿、蓝色分量，数值范围0~1；第2维排列不同的通道.
			[Devices,Colors]=Image5D.internal.OirReaderAPI.DeviceColors.Call(obj.Pointer);
		end
		function Pixels=ReadPixels(obj,TStart,TSize,varargin)
			%读入像素块值
			%# 语法
			% ```
			% Pixels=obj.ReadPixels;
			% %读入全部像素
			%
			% Pixels=obj.ReadPixels(TStart,TSize);
			% %读入一段时间内的所有像素
			%
			% Pixels=obj.ReadPixels(TStart,TSize,C);
			% %读入特定通道设备在一段时间内采得的像素
			%
			% Pixels=obj.ReadPixels(TStart,TSize,ZStart,ZSize);
			% %读入一段时间和Z层内采得的像素
			%
			% Pixels=obj.ReadPixels(TStart,TSize,ZStart,ZSize,CStart,CSize);
			% %读入一定时间、Z层、通道范围内的像素
			% ```
			%# 示例
			% ```
			% %读入全部像素
			% Pixels=obj.ReadPixels(0,obj.SizeT);
			% %显示第1帧，即C=1, Z=1, T=1
			% imshow(Pixels(:,:,1,1,1)',[]);
			% %注意imshow会将第2维而不是第1维作为X轴，因此需要转置；此外Pixels是uint16类型，需要加上[]参数才能正常显示。
			% ```
			%# 输入参数
			% C(1,1)uint8，通道序号，从0开始
			% TStart(1,1)uint16，起始采样时间，从0开始
			% TSize(1,1)uint16，要读入的像素块时长
			% ZStart(1,1)uint8，起始Z层，从0开始
			% ZSize(1,1)uint8，要读入的像素块Z层数
			% CStart(1,1)uint8，起始通道号，从0开始
			% CSize(1,1)uint8，要读入的像素块通道数
			%# 返回值
			% Pixels(:,:,:,:,:)uint16，像素块，维度顺序XYCZT。注意，MATLAB imshow imwrite 等图像处理函数默认维度顺序是YXC，与文件中不同，可能需要permute。
			switch nargin
				case 1
					Pixels=Image5D.internal.OirReaderAPI.ReadPixels.Call(obj.Pointer);
				case {3,4,5,7}
					varargin=cellfun(@uint8,varargin,UniformOutput=false);
					Pixels=Image5D.internal.OirReaderAPI.ReadPixels.Call(obj.Pointer,uint16(TStart),uint16(TSize),varargin{:});
				otherwise
					Image5D.Image5DException.Wrong_number_of_parameters.Throw;
			end
		end
		function ReadToPointer(obj,Pointer,TStart,TSize,varargin)
			%读入像素块值到给定指针
			%为了避免MATLAB中介数组造成额外的拷贝开销，此方法允许直接将像素值拷贝到指定C++内存指针。
			%# 语法
			% ```
			% obj.ReadToPointer(Pointer);
			% %读入全部像素
			%
			% obj.ReadToPointer(Pointer,TStart,TSize);
			% %读入一段时间内的所有像素
			%
			% obj.ReadToPointer(Pointer,TStart,TSize,C);
			% %读入特定通道设备在一段时间内采得的像素
			%
			% obj.ReadToPointer(Pointer,TStart,TSize,ZStart,ZSize);
			% %读入一段时间和Z层内采得的像素
			%
			% obj.ReadToPointer(Pointer,TStart,TSize,ZStart,ZSize,CStart,CSize);
			% %读入一定时间、Z层、通道范围内的像素
			% ```
			%# 示例
			% ```
			% import Image5D.*
			% %此方法常用于格式转换，例如在OIR与TIFF之间进行直接像素对拷
			% [~,Colors]=obj.DeviceColors;
			% Colors(4,end)=0;
			% Writer=OmeTiffRWer.Create('图像.tif',PixelType.UINT16,obj.SizeX,obj.SizeY,ChannelColor.New(flipud(Colors).Call(obj.SizeZ,obj.SizeT,DimensionOrder.XYCZT);
			% obj.ReadToPointer(Writer.PixelPointer);
			% %上例新建了一个和OIR尺寸相同的TIFF图像，然后取得像素指针，令OIR读入器直接读入该指针，实现两文件直接对拷，而不经过MATLAB数组中介，性能较高。
			% ```
			%# 输入参数
			% Pointer(1,1)Image5D.SafePointer，C++内存指针，应有足够Capacity存储读入的像素值
			% TStart(1,1)uint16，起始采样时间，从0开始
			% TSize(1,1)uint16，要读入的像素块时长
			% C(1,1)uint8，通道序号，从0开始
			% ZStart(1,1)uint8，起始Z层，从0开始
			% ZSize(1,1)uint8，要读入的像素块Z层数
			% CStart(1,1)uint8，起始通道号，从0开始
			% CSize(1,1)uint8，要读入的像素块通道数
			%See also Image5D.SafePointer Image5D.OmeTiffRWer.PixelPointer Image5D.OirReader.ReadPixels 
			switch nargin
				case 2
					Image5D.internal.OirReaderAPI.ReadToPointer.Call(obj.Pointer,Pointer.Pointer,Pointer.Capacity);
				case {4,5,6,8}
					varargin=cellfun(@uint8,varargin,UniformOutput=false);
					Image5D.internal.OirReaderAPI.ReadToPointer.Call(obj.Pointer,Pointer.Pointer,Pointer.Capacity,uint16(TStart),uint16(TSize),varargin{:});
				otherwise
					Image5D.Image5DException.Wrong_number_of_parameters.Throw;
			end
		end
	end
end