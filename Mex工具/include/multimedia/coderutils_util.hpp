/**
* @file         coderutils_util.h
*
* Purpose       export/import switch for utility functions used for VideoReader codegen
*
* Authors       Dinesh Iyer
*
* Copyright     2019 The MathWorks, Inc. 
*
*/
#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef _MULTIMEDIA_VIDEO_CODER_UTILS_UTIL_HPP_
#define _MULTIMEDIA_VIDEO_CODER_UTILS_UTIL_HPP_

#ifdef BUILDING_VIDEOCODERUTILS
/* For DLL_EXPORT_SYM and EXTERN_C */
#include "package.h"
/* For size_t, boolean_T, etc */
#include "tmwtypes.h"

#define VIDEOCODERUTILS_API DLL_EXPORT_SYM
#else
/*
DLL_IMPORT_SYM is not needed because it's a C API that only uses EXTERN_C
#define VIDEOCODERUTILS_API  DLL_IMPORT_SYM
*/

/* For size_t, boolean_T, etc */
#ifdef MATLAB_MEX_FILE
#include "tmwtypes.h"
#else
#include "rtwtypes.h"
#endif

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif
#endif

#ifndef VIDEOCODERUTILS_API
#define VIDEOCODERUTILS_API
#endif

#endif

#endif // _MULTIMEDIA_VIDEO_CODER_UTILS_UTIL_HPP_