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
		%图像元数据XML
		LsmimageXml
		%各Z层的激光透过率
		LaserTransmissivity
		%PMT电压，第1维通道，第2维Z层
		PmtVoltage
		%采样设备名称和通道颜色
		%(:,2)table，每行一个设备通道，包含以下列：
		% - Device(:,1)string，设备名称
		% - Color(:,3)table，通道颜色，包含 Red Green Blue 三列
		DeviceColors
	end
	methods(Static)
		function ConcatenateByRename(HeaderPaths)
			%通过修改文件名的方法串联多个OIR文件序列
			%OIR文件序列是通过文件名识别的，因此理论上可以通过修改文件名将几个不同的序列串联到一起。但是这种
			% 方法存在如下限制：
			% - 这不是标准的OIR串联方法，串联后只有基于Image5D库的应用能够打开这样串联的文件序列，而
			%  FluoView或ImageJ等其它读入器可能无法查看这样的OIR序列。
			% - 不会检查不同序列之间是否具有匹配的XYCZ尺寸、设备、通道颜色、序列间隔等，全部以第一个OIR为准
			%  （包括文件名）。调用方应当保证这些元数据匹配，否则可能产生意外结果。
			%将直接执行重命名操作，不会复制文件内容。
			%# 语法
			% ```
			% Image5D.OirReader.ConcatenateByRename(HeaderPaths);
			% ```
			%# 输入参数
			% HeaderPaths(:,1)string，要合并的OIR头文件路径。所有元数据和文件名将以第1个文件为准，后续文件被
			%  重命名。如果这些文件不在同一目录下，所有后续文件将被移动到第1个文件同目录下。
			[Directory,Filename]=fileparts(HeaderPaths(1));
			NumFiles=1;
			UnextendedPath=fullfile(Directory,Filename);
			while isfile(UnextendedPath+sprintf("_%05u",NumFiles))
				NumFiles=NumFiles+1;
			end
			HeaderPaths(1)=[];
			NumHeaders=numel(HeaderPaths);
			Sources=MATLAB.Containers.Vector;
			Destinations=MATLAB.Containers.Vector;
			for H=1:NumHeaders
				Sources.PushBack(HeaderPaths(H));
				FollowerNF=1;
				[FollowerDirectory,FollowerFilename]=fileparts(HeaderPaths(H));
				FollowerUP=fullfile(FollowerDirectory,FollowerFilename);
				FollowerFollower=FollowerUP+sprintf("_%05u",FollowerNF);
				while isfile(FollowerFollower)
					Sources.PushBack(FollowerFollower);
					FollowerNF=FollowerNF+1;
					FollowerFollower=FollowerUP+sprintf("_%05u",FollowerNF);
				end
				FilesEnd=NumFiles+FollowerNF;
				Destinations.PushBack(UnextendedPath+compose("_%05u",NumFiles:FilesEnd-1));
				NumFiles=FilesEnd;
			end
			MATLAB.IO.MoveFile(Sources.Data,Destinations.Data);
		end
		function SplitByRename(HeaderPath,SplitTable)
			%如果使用ConcatenateByRename发生了错误的合并并且已经无法撤销，可以尝试用此方法重新拆分开来
			%# 语法
			% ```
			% SplitByRename(HeaderPath,SplitTable)
			% ```
			%# 输入参数
			% HeaderPath(1,1)string，已合并的文件序列的头文件路径
			% SplitTable table，拆分表。应当包含一个从小到大排序的Index列，指示每个拆分位点的索引，那个索
			%  引处的文件在拆分后将成为新序列的头文件。还包含一个HeaderName列，指示新序列的头文件名。
			[Directory,Filename]=fileparts(HeaderPath);
			UnextendedPath=fullfile(Directory,Filename);
			Sources=MATLAB.Containers.Vector;
			Destinations=MATLAB.Containers.Vector;
			for S=1:height(SplitTable)-1
				StartIndex=SplitTable.Index(S);
				FollowerUP=fullfile(Directory,SplitTable.HeaderName(S));
				Destinations.PushBack(FollowerUP+".oir");
				NumFollowers=SplitTable.Index(S+1)-StartIndex-1;
				Sources.PushBack(UnextendedPath+compose("_%05u",StartIndex:StartIndex+NumFollowers));
				Destinations.PushBack(FollowerUP+compose("_%05u",1:NumFollowers));
			end
			StartIndex=SplitTable.Index(end);
			FollowerUP=fullfile(Directory,SplitTable.HeaderName(end));
			Sources.PushBack(UnextendedPath+sprintf("_%05u",StartIndex));
			Destinations.PushBack(FollowerUP+".oir");
			NumFollowers=1;
			PathToTest=UnextendedPath+sprintf("_%05u",StartIndex+NumFollowers);
			while isfile(PathToTest)
				Sources.PushBack(PathToTest);
				NumFollowers=NumFollowers+1;
				PathToTest=UnextendedPath+sprintf("_%05u",StartIndex+NumFollowers);
			end
			Destinations.PushBack(FollowerUP+compose("_%05u",1:NumFollowers-1));
			MATLAB.IO.MoveFile(Sources.Data,Destinations.Data);
		end
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
			obj.Pointer=Image5D.internal.Image5DAPI.Oir_CreateReader.Call(HeaderPath);
		end
		function delete(obj)
			%删除OirReader对象。
			%删除OirReader对象变量时会自动关闭相关文件，无需手动操作。
			if obj.Pointer
				Image5D.internal.Image5DAPI.Oir_DeleteReader.Call(obj.Pointer);
			end
		end
		function Size=get.SizeX(obj)
			Size=Image5D.internal.Image5DAPI.Oir_SizeX.Call(obj.Pointer);
		end
		function Size=get.SizeY(obj)
			Size=Image5D.internal.Image5DAPI.Oir_SizeY.Call(obj.Pointer);
		end
		function Size=get.SizeC(obj)
			Size=Image5D.internal.Image5DAPI.Oir_SizeC.Call(obj.Pointer);
		end
		function Size=get.SizeZ(obj)
			Size=Image5D.internal.Image5DAPI.Oir_SizeZ.Call(obj.Pointer);
		end
		function Size=get.SizeT(obj)
			Size=Image5D.internal.Image5DAPI.Oir_SizeT.Call(obj.Pointer);
		end
		function SI=get.SeriesInterval(obj)
			SI=Image5D.internal.Image5DAPI.Oir_SeriesInterval.Call(obj.Pointer);
		end
		function LX=get.LsmimageXml(obj)
			LX=Image5D.internal.Image5DAPI.Oir_LsmimageXml.Call(obj.Pointer);
		end
		function LT=get.LaserTransmissivity(obj)
			LT=Image5D.internal.Image5DAPI.Oir_LaserTransmissivity.Call(obj.Pointer);
		end
		function PV=get.PmtVoltage(obj)
			PV=Image5D.internal.Image5DAPI.Oir_PmtVoltage.Call(obj.Pointer);
		end
		function DC=get.DeviceColors(obj)
			[Device,Color]=Image5D.internal.Image5DAPI.Oir_DeviceColors.Call(obj.Pointer);
			Color=array2table(Color,VariableNames=["Red","Green","Blue"]);
			DC=table(Device,Color);
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
			% TStart(1,1)uint32，起始采样时间，从0开始
			% TSize(1,1)uint32，要读入的像素块时长
			% ZStart(1,1)uint8，起始Z层，从0开始
			% ZSize(1,1)uint8，要读入的像素块Z层数
			% CStart(1,1)uint8，起始通道号，从0开始
			% CSize(1,1)uint8，要读入的像素块通道数
			%# 返回值
			% Pixels(:,:,:,:,:)uint16，像素块，维度顺序XYCZT。注意，MATLAB imshow imwrite 等图像处理函数默认维度顺序是YXC，与文件中不同，可能需要permute。
			switch nargin
				case 1
					Pixels=Image5D.internal.Image5DAPI.Oir_ReadPixels.Call(obj.Pointer);
				case {3,4,5,7}
					varargin=cellfun(@uint8,varargin,UniformOutput=false);
					Pixels=Image5D.internal.Image5DAPI.Oir_ReadPixels.Call(obj.Pointer,uint32(TStart),uint32(TSize),varargin{:});
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
			% Reader=OirReader('O.oir');
			% [~,Colors]=Reader.DeviceColors;
			% Writer=OmeTiffRWer.Create('T.tif',PixelType.UINT16,Reader.SizeX,Reader.SizeY,ChannelColor.FromOirColors(Colors),Reader.SizeZ,Reader.SizeT,DimensionOrder.XYCZT);
			% Reader.ReadToPointer(Writer.PixelPointer);
			% %上例新建了一个和OIR尺寸相同的TIFF图像，然后取得像素指针，令OIR读入器直接读入该指针，实现两文件直接对拷，而不经过MATLAB数组中介，性能较高。
			% ```
			%# 输入参数
			% Pointer(1,1)Image5D.SafePointer，C++内存指针，应有足够Capacity存储读入的像素值
			% TStart(1,1)uint32，起始采样时间，从0开始
			% TSize(1,1)uint32，要读入的像素块时长
			% C(1,1)uint8，通道序号，从0开始
			% ZStart(1,1)uint8，起始Z层，从0开始
			% ZSize(1,1)uint8，要读入的像素块Z层数
			% CStart(1,1)uint8，起始通道号，从0开始
			% CSize(1,1)uint8，要读入的像素块通道数
			%See also Image5D.SafePointer Image5D.OmeTiffRWer.PixelPointer Image5D.OirReader.ReadPixels 
			switch nargin
				case 2
					Image5D.internal.Image5DAPI.Oir_ReadToPointer.Call(obj.Pointer,Pointer.Pointer,Pointer.Capacity);
				case {4,5,6,8}
					varargin=cellfun(@uint8,varargin,UniformOutput=false);
					Image5D.internal.Image5DAPI.Oir_ReadToPointer.Call(obj.Pointer,Pointer.Pointer,Pointer.Capacity,uint32(TStart),uint32(TSize),varargin{:});
				otherwise
					Image5D.Image5DException.Wrong_number_of_parameters.Throw;
			end
		end
	end
end