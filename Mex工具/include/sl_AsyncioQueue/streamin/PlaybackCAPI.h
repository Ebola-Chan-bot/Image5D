/* Copyright 2020 The MathWorks, Inc. */

#ifndef PLAYBACK_CAPI_H
#define PLAYBACK_CAPI_H

#include "tmwtypes.h"

#ifndef DLL_EXPORT_SYM
#ifdef SL_INTERNAL
#include "package.h"
#else
#define DLL_EXPORT_SYM
#endif
#endif

#ifdef __cplusplus
#define PLAYBACK_EXPORT_EXTERN_C extern "C" DLL_EXPORT_SYM
#else
#define PLAYBACK_EXPORT_EXTERN_C extern DLL_EXPORT_SYM
#endif

typedef void * SDIPlaybackExecData;
typedef const char * SDIPlaybackBlockPath;

PLAYBACK_EXPORT_EXTERN_C SDIPlaybackExecData pbInit(
    SDIPlaybackBlockPath blockPath,
    SDIPlaybackBlockPath mldatxFilePath,
    const int runID);

PLAYBACK_EXPORT_EXTERN_C void pbStartForPort(
    SDIPlaybackExecData,
    const int portIdx,
    const int tid);

PLAYBACK_EXPORT_EXTERN_C void pbSetPortBuiltinDataType(
    SDIPlaybackExecData,
    const int portIdx,
    const int dataTypeID);
PLAYBACK_EXPORT_EXTERN_C void pbSetPortStringDataType(
    SDIPlaybackExecData,
    const int portIdx);
PLAYBACK_EXPORT_EXTERN_C void pbSetPortEnumDataType(
    SDIPlaybackExecData,
    const int portIdx,
    const char * enumName,
    const int baseTypeID);
PLAYBACK_EXPORT_EXTERN_C void pbSetPortFxpDataType(
    SDIPlaybackExecData,
    const int portIdx,
    const int mode,
    const uint32_T signedness,
    const uint32_T wordLength,
    const int32_T exponent,
    const double slope,
    const double bias);

PLAYBACK_EXPORT_EXTERN_C void pbSetPortIsComplex(SDIPlaybackExecData, const int portIdx);
PLAYBACK_EXPORT_EXTERN_C void pbSetPortIsVarDims(SDIPlaybackExecData, const int portIdx);
PLAYBACK_EXPORT_EXTERN_C void pbSetPortWidth(
    SDIPlaybackExecData,
    const int portIdx,
    const int w);

PLAYBACK_EXPORT_EXTERN_C void pbSetPortUnits(
    SDIPlaybackExecData,
    const int portIdx,
    const char * units);

PLAYBACK_EXPORT_EXTERN_C void pbInitPort(
    SDIPlaybackExecData,
    const int portIdx,
    const int sigID,
    const int expBefore,
    const int expAfter,
    const double startTime);

PLAYBACK_EXPORT_EXTERN_C void pbOutputSignalForPort(
    SDIPlaybackExecData,
    const int portIdx,
    void * pOutput,
    const double curTime);

PLAYBACK_EXPORT_EXTERN_C double pbOutputMessageForPort(
    SDIPlaybackExecData,
    const int portIdx,
    const double curTime,
    boolean_T* ok);

PLAYBACK_EXPORT_EXTERN_C int pbGetCurrentDim(
    SDIPlaybackExecData,
    const int portIdx,
    const int dIdx);

PLAYBACK_EXPORT_EXTERN_C double pbZeroCrossings(
    SDIPlaybackExecData,
    const int portIdx,
    const double curTime);

PLAYBACK_EXPORT_EXTERN_C void pbTerminate(SDIPlaybackExecData);

PLAYBACK_EXPORT_EXTERN_C void pbCleanup(SDIPlaybackExecData *);

#endif
