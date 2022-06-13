/**
* @file         codegenmexutils_util.hpp
*
* Purpose       export/import switch for MEX utility functions needed for codegen
*
* Authors       Dinesh Iyer
*
* Copyright     2019 The MathWorks, Inc.
*
*/
#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef _IMAGES_DATATYPES_CODEGENMEXUTILS_UTIL_HPP_
#define _IMAGES_DATATYPES_CODEGENMEXUTILS_UTIL_HPP_

#ifdef BUILDING_IMAGESDATATYPESCODEGENMEXUTILS
#include "package.h"
/* For size_t, boolean_T, etc */
#include "tmwtypes.h"

#define IMAGESDATATYPESCODEGENMEXUTILS_API  DLL_EXPORT_SYM

#else

//#define IMAGESDATATYPESCODEGENMEXUTILS_API  DLL_IMPORT_SYM

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

#ifndef IMAGESDATATYPESCODEGENMEXUTILS_API
#define IMAGESDATATYPESCODEGENMEXUTILS_API
#endif

#endif

#endif // _IMAGES_DATATYPES_CODEGENMEXUTILS_UTIL_HPP_
