/**
* @file         opencvutils_util.h
*
* Purpose       export/import switch for OpenCV utilities
*
* Authors       Dinesh Iyer
*
* Copyright     2019 The MathWorks, Inc.
*
*/
#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef _IMAGES_DATATYPE_OPENCVUTILS_HPP_
#define _IMAGES_DATATYPE_OPENCVUTILS_HPP_

#ifdef BUILDING_IMAGESDATATYPESOPENCVUTILS
/* For DLL_EXPORT_SYM and EXTERN_C */
#include "package.h"

#define IMAGESDATATYPESOPENCVUTILS_API  DLL_EXPORT_SYM

#else
// #define IMAGESDATATYPESOPENCVUTILS_API  DLL_IMPORT_SYM

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif
#endif

#ifndef IMAGESDATATYPESOPENCVUTILS_API
#define IMAGESDATATYPESOPENCVUTILS_API
#endif

#endif

#endif // _IMAGES_DATATYPE_IMAGESDATATYPESOPENCVUTILS_HPP_
