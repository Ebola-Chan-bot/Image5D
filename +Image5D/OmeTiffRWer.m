classdef OmeTiffRWer<handle
	%OME TIFF 快速5D读写器，使用内存映射文件和强制像素块连续实现高速读写
	%为了实现高速读写，读写器写出的是自研的OB5格式的 OME TIFF，完全符合 OME TIFF 基本标准，因此可以被任何标准 OME TIFF 读入器读入（ImageJ, BioFormats等）。但是，其它应用程序创建的TIFF文件不一定符合OB5格式规范，因此本读写器仅支持对那些文件的只读功能，不支持修改。
	%读写器对象不能直接构造，需要从以下静态方法中取得：
	% - OpenRead，只读打开现存TIFF文件
	% - OpenRW，打开现存 OB5 TIFF 文件，可读可写
	% - Create，创建新 OB5 TIFF 文件，覆盖现存文件
	%注意，本读写器并不完全支持TIFF格式的所有特性。为了实现快速5D读入，读写器会假定所有IFD的图像像素尺寸相同，且像素块完整不分条带。不满足此条件的TIFF文件将引发错误。
	%每次修改属性值都会导致文件结构重新生成，此过程可能造成像素值损坏，且性能较低。因此请勿依次修改多个属性，而是使用ModifyParameters方法一次性修改多个属性值。
    properties(GetAccess=private,SetAccess=immutable,Transient)
		%不同于C++，MATLAB类即使构造出错也会析构，所以必须先置0
		Pointer(1,1)uint64=0
	end
	properties(Dependent)
		%像素类型。如果文件符合OME标准，从 OME XML 中读入此属性；否则从首IFD中取得。假定所有IFD的像素类型与首IFD相同。
		PixelType Image5D.PixelType
		%维度顺序。仅 OME TIFF 支持此属性。
		DimensionOrder Image5D.DimensionOrder
		%像素字节数，根据PixelType计算得到。不能直接修改此属性，请修改PixelType。
		SizeP
		%图像宽度。如果文件符合OME标准，从 OME XML 中读入此属性；否则从首IFD中取得。假定所有IFD的图像宽度与首IFD相同。
		SizeX uint16
		%图像高度。如果文件符合OME标准，从 OME XML 中读入此属性；否则从首IFD中取得。假定所有IFD的图像高度与首IFD相同。
		SizeY uint16
		%IFD总数。如果文件符合OME标准，此值必等于SizeC*SizeZ*SizeT。不能直接修改此属性，请修改SizeC、SizeZ或SizeT。
		SizeI
		%颜色通道数。仅 OME TIFF 支持此属性。
		SizeC uint8
		%Z层数。仅 OME TIFF 支持此属性。
		SizeZ uint8
		%采样时点数。仅 OME TIFF 支持此属性。
		SizeT uint32
		%各通道颜色，每个通道对应一个Image5D.ChannelColor对象。仅 OME TIFF 支持此属性。
		ChannelColors Image5D.ChannelColor
		%文件名。仅 OME TIFF 支持此属性。此值从 OME XML 中读取，不一定与真实的文件名相同。
		FileName string
		%图像描述。只包含首IFD的图像描述。如果文件符合OME标准，此图像描述即为 OME XML 元数据。修改此属性将可能导致其它基本属性也被修改。
		ImageDescription string
	end
	methods(Access=private)
		function obj = OmeTiffRWer(Pointer)
			obj.Pointer=Pointer;
		end
	end
	methods(Static)
		function obj=OpenRead(FilePath)
			%只读打开现存TIFF文件
			%此方法可以打开符合一般TIFF格式规范的文件，但要求所有IFD图像尺寸相同，且不能分多个条带。如果文件符合OME规范，还要求SizeI==SizeC*SizeZ*SizeT
			%# 语法
			% ```
			% obj=Image5D.OmeTiffRWer.OpenRead(FilePath);
			% ```
			%# 输入参数
			% FilePath，文件路径
			%# 返回值
			% obj，TIFF读入器对象，仅支持读入属性和像素值，不能编辑写出。
			obj=Image5D.OmeTiffRWer(Image5D.internal.Image5DAPI.Tiff_OpenRead.Call(FilePath));
		end
		function obj=OpenRW(FilePath)
			%读写打开现存 OB5 TIFF 文件
			%此方法只能打开由Create方法创建的 OB5 TIFF 格式文件，可读可写。
			%# 语法
			% ```
			% obj=Image5D.OmeTiffRWer.OpenRW(FilePath);
			% ```
			%# 输入参数
			% FilePath，文件路径
			%# 返回值
			% obj，OB5 TIFF 读写器，支持读写属性和像素值。
			%See also Image5D.OmeTiffRWer.Create
			obj=Image5D.OmeTiffRWer(Image5D.internal.Image5DAPI.Tiff_OpenRW.Call(FilePath));
		end
		function obj=Create(FilePath,PixelTypeOrImageDescription,SizeX,SizeY,ChannelColors,SizeZ,SizeT,DimensionOrder)
			%覆盖创建 OB5 TIFF 文件
			%必须在创建时指定图像描述或所有基本属性，以便预分配磁盘空间和文件结构，实现像素值快速写出
			%# 语法
			% ```
			% obj=Image5D.OmeTiffRWer.Create(FilePath,ImageDescription);
			% %根据图像描述创建新文件
			% 
			% obj=Image5D.OmeTiffRWer.Create(FilePath,PixelType,SizeX,SizeY,ChannelColors,SizeZ,SizeT,DimensionOrder)
			% %根据基本属性创建新文件
			% ```
			%# 示例
			% ```
			% import Image5D.*
			%
			% %首先指定基本属性创建第1个图像文件
			% obj1=OmeTiffRWer.Create('示例图像1.tif',PixelType.UINT16,512,512,ChannelColor.New([1 1 1;1 0 0;0 1 0;0 0 1]),DimensionOrder.XYCZT);
			%
			% %创建第2个图像文件，和第1个只有文件名不同
			% obj2=OmeTiffRWer.Create('示例图像2.tif',obj1.ImageDescription);
			% ```
			%# 输入参数
			% FilePath，文件路径
			% ImageDescription(1,1)string，OME XML 图像描述，通常从另一个 OME TIFF 文件取得，包含全部的基本属性信息。其中的文件名字段会根据FilePath参数自动修改。
			% PixelType(1,1)Image5D.PixelType，像素类型，不能指定为DEFAULT
			% SizeX(1,1)uint16，图像宽度
			% SizeY(1,1)uint16，图像高度
			% ChannelColors(:,1)Image5D.ChannelColor，各通道颜色
			% SizeZ(1,1)uint8，Z层数
			% SizeT(1,1)uint32，采样时点数
			% DimensionOrder(1,1)Image5D.DimensionOrder，维度顺序，不能指定为DEFAULT
			%# 返回值
			% obj，OB5 TIFF 读写器，支持读写属性和像素值。
			%# 提示
			%新创建的文件虽然尚未写入像素值，但磁盘空间已经分配，此时如果进行读入像素值操作，不会发生错误，但读入的像素值将可能是任意的，不保证为0。
			%See also Image5D.PixelType Image5D.ChannelColor Image5D.DimensionOrder
			import Image5D.*
			switch nargin
				case 2
					obj=OmeTiffRWer(internal.Image5DAPI.Tiff_OpenCreate.Call(FilePath,PixelTypeOrImageDescription));
				case 8
					obj=OmeTiffRWer(internal.Image5DAPI.Tiff_OpenCreate.Call(FilePath,uint8(PixelTypeOrImageDescription),uint16(SizeX),uint16(SizeY),uint32(ChannelColors),uint8(SizeZ),uint32(SizeT),uint8(DimensionOrder)));
				otherwise
					Image5DException.Wrong_number_of_parameters.Throw;
			end
		end
		function [Image,DimensionOrder,ChannelColors]=QuickRead(TiffPath)
			%快速读入Tiff图像
			%本函数直接将Tiff文件读入为5D数组，同时返回维度顺序和通道颜色
			%# 语法
			% ```
			% [Image,DimensionOrder,ChannelColors]=Image5D.OmeTiffRWer.QuickRead(TiffPath);
			% ```
			%# 输入参数
			% TiffPath(1,1)string，Tiff文件路径
			%# 返回值
			% Image(:,:,:,:,:)，5D图像数组
			% DimensionOrder(1,1)Image5D.DimensionOrder，维度顺序
			% ChannelColors(:,1)Image5D.ChannelColor，各通道颜色
			%See also Image5D.OmeTiffRWer.QuickWrite Image5D.OmeTiffRWer.OpenRead
			Image=Image5D.OmeTiffRWer.OpenRead(TiffPath);
			if nargout>=2
				DimensionOrder=Image.DimensionOrder;
				if nargout>=3
					ChannelColors=Image.ChannelColors;
				end
			end
			Image=Image.ReadPixels;
		end
		function QuickWrite(TiffPath,Image,DimensionOrder,ChannelColors)
			%快速写出Tiff图像
			%本函数直接将5D数组根据维度顺序写出为Tiff文件，可选指定通道颜色
			%# 语法
			% ```
			% Image5D.OmeTiffRWer.QuickWrite(TiffPath,Image,DimensionOrder)
			% %以指定维度顺序将图像数据写出到Tiff文件，通道颜色为白色
			%
			% Image5D.OmeTiffRWer.QuickWrite(___,ChannelColors)
			% %与上述语法组合使用，额外指定通道颜色
			% ```
			%# 示例
			% ```
			% import Image5D.*
			% Reader=OirReader('O.oir');
			% [~,Colors]=Reader.DeviceColors;
			% OmeTiffRWer.QuickWrite('T.tif',Reader.ReadPixels,DimensionOrder.XYCZT,ChannelColor.FromOirColors(Colors));
			% ```
			%# 输入参数
			% TiffPath(1,1)string，要写出的文件路径
			% Image(:,:,:,:,:)，要写出的数组，最多5维
			% DimensionOrder(1,1)Image5D.DimensionOrder，维度顺序
			% ChannelColors(:,1)Image5D.ChannelColor，各通道颜色
			%See also Image5D.DimensionOrder Image5D.ChannelColor Image5D.OmeTiffRWer.QuickRead
			[SizeX,SizeY,SizeC,SizeZ,SizeT]=size(Image,DimensionOrder.SizePermute);
			if nargin<4
				ChannelColors=Image5D.ChannelColor.New(-ones(SizeC,1,'int32'));
			end
			Writer=Image5D.OmeTiffRWer.Create(TiffPath,Image5D.PixelType(class(Image)),SizeX,SizeY,ChannelColors,SizeZ,SizeT,DimensionOrder);
			Writer.WritePixels(Image);
		end
	end
	methods
		function V=get.PixelType(obj)
			V=Image5D.PixelType(Image5D.internal.Image5DAPI.Tiff_PixelType.Call(obj.Pointer));
		end
		function set.PixelType(obj,V)
			Image5D.PixelType(Image5D.internal.Image5DAPI.Tiff_PixelType.Call(obj.Pointer,uint8(V)));
		end
		function V=get.DimensionOrder(obj)
			V=Image5D.DimensionOrder(Image5D.internal.Image5DAPI.Tiff_DimensionOrder.Call(obj.Pointer));
		end
		function set.DimensionOrder(obj,V)
			Image5D.DimensionOrder(Image5D.internal.Image5DAPI.Tiff_DimensionOrder.Call(obj.Pointer,uint8(V)));
		end
		function V=get.SizeP(obj)
			V=Image5D.internal.Image5DAPI.Tiff_SizeP.Call(obj.Pointer);
		end
		function V=get.SizeX(obj)
			V=Image5D.internal.Image5DAPI.Tiff_SizeX.Call(obj.Pointer);
		end
		function set.SizeX(obj,V)
			Image5D.internal.Image5DAPI.Tiff_SizeX.Call(obj.Pointer,V);
		end
		function V=get.SizeY(obj)
			V=Image5D.internal.Image5DAPI.Tiff_SizeY.Call(obj.Pointer);
		end
		function set.SizeY(obj,V)
			Image5D.internal.Image5DAPI.Tiff_SizeY.Call(obj.Pointer,V);
		end
		function V=get.SizeI(obj)
			V=Image5D.internal.Image5DAPI.Tiff_SizeI.Call(obj.Pointer);
		end
		function V=get.SizeC(obj)
			V=Image5D.internal.Image5DAPI.Tiff_SizeC.Call(obj.Pointer);
		end
		function set.SizeC(obj,V)
			Image5D.internal.Image5DAPI.Tiff_SizeC.Call(obj.Pointer,V);
		end
		function V=get.SizeZ(obj)
			V=Image5D.internal.Image5DAPI.Tiff_SizeZ.Call(obj.Pointer);
		end
		function set.SizeZ(obj,V)
			Image5D.internal.Image5DAPI.Tiff_SizeZ.Call(obj.Pointer,V);
		end
		function V=get.SizeT(obj)
			V=Image5D.internal.Image5DAPI.Tiff_SizeT.Call(obj.Pointer);
		end
		function set.SizeT(obj,V)
			Image5D.internal.Image5DAPI.Tiff_SizeT.Call(obj.Pointer,V);
		end
		function V=get.ChannelColors(obj)
			V=Image5D.ChannelColor.New(Image5D.internal.Image5DAPI.Tiff_ChannelColors.Call(obj.Pointer));
		end
		function set.ChannelColors(obj,V)
			Image5D.internal.Image5DAPI.Tiff_ChannelColors.Call(obj.Pointer,uint32(V));
		end
		function V=get.FileName(obj)
			V=Image5D.internal.Image5DAPI.Tiff_FileName.Call(obj.Pointer);
		end
		function set.FileName(obj,V)
			Image5D.internal.Image5DAPI.Tiff_FileName.Call(obj.Pointer,V);
		end
		function V=get.ImageDescription(obj)
			V=Image5D.internal.Image5DAPI.Tiff_ImageDescription.Call(obj.Pointer);
		end
		function set.ImageDescription(obj,V)
			Image5D.internal.Image5DAPI.Tiff_ImageDescription.Call(obj.Pointer,V);
		end
		function ModifyParameters(obj,PixelType,SizeX,SizeY,SizeC,SizeZ,SizeT,ChannelColors,DimensionOrder,FileName)
			%一次修改多个参数，只进行一次文件重构，因此比依次修改多个属性更快。
			%可以只修改一部分属性，另一部分保留原值。可使用特定的缺省值表示保留原值不做修改，而无需获取原值。
			%# 语法
			% ```
			% obj.ModifyParameters(PixelType,SizeX,SizeY,SizeC,SizeZ,SizeT,ChannelColors,DimensionOrder,FileName);
			% ```
			%# 示例
			% ```
			% %以下示例不修改任何属性，全部缺省保留原值。如需修改，只需将对应位置参数改为修改后的值即可。
			% import Image5D.*
			% obj.ModifyParameters(PixelType.DEFAULT,0,0,0,0,0,[],DimensionOrder.DEFAULT,"");
			% ```
			%# 输入参数
			% PixelType(1,1)Image5D.PixelType，像素类型。指定DEFAULT表示不修改此参数。
			% SizeX(1,1)uint16，图像宽度。指定0表示不修改此参数。
			% SizeY(1,1)uint16，图像高度。指定0表示不修改此参数。
			% SizeC(1,1)uint8，颜色通道数。指定0表示不修改此参数。
			% SizeZ(1,1)uint8，Z层数。指定0表示不修改此参数。
			% SizeT(1,1)uint32，采样时点数。指定0表示不修改此参数。
			% ChannelColors(:,1)Image5D.ChannelColor，各通道颜色，必须是尺寸等于SizeC的向量。指定[]表示不修改此参数。
			% DimensionOrder(1,1)Image5D.DimensionOrder，维度顺序。指定DEFAULT表示不修改此参数。
			% FileName string，文件名。指定""表示不修改此参数。
			%See also Image5D.PixelType Image5D.ChannelColor Image5D.DimensionOrder
			arguments
				obj
				PixelType=Image5D.PixelType.DEFAULT
				SizeX=0x0
				SizeY=0x0
				SizeC=0x0
				SizeZ=0x0
				SizeT=0x0
				ChannelColors=[]
				DimensionOrder=Image5D.DimensionOrder.DEFAULT
				FileName=""
			end
			Image5D.internal.Image5DAPI.Tiff_ModifyParameters.Call(obj.Pointer,uint8(PixelType),uint16(SizeX),uint16(SizeY),uint8(SizeC),uint8(SizeZ),uint32(SizeT),uint32(ChannelColors),uint8(DimensionOrder),FileName);
		end
		function Pixels=ReadPixels(obj,TStart,TSize,varargin)
			%读入指定范围内的像素值。仅 OME TIFF 支持此方法。
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
			% %显示首帧。注意imshow假定维度顺序为YX，而返回像素值维度顺序为XY，所以需要转置。此外imshow一般只接受uint8或浮点类型像素值，需要加一个[]参数才能正常显示除此之外的像素值类型。
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
			%See also Image5D.OmeTiffRWer.ReadPixelsI Image5D.OmeTiffRWer.WritePixels
			switch nargin
				case 1
					Pixels=Image5D.internal.Image5DAPI.Tiff_ReadPixels.Call(obj.Pointer);
				case {3,7}
					varargin=cellfun(@uint8,varargin,UniformOutput=false);
					Pixels=Image5D.internal.Image5DAPI.Tiff_ReadPixels.Call(obj.Pointer,uint32(TStart),uint32(TSize),varargin{:});
				otherwise
					Image5D.Image5DException.Wrong_number_of_parameters.Throw;
			end
		end
		function Pixels=ReadPixelsI(obj,IStart,ISize)
			%读入一定IFD范围内的像素值
			%# 语法
			% ```
			% Pixels=obj.ReadPixelsI(IStart,ISize);
			% ```
			%# 示例
			% ```
			% Pixels=obj.ReadPixelsI(0,obj.SizeI);
			% %读入全部像素值
			%
			% imshow(Pixels(:,:,1)',[]);
			% %显示首帧。注意imshow假定维度顺序为YX，而返回像素值维度顺序为XYI，所以需要转置。此外imshow一般只接受uint8或浮点类型像素值，需要加一个[]参数才能正常显示除此之外的像素值类型。
			% ```
			%# 输入参数
			% IStart(1,1)uint32，要读入的起始IFD，从0开始
			% ISize(1,1)uint32，要读入的IFD个数
			%# 返回值
			% Pixels(:,:,:)，像素值，数据类型取决于PixelType属性，维度顺序XYI
			%See also Image5D.OmeTiffRWer.ReadPixels Image5D.OmeTiffRWer.WritePixelsI
			Pixels=Image5D.internal.Image5DAPI.Tiff_ReadPixelsI.Call(obj.Pointer,uint32(IStart),uint32(ISize));
		end
		function WritePixels(obj,Pixels,TStart,TSize,varargin)
			%写出指定范围内的像素值
			%# 语法
			% ```
			% obj.WritePixels(Pixels);
			% %写出所有像素值
			% 
			% obj.WritePixels(Pixels,TStart,TSize);
			% %写出指定时间范围内的像素值
			% 
			% obj.WritePixels(Pixels,TStart,TSize,ZStart,ZSize,CStart,CSize);
			% %写出指定范围内的像素值
			% ```
			%# 示例
			% ```
			% obj.WritePixels(ones(obj.SizeX,obj.SizeY,obj.SizeC,obj.SizeZ,obj.SizeT,'uint16'.Call(0,obj.SizeT,0,obj.SizeZ,0,obj.SizeC);
			% %写出全部像素值为1。注意此处假定PixelType为UINT16且DimensionOrder为XYCZT。
			% ```
			%# 输入参数
			% Pixels(:,:,:,:,:)，要写出的像素值，数据类型取决于PixelType属性，维度顺序取决于DimensionOrder属性
			% TStart(1,1)uint32，要写出的起始时点，从0开始
			% TSize(1,1)uint32，要写出的时点个数
			% ZStart(1,1)uint8，要写出的起始Z层，从0开始
			% ZSize(1,1)uint8，要写出的Z层个数
			% CStart(1,1)uint8，要写出的起始通道，从0开始
			% CSize(1,1)uint8，要写出的通道个数
			%See also Image5D.OmeTiffRWer.ReadPixels Image5D.OmeTiffRWer.WritePixelsI
			switch nargin
				case 2
					Image5D.internal.Image5DAPI.Tiff_WritePixels.Call(obj.Pointer,Pixels);
				case {4,8}
					varargin=cellfun(@uint8,varargin,UniformOutput=false);
					Image5D.internal.Image5DAPI.Tiff_WritePixels.Call(obj.Pointer,Pixels,uint32(TStart),uint32(TSize),varargin{:});
				otherwise
					Image5D.Image5DException.Wrong_number_of_parameters.Throw;
			end
		end
		function WritePixelsI(obj,Pixels,IStart,ISize)
			%写出一定IFD范围内的像素值
			%# 语法
			% ```
			% obj.WritePixelsI(Pixels,IStart,ISize);
			% ```
			%# 示例
			% ```
			% obj.WritePixelsI(ones(obj.SizeX,obj.SizeY,obj.SizeI,'uint16'.Call(0,obj.SizeI);
			% %写出全部像素值为1。注意此处假定PixelType为UINT16。
			% ```
			%# 输入参数
			% Pixels(:,:,:)，要写出的像素值，数据类型取决于PixelType属性，维度顺序XYI
			% IStart(1,1)uint32，要写出的起始IFD，从0开始
			% ISize(1,1)uint32，要写出的IFD个数
			%See also Image5D.OmeTiffRWer.ReadPixelsI Image5D.OmeTiffRWer.WritePixels
			Image5D.internal.Image5DAPI.Tiff_WritePixelsI.Call(obj.Pointer,Pixels,uint32(IStart),uint32(ISize));
		end
		function Pointer=PixelPointer(obj,T,Z,C)
			%返回一个指向像素值的指针。仅 OB5 TIFF 支持此方法。
			%此方法返回一个C++内存指针，直接指向存储像素值的内存位置。通常用于ReadToPointer, WriteFromPointer等方法，实现不经过MATLAB中介的快速文件对拷
			%# 语法
			% ```
			% Pointer=obj.PixelPointer;
			% %返回所有像素的头指针
			%
			% Pointer=obj.PixelPointer(T);
			% %返回从指定时间开始的像素头指针
			%
			% Pointer=obj.PixelPointer(T,Z,C);
			% %返回从指定位置开始的像素头指针
			% ```
			%# 输入参数
			%T(1,1)uint32，像素头起始时间，从0开始
			%Z(1,1)uint8，像素头起始Z层，从0开始
			%C(1,1)uint8，像素头起始通道，从0开始
			%# 返回值
			% Pointer(1,1)Image5D.SafePointer，C++内存指针
			%# 提示
			% 不同于Read和Write，此方法返回的仅是一个指针，并不会对内存中连续排布的像素值进行实际剪切，因此请注意各维尺寸和维度顺序。
			% 具体来说，除最高维度外，较低维度的非0起始并不意味着该维度的0位置永远不会被读到。例如假设维度顺序为XYCZT，即使指针在Z1T0处起始，只意味着Z0T0被跳过，而Z0T1仍然在内存范围内。
			%See also Image5D.SafePointer Image5D.OmeTiffRWer.PixelPointerI Image5D.OmeTiffRWer.ReadToPointer Image5D.OmeTiffRWer.WriteFromPointer
			switch nargin
				case 1
					[Pointer,Capacity]=Image5D.internal.Image5DAPI.Tiff_PixelPointer.Call(obj.Pointer);
				case 2
					[Pointer,Capacity]=Image5D.internal.Image5DAPI.Tiff_PixelPointer.Call(obj.Pointer,uint32(T));
				case 4
					[Pointer,Capacity]=Image5D.internal.Image5DAPI.Tiff_PixelPointer.Call(obj.Pointer,uint32(T),uint8(Z),uint8(C));
				otherwise
					Image5D.Image5DException.Wrong_number_of_parameters.Throw;
			end
			Pointer=Image5D.SafePointer(Pointer,Capacity);
		end
		function Pointer=PixelPointerI(obj,I)
			%返回一个指向像素值的指针。仅 OB5 TIFF 支持此方法。
			%此方法返回一个C++内存指针，直接指向存储像素值的内存位置。通常用于ReadToPointerI, WriteFromPointerI等方法，实现不经过MATLAB中介的快速文件对拷
			%# 语法
			% ```
			% Pointer=obj.PixelPointerI(I);
			% ```
			%# 输入参数
			%I(1,1)uint32，像素头起始IFD，从0开始
			%See also Image5D.OmeTiffRWer.PixelPointer Image5D.OmeTiffRWer.ReadToPointerI Image5D.OmeTiffRWer.WriteFromPointerI
			[Pointer,Capacity]=Image5D.internal.Image5DAPI.Tiff_PixelPointer.Call(obj.Pointer,uint32(I));
			Pointer=Image5D.SafePointer(Pointer,Capacity);
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
			% obj.ReadToPointer(Pointer,TStart,TSize,ZStart,ZSize,CStart,CSize);
			% %读入一定时间、Z层、通道范围内的像素
			% ```
			%# 示例
			% ```
			% import Image5D.*
			% %此方法常用于文件截取，例如取出一段时间内的像素写出到另一个TIFF文件
			% Writer=OmeTiffRWer.Create('图像.tif',obj.PixelType,obj.SizeX,obj.SizeY,obj.ChannelColors,obj.SizeZ,2,obj.DimensionOrder);
			% obj.ReadToPointer(Writer.PixelPointer,0,2);
			% %上例新建了一个SizeT=2的新TIFF，然后将旧TIFF的前两个时点截取，直接拷贝到新TIFF中，不经过MATLAB数组。
			% ```
			%# 输入参数
			% Pointer(1,1)Image5D.SafePointer，C++内存指针，应有足够Capacity存储读入的像素值
			% TStart(1,1)uint32，起始采样时间，从0开始
			% TSize(1,1)uint32，要读入的像素块时长
			% ZStart(1,1)uint8，起始Z层，从0开始
			% ZSize(1,1)uint8，要读入的像素块Z层数
			% CStart(1,1)uint8，起始通道号，从0开始
			% CSize(1,1)uint8，要读入的像素块通道数
			%See also Image5D.SafePointer Image5D.OmeTiffRWer.PixelPointer Image5D.OmeTiffRWer.ReadToPointerI Image5D.OmeTiffRWer.WriteFromPointer
			switch nargin
				case 2
					Image5D.internal.Image5DAPI.Tiff_ReadToPointer.Call(obj.Pointer,Pointer.Pointer,Pointer.Capacity);
				case {4,5,8}
					varargin=cellfun(@uint8,varargin,UniformOutput=false);
					Image5D.internal.Image5DAPI.Tiff_ReadToPointer.Call(obj.Pointer,Pointer.Pointer,Pointer.Capacity,uint32(TStart),uint32(TSize),varargin{:});
				otherwise
					Image5D.Image5DException.Wrong_number_of_parameters.Throw;
			end
		end
		function ReadToPointerI(obj,Pointer,IStart,ISize)
			%读入指定IFD范围内的像素块值到给定指针
			%为了避免MATLAB中介数组造成额外的拷贝开销，此方法允许直接将像素值拷贝到指定C++内存指针。
			%# 语法
			% ```
			% obj.ReadToPointerI(Pointer,IStart,ISize);
			% ```
			%# 输入参数
			% Pointer(1,1)Image5D.SafePointer，C++内存指针，应有足够Capacity存储读入的像素值
			% IStart(1,1)uint32，起始IFD，从0开始
			% ISize(1,1)uint32，要读入的IFD数目
			%See also Image5D.SafePointer Image5D.OmeTiffRWer.PixelPointerI Image5D.OmeTiffRWer.ReadToPointer Image5D.OmeTiffRWer.WriteFromPointerI
			Image5D.internal.Image5DAPI.Tiff_ReadToPointerI.Call(obj.Pointer,Pointer.Pointer,Pointer.Capacity,uint32(IStart),uint32(ISize));
		end
		function WriteFromPointer(obj,Pointer,TStart,TSize,varargin)
			%从给定指针写出到指定像素范围
			%为了避免MATLAB中介数组造成额外的拷贝开销，此方法允许直接从C++内存指针拷贝到指定像素范围
			%# 语法
			% ```
			% obj.WriteFromPointer(Pointer);
			% %写出全部像素
			%
			% obj.WriteFromPointer(Pointer,TStart,TSize);
			% %写出像素到指定时间段
			%
			% obj.WriteFromPointer(Pointer,TStart,TSize,ZStart,ZSize,CStart,CSize);
			% %写出像素到指定TZC范围
			% ```
			%# 示例
			% ```
			% import Image5D.*
			% %此方法常用于文件拼接，例如将两个TIFF文件在时间轴上串联
			% Reader=OmeTiffRWer.OpenRead('图像1.tif');
			% SizeT=Reader.SizeT;
			% obj.WriteFromPointer(Reader.PixelPointer,0,SizeT);
			% Reader=OmeTiffRWer.OpenRead('图像2.tif');
			% obj.WriteFromPointer(Reader.PixelPointer,SizeT,Reader.SizeT);
			% %上例假定所有文件都具有XYCZT的维度顺序，且XYCZ四个维度尺寸均相同，否则可能发生像素值错位。
			% ```
			%# 输入参数
			% Pointer(1,1)Image5D.SafePointer，C++内存指针，应有足够Capacity包含要写出的像素值
			% TStart(1,1)uint32，起始采样时间，从0开始
			% TSize(1,1)uint32，要写出的像素块时长
			% ZStart(1,1)uint8，起始Z层，从0开始
			% ZSize(1,1)uint8，要写出的像素块Z层数
			% CStart(1,1)uint8，起始通道号，从0开始
			% CSize(1,1)uint8，要写出的像素块通道数
			%See also Image5D.SafePointer Image5D.OmeTiffRWer.PixelPointer Image5D.OmeTiffRWer.ReadToPointer Image5D.OmeTiffRWer.WriteFromPointerI
			switch nargin
				case 2
					Image5D.internal.Image5DAPI.Tiff_WriteFromPointer.Call(obj.Pointer,Pointer.Pointer,Pointer.Capacity);
				case {4,5,8}
					varargin=cellfun(@uint8,varargin,UniformOutput=false);
					Image5D.internal.Image5DAPI.Tiff_WriteFromPointer.Call(obj.Pointer,Pointer.Pointer,Pointer.Capacity,uint32(TStart),uint32(TSize),varargin{:});
				otherwise
					Image5D.Image5DException.Wrong_number_of_parameters.Throw;
			end
		end
		function WriteFromPointerI(obj,Pointer,IStart,ISize)
			%从给定指针写出到指定IFD范围
			%为了避免MATLAB中介数组造成额外的拷贝开销，此方法允许直接从C++内存指针拷贝到指定像素范围
			%# 语法
			% ```
			% obj.WriteFromPointer(Pointer,IStart,ISize);
			% ```
			%# 示例
			% ```
			% import Image5D.*
			% %此方法常用于文件拼接，例如将两个TIFF文件IFD串联
			% Reader=OmeTiffRWer.OpenRead('图像1.tif');
			% SizeI=Reader.SizeI;
			% obj.WriteFromPointer(Reader.PixelPointer,0,SizeI);
			% Reader=OmeTiffRWer.OpenRead('图像2.tif');
			% obj.WriteFromPointer(Reader.PixelPointer,SizeI,Reader.SizeI);
			% %上例假定所有文件XY尺寸均相同，否则可能发生像素值错位。
			% ```
			%# 输入参数
			% Pointer(1,1)Image5D.SafePointer，C++内存指针，应有足够Capacity包含要写出的像素值
			% IStart(1,1)uint32，起始IFD，从0开始
			% ISize(1,1)uint32，要写出的IFD数目
			%See also Image5D.SafePointer Image5D.OmeTiffRWer.PixelPointerI Image5D.OmeTiffRWer.ReadToPointerI Image5D.OmeTiffRWer.WriteFromPointer
			Image5D.internal.Image5DAPI.Tiff_WriteFromPointerI.Call(obj.Pointer,Pointer.Pointer,Pointer.Capacity,uint32(IStart),uint32(ISize));
		end
		function delete(obj)
			%删除OmeTiffRWer对象。
			%删除OmeTiffRWer对象变量时会自动关闭相关文件，无需手动操作。
			if obj.Pointer
				Image5D.internal.Image5DAPI.Tiff_Close.Call(obj.Pointer);
			end
		end
	end
end