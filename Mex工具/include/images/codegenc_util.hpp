/**
* @file         codegenc_util.h
*
* Purpose       export/import switch for C shared lib codegen functions
*
* Authors       Dinesh Iyer
*
* Copyright     2019-2020 The MathWorks, Inc.
*
*/

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef _IMAGES_DATATYPE_CODEGENC_HPP_
#define _IMAGES_DATATYPE_CODEGENC_HPP_

#ifdef IMAGEC_IN_CODEGEN_MODE
#include "rtwtypes.h"

#ifndef IMAGESDATATYPESCODEGENC_API
#define IMAGESDATATYPESCODEGENC_API
#endif

#else

#ifdef BUILDING_IMAGESDATATYPESCODEGENC
/* For DLL_EXPORT_SYM and EXTERN_C */
#include "package.h"
/* For size_t, boolean_T, etc */
#include "tmwtypes.h"

#define IMAGESDATATYPESCODEGENC_API  DLL_EXPORT_SYM

#else
// #define IMAGESDATATYPESCODEGENC_API  DLL_IMPORT_SYM


#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif
#endif

#ifndef IMAGESDATATYPESCODEGENC_API
#define IMAGESDATATYPESCODEGENC_API
#endif

#endif

#endif

#endif // _IMAGES_DATATYPE_CODEGENC_HPP_
