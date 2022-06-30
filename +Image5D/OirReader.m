classdef OirReader<handle
    properties(GetAccess=private,SetAccess=immutable)
		%不同于C++，MATLAB类即使构造出错也会析构，所以必须先置0
		Pointer(1,1)uint64=0
	end
    methods
		function obj=OirReader(HeaderPath)
			obj.Pointer=ExceptionCheck(OirReaderMex(uint8(Image5D.internal.OirReaderAPI.CreateOirReader),HeaderPath));
		end
		function delete(obj)
			OirReaderMex(uint8(Image5D.internal.OirReaderAPI.DestroyOirReader),obj.Pointer);
		end
		function Size=SizeX(obj)
			Size=OirReaderMex(uint8(Image5D.internal.OirReaderAPI.SizeX),obj.Pointer);
		end
		function Size=SizeY(obj)
			Size=OirReaderMex(uint8(Image5D.internal.OirReaderAPI.SizeY),obj.Pointer);
		end
		function Size=SizeC(obj)
			Size=OirReaderMex(uint8(Image5D.internal.OirReaderAPI.SizeC),obj.Pointer);
		end
		function Size=SizeZ(obj)
			Size=OirReaderMex(uint8(Image5D.internal.OirReaderAPI.SizeZ),obj.Pointer);
		end
		function Size=SizeT(obj)
			Size=OirReaderMex(uint8(Image5D.internal.OirReaderAPI.SizeT),obj.Pointer);
		end
		function SI=SeriesInterval(obj)
			SI=OirReaderMex(uint8(Image5D.internal.OirReaderAPI.SeriesInterval),obj.Pointer);
		end
		function [Devices,Colors]=DeviceColors(obj)
			[Devices,Colors]=OirReaderMex(uint8(Image5D.internal.OirReaderAPI.DeviceColors),obj.Pointer);
		end
		function Pixels=ReadPixels(obj,TStart,TSize,varargin)
			varargin=cellfun(@uint8,varargin,UniformOutput=false);
			[Succeed,Pixels]=OirReaderMex(uint8(Image5D.internal.OirReaderAPI.ReadPixels),obj.Pointer,uint16(TStart),uint16(TSize),varargin{:});
			if(~Succeed)
				error("Image5D:Image5DException","读取像素失败，可能是指定的坐标超出范围");
			end
		end
    end
end