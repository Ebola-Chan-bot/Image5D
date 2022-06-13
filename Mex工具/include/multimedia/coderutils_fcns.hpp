/**
 * @file    coderutils_fcns.hpp
 * @Copyright 2019 The MathWorks, Inc.
 */

#ifndef _MULTIMEDIA_VIDEO_CODER_UTILS_FCNS_HPP_
#define _MULTIMEDIA_VIDEO_CODER_UTILS_FCNS_HPP_

#ifdef BUILDING_VIDEOCODERUTILS
#include "video/file/coderutils/coderutils_util.hpp"
#else
#include "coderutils_util.hpp" /* To deal with the fact that PackNGo has no include file hierarchy during test */
#endif

#ifdef __cplusplus
extern "C" {
#endif

EXTERN_C VIDEOCODERUTILS_API void coderPermute( const void* in,
                                                void* out, 
                                                size_t nDims, 
                                                const size_t* inDims, 
                                                const size_t* perm, 
                                                size_t numBytesPerVal );
#ifdef __cplusplus
}
#endif

#endif // _MULTIMEDIA_VIDEO_CODER_UTILS_FCNS_HPP_
