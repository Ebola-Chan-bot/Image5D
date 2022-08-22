classdef OirReaderAPI<uint8
    enumeration
        CreateOirReader(0)
		DestroyOirReader(1)
		SizeX(2)
		SizeY(3)
		SizeC(4)
		SizeZ(5)
		SizeT(6)
		SeriesInterval(7)
		DeviceColors(8)
		ReadPixels(9)
		ReadToPointer(10)
	end
	methods
		function varargout=Call(obj,varargin)
			varargout=cell(1,nargout);
			[Status,varargout{:}]=OirReaderMex(uint8(obj),varargin{:});
			StatusCheck(Status);
		end
	end
end