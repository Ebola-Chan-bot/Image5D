classdef OmeTiffRWer<handle
    properties(GetAccess=private,SetAccess=immutable)
		%不同于C++，MATLAB类即使构造出错也会析构，所以必须先置0
		Pointer(1,1)uint64=0
	end
	properties(Dependent)
		PixelType
		DimensionOrder
		SizeX uint16
		SizeY uint16
		SizeC uint8
		SizeZ uint8
		SizeT uint16
		ChannelColors
		FileName
		ImageDescription
	end
	methods(Access=private)
		function obj = OmeTiffRWer(Pointer)
			obj.Pointer=Pointer;
		end
	end
	methods(Static)
		function obj=OpenRead(FilePath)
			obj=Image5D.OmeTiffRWer(ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.OpenRead),FilePath)));
		end
		function obj=OpenRW(FilePath)
			obj=Image5D.OmeTiffRWer(ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.OpenRW),FilePath)));
		end
		function obj=Create(FilePath,PixelType,SizeX,SizeY,SizeC,SizeZ,SizeT,ChannelColors,DimensionOrder)
			obj=Image5D.OmeTiffRWer(ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.Create),FilePath,uint8(PixelType),uint16(SizeX),uint16(SizeY),uint8(SizeC),uint8(SizeZ),uint16(SizeT),uint32(ChannelColors),uint8(DimensionOrder))));
		end
	end
	methods
		function V=get.PixelType(obj)
			V=Image5D.PixelType(ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.PixelType),obj.Pointer)));
		end
		function set.PixelType(obj,V)
			Image5D.PixelType(ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.PixelType),obj.Pointer,uint8(V))));
		end
		function V=get.DimensionOrder(obj)
			V=Image5D.DimensionOrder(ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.DimensionOrder),obj.Pointer)));
		end
		function set.DimensionOrder(obj,V)
			Image5D.DimensionOrder(ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.DimensionOrder),obj.Pointer,uint8(V))));
		end
		function V=SizeP(obj)
			V=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.SizeP),obj.Pointer));
		end
		function V=get.SizeX(obj)
			V=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.SizeX),obj.Pointer));
		end
		function set.SizeX(obj,V)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.SizeX),obj.Pointer,V));
		end
		function V=get.SizeY(obj)
			V=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.SizeY),obj.Pointer));
		end
		function set.SizeY(obj,V)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.SizeY),obj.Pointer,V));
		end
		function V=get.SizeC(obj)
			V=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.SizeC),obj.Pointer));
		end
		function set.SizeC(obj,V)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.SizeC),obj.Pointer,V));
		end
		function V=get.SizeZ(obj)
			V=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.SizeZ),obj.Pointer));
		end
		function set.SizeZ(obj,V)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.SizeZ),obj.Pointer,V));
		end
		function V=get.SizeT(obj)
			V=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.SizeT),obj.Pointer));
		end
		function set.SizeT(obj,V)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.SizeT),obj.Pointer,V));
		end
		function V=get.ChannelColors(obj)
			V=Image5D.ChannelColor.New(ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.ChannelColors),obj.Pointer)));
		end
		function set.ChannelColors(obj,V)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.ChannelColors),obj.Pointer,uint32(V)));
		end
		function V=get.FileName(obj)
			V=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.FileName),obj.Pointer));
		end
		function set.FileName(obj,V)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.FileName),obj.Pointer,V));
		end
		function V=get.ImageDescription(obj)
			V=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.ImageDescription),obj.Pointer));
		end
		function set.ImageDescription(obj,V)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.ImageDescription),obj.Pointer,V));
		end
		function ModifyParameters(obj,PixelType,SizeX,SizeY,SizeC,SizeZ,SizeT,ChannelColors,DimensionOrder,FileName)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.ModifyParameters),obj.Pointer,uint8(PixelType),uint16(SizeX),uint16(SizeY),uint8(SizeC),uint8(SizeZ),uint16(SizeT),uint32(ChannelColors),uint8(DimensionOrder),FileName));
		end
		function Pixels=ReadPixelsI(obj,IStart,ISize)
			Pixels=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.ReadPixelsI),obj.Pointer,uint32(IStart),uint32(ISize)));
		end
		function Pixels=ReadPixelsT(obj,TStart,TSize)
			Pixels=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.ReadPixelsT),obj.Pointer,uint16(TStart),uint16(TSize)));
		end
		function Pixels=ReadPixelsTZC(obj,TStart,TSize,ZStart,ZSize,CStart,CSize)
			Pixels=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.ReadPixelsTZC),obj.Pointer,uint16(TStart),uint16(TSize),uint8(ZStart),uint8(ZSize),uint8(CStart),uint8(CSize)));
		end
		function WritePixelsI(obj,Pixels,IStart,ISize)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.WritePixelsI),obj.Pointer,Pixels,uint32(IStart),uint32(ISize)));
		end
		function WritePixelsT(obj,Pixels,TStart,TSize)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.WritePixelsT),obj.Pointer,Pixels,uint16(TStart),uint16(TSize)));
		end
		function WritePixelsTZC(obj,Pixels,TStart,TSize,ZStart,ZSize,CStart,CSize)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.WritePixelsTZC),obj.Pointer,Pixels,uint16(TStart),uint16(TSize),uint8(ZStart),uint8(ZSize),uint8(CStart),uint8(CSize)));
		end
		function V=PixelPointer(obj,IT,Z,C)
			if nargin>2
				V=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.PixelPointer),obj.Pointer,uint16(IT),uint8(Z),uint8(C)));
			else
				V=ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.PixelPointer),obj.Pointer,uint32(IT)));
			end
		end
		function delete(obj)
			ExceptionCheck(OmeBigTiff5D(uint8(Image5D.internal.OmeTiffAPI.Close),obj.Pointer));
		end
	end
end