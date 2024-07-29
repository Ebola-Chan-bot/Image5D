classdef Image5DAPI<uint64
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

		%OirReader v1.6.0
		Oir_LsmimageXml(38)
		Oir_LaserTransmissivity(39)
		Oir_PmtVoltage(40)
		Oir_ZDriveUnitType(41)

		%OirReader v3.1.0
		Oir_ConcatenateSizeT(42)
		Oir_ChannelColors(43)
	end
	methods
		function varargout=Call(obj,varargin)
			[varargout{1:nargout}]=Image5DMex(uint64(obj),varargin{:});
		end
	end
end