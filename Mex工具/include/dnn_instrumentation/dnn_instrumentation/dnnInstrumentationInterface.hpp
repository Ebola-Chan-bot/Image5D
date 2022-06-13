/* Copyright 2018-2019 The MathWorks, Inc. */

#ifndef dnnInstrumentationInterface_h
#define dnnInstrumentationInterface_h

#ifdef __cplusplus
#define DNNINST_API_EXTERN_C                                                                  \
    extern "C"

#else
#define DNNINST_API_EXTERN_C
#endif

#ifdef SL_INTERNAL

#include "version.h"

#if defined(BUILDING_LIBMWDNN_INSTRUMENTATION)
#define DNNINST_API DLL_EXPORT_SYM
#define DNNINST_API_C DNNINST_API_EXTERN_C DLL_EXPORT_SYM
#else
#define DNNINST_API DLL_IMPORT_SYM
#define DNNINST_API_C DNNINST_API_EXTERN_C DLL_IMPORT_SYM
#endif

#else

#define DNNINST_API
#define DNNINST_API_C DNNINST_API_EXTERN_C

#endif

#if defined(_MSC_VER) && (_MSC_VER < 1600)
typedef unsigned int uint32_t;
#elif defined(__LCC__)
typedef unsigned int uint32_t;
#else
#include <stdint.h>
#endif

#include <string>

DNNINST_API_C void instrumentationInit(unsigned int numLoggingEntities ,bool isAllData=true);

DNNINST_API_C void registerId(const std::string& aLayerName);

DNNINST_API_C void instrumentationCleanup(void);

DNNINST_API_C void logData(const std::string& aLayerName, float* aData, unsigned int aSize);

#endif
