classdef Image5DAPI<uint8
	enumeration
		%OirReader
		Oir_CreateReader(0)
		Oir_DeleteReader(1)
		Oir_SizeX(2)
		Oir_SizeY(3)
		Oir_SizeC(4)
		Oir_SizeZ(5)
		Oir_SizeT(6)
		Oir_SeriesInterval(7)
		Oir_DeviceColors(8)
		Oir_ReadPixels(9)
		Oir_ReadToPointer(10)

		%OmeTiffRWer
		Tiff_OpenRead(11)
		Tiff_OpenRW(12)
		Tiff_OpenCreate(13)
		Tiff_PixelType(14)
		Tiff_DimensionOrder(15)
		Tiff_SizeP(16)
		Tiff_SizeX(17)
		Tiff_SizeY(18)
		Tiff_SizeI(19)
		Tiff_SizeC(20)
		Tiff_SizeZ(21)
		Tiff_SizeT(22)
		Tiff_ChannelColors(23)
		Tiff_FileName(24)
		Tiff_ImageDescription(25)
		Tiff_ModifyParameters(26)
		Tiff_ReadPixels(27)
		Tiff_ReadPixelsI(28)
		Tiff_WritePixels(29)
		Tiff_WritePixelsI(30)
		Tiff_PixelPointer(31)
		Tiff_PixelPointerI(32)
		Tiff_ReadToPointer(33)
		Tiff_ReadToPointerI(34)
		Tiff_WriteFromPointer(35)
		Tiff_WriteFromPointerI(36)
		Tiff_Close(37)
	end
	methods(Static,Access=private)
		function [Exception,InnerException]=Status2Exception(Status)
			import Image5D.InnerExceptionType
			Exception=Image5D.Image5DException(Status.ExceptionType);
			if Exception~=Image5D.Image5DException.Successful_operation
				switch InnerExceptionType(Status.InnerException)
					case InnerExceptionType.NoInner
						InnerException=missing;
					case InnerExceptionType.Image5DException
						[IE,II]=Image5D.internal.Image5DAPI.Status2Exception(Status.ErrorCode);
						if isequal(II,missing)
							InnerException=IE;
						else
							InnerException=struct;
							InnerException.Exception=IE;
							InnerException.InnerException=II;
						end
					case InnerExceptionType.Win32Exception
						InnerException=MATLAB.Lang.WindowsErrorCode(Status.ErrorCode);
					case InnerExceptionType.XmlException
						InnerException=Image5D.XmlErrorCode(Status.ErrorCode);
				end
			else
				InnerException=missing;
			end
		end
	end
	methods
		function varargout=Call(obj,varargin)
			varargout=cell(1,nargout);
			[Status,varargout{:}]=Image5DMex(uint8(obj),varargin{:});
			[Exception,InnerException]=Image5D.internal.Image5DAPI.Status2Exception(Status);
			if Exception~=Image5D.Image5DException.Successful_operation
				if isequal(InnerException,missing)
					Exception.Throw;
				elseif isenum(InnerException)
					Exception.Throw(string(InnerException),Detail=InnerException);
				else
					Exception.Throw(Detail=InnerException);
				end
			end
		end
	end
end