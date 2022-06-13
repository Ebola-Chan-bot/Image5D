/**
* @file         opencvutils_utils.hpp
*
* Purpose       Declaration OpenCV utility functions 
*
* Authors       Dinesh Iyer
*
* Copyright     2019-2020 MathWorks, Inc.
*
*/

#ifndef imagesdatatypesopencvutils_header
#define imagesdatatypesopencvutils_header

#include "images/datatypes/Image.hpp"
#include "images/datatypes/ImageDefs.hpp"

#ifdef BUILDING_IMAGESDATATYPESOPENCVUTILS
#include "images/datatypes/opencvutils/opencvutils_util.hpp"
#else
#include "opencvutils_util.hpp"
#endif


// Forward Declaration
namespace cv
{
    class Mat;
}


namespace images
{
    namespace datatypes
    {
        namespace opencvutils
        {
            /**
              * Construct an Image from a cv::Mat instance.
              *
              * cv::Mat does not maintain any notion of the photometric
              * interpretation of the data.
              * Hence, the user must specify this upon construction.
              * The data is always in row-major planar order.
              *
              * @param - srcMatrix - Instance of cv::Mat
              * @param - format - ColorFormat of the image data.
              * @param - isCopyData - Flag to determine if a deep copy of the
                                      underlying data is made when creating the
                                      Image
              * @param - relinquish - YES indicates the instance can give up its
              *                        underlying data buffer without a data copy
              */
 
            IMAGESDATATYPESOPENCVUTILS_API images::datatypes::Image  
                        constructFromCvMat( const cv::Mat& srcMatrix,
                                            images::datatypes::ColorFormat format,
                                            bool isCopyData = true, 
                                            images::datatypes::RelinquishDataUnderlying relinquish = 
                                                    images::datatypes::RelinquishDataUnderlying::NO );
            
                                                         
            /**
              * Create a cv::Mat from an image instance
              *
              * cv::Mat does not maintain any notion of the channel order of the data present.
              * Additionally, it always assumes the data is in row-major interleaved order
              *
              * The conversion assumes the following rules:
              * DATATYPE and NumChannels of the Image determine the datatype
              * and number of channels of cv::Mat
              *
              * If Image has data with Layout other than RowMajor,
              * it is transformed into RowMajor. If this needs to be done, the isCopyData
              * flag is ignored.
              *
              * However, the color format is not changed i.e.
              * BGR data is not changed into RGB data.
              *
              * @param - im - Instance of Image that needs to be converted
              * @param - isCopyData - Flag to determine if a deep copy of the
                                      underlying data is made when creating the
                                      cv::Mat
              * @return - cv::Mat instance
              */
            IMAGESDATATYPESOPENCVUTILS_API cv::Mat convertToCvMat( const images::datatypes::Image& im,
                                           bool isCopyData = true );
        }
    }
}

#endif  // imagesdatatypesopencvutils_header
