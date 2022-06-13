/**
* @file         ImageMetadata.hpp
*
* Purpose       Class definition of ImageMetadata class
*
* Authors       Dinesh Iyer
*
* Copyright     2019-2021 MathWorks, Inc.
*
*/

#ifndef imagesdatatypesimagemetadata_header
#define imagesdatatypesimagemetadata_header

#include "datatypes_util.hpp"
#include "ImageDefs.hpp"

namespace images
{
    namespace datatypes
    {
        class ImageMetadata
        {
            public:
                // ------------------------------------------------
                // Life time
                // ------------------------------------------------
 
                /**
                  * Default constructor
                  * This represents metadata for an empty Image
                  */
                ImageMetadata()
                    : ImageMetadata( 0, 0, 3, 
                                     UnderlyingType::Uint8,
                                     ColorFormat::RGB,
                                     Layout::RowMajor )
                {}

                /**
                  * Create the metadata for describing an image
                  *
                  * @param - rows - Number of rows in the image
                  * @param - columns - Number of columns in the image
                  * @param - numChannels - Number of channels in the image
                  * @param - dtype - Datatype of the underlying data
                  * @param - format - ColorFormat of the image data
                  * @param - layout - Layout of the image data
                  * @param - relinquish - YES indicates the instance can give up its
                  *                        underlying data buffer without a data copy
                  */
                ImageMetadata( DimType rows,
                               DimType columns,
                               DimType numChannels,
                               UnderlyingType dtype,
                               ColorFormat format,
                               Layout layout )
                    : fHeight(rows), fWidth(columns), fNumChannels(numChannels),
                      fUnderlyingType(dtype),
                      fColorFormat(format),
                      fLayout(layout)
                {}

                // Not really necessary. But adding for completeness
                ~ImageMetadata() noexcept 
                {}

                // ------------------------------------------------
                // Copy and assignment operations
                // ------------------------------------------------
                // Miranda operations are sufficient for this class

                // ------------------------------------------------
                // Property Getters/Setters
                // ------------------------------------------------
                DimType getHeight() const { return fHeight; }
                DimType getWidth() const { return fWidth; }
                DimType getNumChannels() const { return fNumChannels; }
 
                UnderlyingType getUnderlyingType() const { return fUnderlyingType; }
                ColorFormat getColorFormat() const { return fColorFormat; }
                Layout getLayout() const { return fLayout; }

                void setHeight(DimType rows) { fHeight = rows; }
                void setWidth(DimType cols) { fWidth = cols; }
                void setNumChannels(DimType chans) { fNumChannels = chans; }

                void setUnderlyingType(UnderlyingType c) { fUnderlyingType = c; }
                void setColorFormat(ColorFormat cf) { fColorFormat = cf; }
                void setLayout(Layout l) { fLayout = l; }


            private:
                // ------------------------------------------------
                // Properties
                // ------------------------------------------------
                DimType fHeight;
                DimType fWidth;
                DimType fNumChannels;
                UnderlyingType fUnderlyingType;
                ColorFormat fColorFormat;
                Layout fLayout;
        };
    }
}


#endif // imagesdatatypesimagemetadata_header