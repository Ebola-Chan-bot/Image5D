#include <Image5D异常.h>
#include "magic_enum修改的Mex工具.hpp"
//OirReader
Mex工具API(Oir_CreateReader);
Mex工具API(Oir_DeleteReader);
Mex工具API(Oir_SizeX);
Mex工具API(Oir_SizeY);
Mex工具API(Oir_SizeC);
Mex工具API(Oir_SizeZ);
Mex工具API(Oir_SizeT);
Mex工具API(Oir_SeriesInterval);
Mex工具API(Oir_DeviceColors);
Mex工具API(Oir_ReadPixels);
Mex工具API(Oir_ReadToPointer);
Mex工具API(Oir_LsmimageXml);
Mex工具API(Oir_LaserTransmissivity);
Mex工具API(Oir_PmtVoltage);
Mex工具API(Oir_ZDriveUnitType);
Mex工具API(Oir_ConcatenateSizeT);
Mex工具API(Oir_ChannelColors);
Mex工具API(Oir_CreationDateTime);

//OmeTiffRWer
Mex工具API(Tiff_OpenRead);
Mex工具API(Tiff_OpenRW);
Mex工具API(Tiff_OpenCreate);
Mex工具API(Tiff_PixelType);
Mex工具API(Tiff_DimensionOrder);
Mex工具API(Tiff_SizeP);
Mex工具API(Tiff_SizeX);
Mex工具API(Tiff_SizeY);
Mex工具API(Tiff_SizeI);
Mex工具API(Tiff_SizeC);
Mex工具API(Tiff_SizeZ);
Mex工具API(Tiff_SizeT);
Mex工具API(Tiff_ChannelColors);
Mex工具API(Tiff_FileName);
Mex工具API(Tiff_ImageDescription);
Mex工具API(Tiff_ModifyParameters);
Mex工具API(Tiff_ReadPixels);
Mex工具API(Tiff_ReadPixelsI);
Mex工具API(Tiff_WritePixels);
Mex工具API(Tiff_WritePixelsI);
Mex工具API(Tiff_PixelPointer);
Mex工具API(Tiff_PixelPointerI);
Mex工具API(Tiff_ReadToPointer);
Mex工具API(Tiff_ReadToPointerI);
Mex工具API(Tiff_WriteFromPointer);
Mex工具API(Tiff_WriteFromPointerI);
Mex工具API(Tiff_Close);
using namespace Mex工具;
using namespace Image5D;
using namespace matlab::data;
void Mex工具::初始化()noexcept {}
Mex工具API(Mex工具::执行)
{
	//这个数组的顺序一般不要轻易修改，只在后面追加，因为会影响MATLAB端的调用
	constexpr Mex工具::API 跳转表[] = {
		// OirReader

		Oir_CreateReader,
		Oir_DeleteReader,
		Oir_SizeX,
		Oir_SizeY,
		Oir_SizeC,
		Oir_SizeZ,
		Oir_SizeT,
		Oir_SeriesInterval,
		Oir_DeviceColors,
		Oir_ReadPixels,
		Oir_ReadToPointer,

		// OmeTiffRWer

		Tiff_OpenRead,
		Tiff_OpenRW,
		Tiff_OpenCreate,
		Tiff_PixelType,
		Tiff_DimensionOrder,
		Tiff_SizeP,
		Tiff_SizeX,
		Tiff_SizeY,
		Tiff_SizeI,
		Tiff_SizeC,
		Tiff_SizeZ,
		Tiff_SizeT,
		Tiff_ChannelColors,
		Tiff_FileName,
		Tiff_ImageDescription,
		Tiff_ModifyParameters,
		Tiff_ReadPixels,
		Tiff_ReadPixelsI,
		Tiff_WritePixels,
		Tiff_WritePixelsI,
		Tiff_PixelPointer,
		Tiff_PixelPointerI,
		Tiff_ReadToPointer,
		Tiff_ReadToPointerI,
		Tiff_WriteFromPointer,
		Tiff_WriteFromPointerI,
		Tiff_Close,

		//OirReader v1.6.0

		Oir_LsmimageXml,
		Oir_LaserTransmissivity,
		Oir_PmtVoltage,
		Oir_ZDriveUnitType,

		//OirReader v3.1.0

		Oir_ConcatenateSizeT,
		Oir_ChannelColors,

		//OirReader v3.2.0

		Oir_CreationDateTime,
	};
	try
	{
		跳转表[万能转码<size_t>(std::move(输入[0]))](输出, 输入);
	}
	catch (Image5D::Exception 异常)
	{
		EnumThrow(异常);
	}
	catch (const pugi::xml_parse_result& 异常)
	{
		EnumThrow(异常.status, 异常.description());
	}
	catch (const Image5D::Win32异常& 异常)
	{
		EnumThrow(异常.Image5D异常, WindowsErrorMessage(异常.Win32错误码).get());
	}
}
void Mex工具::清理()noexcept {}