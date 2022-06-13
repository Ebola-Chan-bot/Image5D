/**
* @file         ImageDefs.hpp
*
* Purpose       Defintions of enumerations required for describing the Image class
*
* Authors       Dinesh Iyer
*
* Copyright     2019-2021 MathWorks, Inc.
*
*/

#ifndef imagesdatatypesimagedefs_header
#define imagesdatatypesimagedefs_header

#include "datatypes_util.hpp"

#include <cstdint>

namespace images
{
    namespace datatypes
    {
        // ------------------------------------------------
        // Enumerations to describe the image data
        // ------------------------------------------------
        /**
         * List of supported datatypes
         */
        enum class IMAGESDATATYPESIMAGE_API UnderlyingType : uint8_t
        {
            Double  = 0,
            Single  = 1,
            Int8    = 2,
            Uint8   = 3,
            Int16   = 4,
            Uint16  = 5,
            Int32   = 6,
            Uint32  = 7,
            Logical = 8
        };

        /**
          * List of supported Color Formats.
          *  The color format describes what each color
          *  channel of a pixel represents
          */
        enum class IMAGESDATATYPESIMAGE_API ColorFormat : uint8_t
        {
            BGRA = 0,
            RGB,
            BGR,
            Grayscale,
            Binary,
            Generic
        };
        
        /**
          * List of supported pixel layouts.
          * The pixel layout describes the arrangement of
          * pixels and samples in memory
          */
        enum class IMAGESDATATYPESIMAGE_API Layout : uint8_t
        {
            ColumnMajor = 0,
            RowMajor
        };
        
        // ------------------------------------------------
        // Enumerations to describe various techniques to
        // to manage the lifetime of the source data buffer
        // provided by the user upon construction
        // ------------------------------------------------
        enum class SourceMgmt : uint8_t
        {
            // Make a copy of the source
            COPY_SRC = 0,
 
            // Take ownership of the source.
            // Memory cleaned when last reference to instance
            // is destroyed
            OWN_SRC,   
 
            // Do not destroy memory upon destruction.
            // Assumes source has longer lifetime than ImageArray
            USE_SRC
        };
        
        // ------------------------------------------------
        // Enumerations to describe whether the instance will
        // create a deep copy of underlying data when calling
        // releaseBuffer()
        // ------------------------------------------------
#if defined(NO)
# undef NO
#endif
#if defined(YES)
# undef YES
#endif
        enum class RelinquishDataUnderlying : uint8_t
        {
            // Make a deep copy when relinquishing the
            // underlying data
            NO = 0,
 
            // No deep copies when relinquishing the underlying
            // data. This mode is recommended when user knows
            // instance will not be copied / assigned.
            YES
        };
        
        // ------------------------------------------------
        // Typedef's used
        // ------------------------------------------------
        // Underlying type used to specify the dimensions of the
        // ImageArray instance
        typedef uint32_t DimType;

        // Indicates the underlying buffer is a collection of bytes.
        // DOES NOT indicate the type of the data that is stored.
        typedef uint8_t BufferType;
    }
}

#endif  // imagesdatatypesimagedefs_header
