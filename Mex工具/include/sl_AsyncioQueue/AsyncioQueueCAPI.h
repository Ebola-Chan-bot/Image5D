/* Copyright 2012-2021 The MathWorks, Inc. */

#ifndef ASYNCIO_QUEUE_CAPI_H
#define ASYNCIO_QUEUE_CAPI_H

#include "sl_AsyncioQueue/AsyncioQueueCTypes.h"

#ifndef DLL_EXPORT_SYM
#ifdef SL_INTERNAL
#include "package.h"
#else
#define DLL_EXPORT_SYM
#endif
#endif

#ifdef __cplusplus
#define ASYNCIOQUEUE_EXPORT_EXTERN_C extern "C" DLL_EXPORT_SYM
#else
#define ASYNCIOQUEUE_EXPORT_EXTERN_C extern DLL_EXPORT_SYM
#endif

typedef const char * sdiModelName;

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiInitializeRepository(void);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiInitializeForHostBasedTarget(
    boolean_T bStreamToHost,
    const char * sdrFileName,
    const char * logOptsFileName,
    boolean_T bLiveStream,
    const char * sdrFileIndex);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiInitializeLogIntervalsForHostBasedTarget(
    const char * optsFileName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiInitializeHostBasedRunForRapicAccelSim(sdiAsyncQueueHandle hAsyncQueue);

ASYNCIOQUEUE_EXPORT_EXTERN_C boolean_T sdiIsUsingInlineAsyncQueues(void);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiBindObserversAndStartStreamingEngine(sdiModelName modelName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiCreateAndBindObservers(
    const sdiModelNameU modelName,
    const sdiModelNameU targetName);

#if !defined(_WIN32) || !defined(__LCC__) || defined(__LCC64__)

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiLabelU sdiGetLabelFromChars(sdiSignalName);
ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiFreeLabel(sdiLabelU);
ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiFreeName(sdiSignalNameU);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncRepoDataTypeHandle sdiAsyncRepoGetBuiltInDataTypeHandle(
    const sdiBuiltInDTypeId dataTypeClassification);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncRepoDataTypeHandle sdiAsyncRepoCreateAliasedDataType(
    sdiAliasedName aliasedName,
    const sdiBuiltInDTypeId dataTypeClassification);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoSetAliasedDataTypeName(
    sdiAsyncRepoDataTypeHandle hDT,
    sdiAliasedName aliasedName);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncRepoDataTypeHandle sdiAsyncRepoCreateEnumDataType(
    sdiEnumName enumName,
    sdiEnumClassification enumClassification,
    const int numValues,
    const int *const values,
    sdiEnumLabels labels);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncRepoDataTypeHandle sdiAsyncRepoCreateFixedPointDataType_BinaryPointScaling(
    const int numericType,
    const int signedness,
    const int wordLength,
    const int fractionLength);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncRepoDataTypeHandle sdiAsyncRepoCreateFixedPointDataType_SlopeBiasScaling(
    const int numericType,
    const int signedness,
    const int wordLength,
    const double slopeAdjFactor,
    const int fixedExponent,
    const double bias);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncRepoDataTypeHandle sdiAsyncRepoCreateHalfDataType(void);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncRepoDataTypeHandle sdiAsyncRepoGetPointerDataTypeHandle(void);

typedef enum {
    STRING_CHAR_ARRAY,
    STRING_FL_USTRING_PTR
} SDI_String_Format;

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncRepoDataTypeHandle sdiAsyncRepoGetStringDataTypeHandle(void);
ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncRepoDataTypeHandle sdiAsyncRepoGetCharArrayStringDataTypeHandle(void);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncRepoDataTypeHandle sdiAsyncRepoGetInt64DataTypeHandle(boolean_T bSigned);

ASYNCIOQUEUE_EXPORT_EXTERN_C int sdiGetDataSizeInBytes(
    const sdiAsyncRepoDataTypeHandle hDataType);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiHierarchyDefinition sdiCreateBusHierDefinition(
    sdiHierarchyDefinition parentHier,
    sdiSignalName name,
    const sdiDims * dims,
    const int busTypeBytes);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAddBusHierLeaf(
    sdiHierarchyDefinition parentHier,
    sdiSignalName name,
    sdiUnitsU units,
    int byteOffset,
    const sdiAsyncRepoDataTypeHandle hDT,
    const sdiComplexity complexity,
    const sdiDims * dims,
    const sdiSampleTimeContinuity sampleTimeContinuity);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAddBusHierVarDimsLeaf(
    sdiHierarchyDefinition parentHier,
    sdiSignalName name,
    sdiUnitsU units,
    int byteOffset,
    const sdiAsyncRepoDataTypeHandle hDT,
    const sdiComplexity complexity,
    const sdiDims * dims,
    const sdiSampleTimeContinuity sampleTimeContinuity);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAddBusHierLeafWithCustomHierarchy(
    sdiHierarchyDefinition parentHier,
    sdiSignalName name,
    sdiUnitsU units,
    int byteOffset,
    const sdiAsyncRepoDataTypeHandle hDT,
    const sdiComplexity complexity,
    const sdiDims * dims,
    const sdiSampleTimeContinuity sampleTimeContinuity,
    const sdiSignalPathToLeafElemU customHierarchy,
    const sdiSignalPathToLeafElemU customBlockPath,
    const sdiStringMetaDataValueU signalDescription);

ASYNCIOQUEUE_EXPORT_EXTERN_C boolean_T sdiIsAsyncQueueDisabled(sdiAsyncQueueHandle hAsyncQueue);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncQueueHandle sdiAsyncRepoCreateAsyncioQueue(
    const sdiAsyncRepoDataTypeHandle hDataType,
    const sdiSignalSourceInfoU * sigSourceInfo,
    const char_T *const modelRefPath,
    const char_T *const sigSourceUUIDstr,
    const sdiComplexity complexity,
    const sdiDims * dims,
    const sdiDimsMode dimsMode,
    const sdiSampleTimeContinuity sampleTimeContinuity,
    const char_T *const units);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncQueueHandle sdiStartAsyncioQueueCreation(
    const sdiAsyncRepoDataTypeHandle hDataType,
    const sdiSignalSourceInfoU * sigSourceInfo,
    const char_T *const modelRefPath,
    const char_T *const sigSourceUUIDstr,
    const sdiComplexity complexity,
    const sdiDims * dims,
    const sdiDimsMode dimsMode,
    const sdiSampleTimeContinuity sampleTimeContinuity,
    const char_T *const units);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiCompleteAsyncioQueueCreation(
    const sdiAsyncQueueHandle hAsyncQueue,
    const sdiAsyncRepoDataTypeHandle hDataType,
    const sdiSignalSourceInfoU * sigSourceInfo);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncQueueHandle sdiCreateAsyncQueueForNVBus(
    sdiHierarchyDefinition * hierarchy,
    const sdiSignalSourceInfoU * sigSourceInfo,
    const char_T *const modelRefPath,
    const char_T *const sigSourceUUIDstr,
    const int busSize,
    const sdiDims * dims,
    const sdiSampleTimeContinuity sampleTimeContinuity,
    const int decimation,
    const int maxPoints,
    sdiSignalName loggedName,
    sdiSignalName origSignalName,
    sdiSignalName propName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiCreateAsyncQueuesForVirtualBus(
    const sdiSignalSourceInfoU * sigSourceInfo,
    const char_T *const modelRefPath,
    const char_T *const sigSourceUUIDstr,
    const int numLeafSignals,
    const sdiVirtualBusLeafElementInfoU *const infoForAllLeafSignals,
    sdiAsyncQueueHandle *hAsyncQueues);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiCreateAsyncQueuesForVirtualBusWithExportSettings(
    const sdiSignalSourceInfoU * sigSourceInfo,
    const char_T *const modelRefPath,
    const char_T *const sigSourceUUIDstr,
    const int numLeafSignals,
    const sdiVirtualBusLeafElementInfoU *const infoForAllLeafSignals,
    sdiAsyncQueueHandle *hAsyncQueues,
    const int decimation,
    const int maxPoints,
    sdiSignalName loggedName,
    sdiSignalName origSignalName,
    sdiSignalName propName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetTargetComputer(
    sdiAsyncQueueHandle hAsyncQueue,
    const sdiModelNameU targetName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetImageTypeMetadata(
    sdiAsyncQueueHandle hAsyncQueue,
    sdiDims dims,
    const double frameRate,
    sdiBuiltInDTypeId pixelBaseType,
    const int maxFrameBytes,
    const int layout,
    const int colorFormat,
    const size_t numChannels,
    const char_T * observerType);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoSetSignalExportSettings(
    sdiAsyncQueueHandle hAsyncQueue,
    const int decimation,
    const int maxPoints);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoSetSignalExportName(
    sdiAsyncQueueHandle hAsyncQueue,
    sdiSignalNameU loggedName,
    sdiSignalNameU signalName,
    sdiSignalNameU propagatedName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoSetBlockPathDomain(
    sdiAsyncQueueHandle hAsyncQueue);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoSetSignalDomainType(
    sdiAsyncQueueHandle hAsyncQueue,
    const char_T *const type);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoSetSignalExportOrder(
    sdiAsyncQueueHandle hAsyncQueue,
    const int val);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoSetRollbackMode(
    sdiAsyncQueueHandle hAsyncQueue,
    sdiRollbackMode mode);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetSignalSampleTimeString(
    sdiAsyncQueueHandle hAsyncQueue,
    const char_T * sampleTime,
    const double discreteIncrement,
    const double stopTime);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetSignalRefRate(
    sdiAsyncQueueHandle hAsyncQueue,
    const double refRate);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetSignalIsFrameBased(
    sdiAsyncQueueHandle hAsyncQueue,
    const boolean_T isFrame);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiDisableCompressedTime(
    sdiAsyncQueueHandle hAsyncQueue);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiEnableTemporalOrdering(
    sdiAsyncQueueHandle hAsyncQueue,
    const int enableOrdering);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiDisableDataThinning(
    sdiAsyncQueueHandle hAsyncQueue);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetRunStartTime(
    sdiAsyncQueueHandle hAsyncQueue,
    const double runStartTime);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetModelDisplayNameForQueue(
    sdiAsyncQueueHandle hAsyncQueue,
    const sdiModelNameU modelDisplayName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAddQueueToExistingRun(
    sdiAsyncQueueHandle hAsyncQueue, sdiModelNameU modelName, sdiModelNameU targetName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiFireTargetQueuesCreatedEvent(
    const sdiModelNameU modelName,
    const sdiModelNameU targetName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiFlushStreamingQueues(
    const sdiModelNameU modelName,
    const sdiModelNameU targetName,
    const boolean_T bRestoreInitialState);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetSignalSharedMemoryInfo(
    sdiAsyncQueueHandle hAsyncQueue,
    const boolean_T isHiddenBlock,
    const sdiSharedMemoryType type,
    const unsigned int numWriters,
    const unsigned int * writerPathLens,
    const sdiFullBlkPathU writerPaths);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiRegisterTemporalMetaData(
    sdiAsyncQueueHandle hAsyncQueue,
    const char *const metaDataName,
    const sdiBuiltInDTypeId dataType);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetRecordingDisabled(sdiAsyncQueueHandle hAsyncQueue, boolean_T val);

ASYNCIOQUEUE_EXPORT_EXTERN_C boolean_T sdiIsRecordingDisabled(sdiAsyncQueueHandle hAsyncQueue);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiRegisterWksVariable(
    sdiAsyncQueueHandle hAsyncQueue,
    const CHAR16_T *const varName,
    const char *const format);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiRegisterFileRecording(
    sdiAsyncQueueHandle hAsyncQueue,
    const CHAR16_T *const fileName,
    const char *const format,
    const int numNumericOpts,
    const char *const * numericOptNames,
    const double *const numericOptVals,
    const int numStrOpts,
    const char *const * strOptNames,
    const char *const * strOptValues);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiRegisterFileRecordingU(
    sdiAsyncQueueHandle hAsyncQueue,
    const CHAR16_T *const fileName,
    const char *const format,
    const int numNumericOpts,
    const char *const * numericOptNames,
    const double *const numericOptVals,
    const int numStrOpts,
    const char *const * strOptNames,
    const CHAR16_T *const * strOptValues);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiWriteSignal(
    sdiAsyncQueueHandle hAsyncQueue,
    const double time,
    const void *const data);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiWriteSignalWithMetaData(
    sdiAsyncQueueHandle hAsyncQueue,
    const double time,
    const void *const data,
    const unsigned int numMetaData,
    sdiSignalName * metaDataNames,
    const void ** metaData);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiWriteVarSizedSignal(
    sdiAsyncQueueHandle hAsyncQueue,
    const double time,
    const void *const data,
    const void *const dims,
    const boolean_T bIs64bit);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiWriteVarSizedSignalWithMetaData(
    sdiAsyncQueueHandle hAsyncQueue,
    const double time,
    const void* const data,
    const void* const dims,
    const boolean_T bIs64bit,
    const unsigned int numMetaData,
    sdiSignalName* metaDataNames,
    const void** metaData);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiConfigureAsyncioQueueToLogFixPtAsDouble(
    sdiAsyncQueueHandle hAsyncQueue);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiWriteGap(
    sdiAsyncQueueHandle hAsyncQueue,
    const double time);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSynchronouslyFlushAllQueuesInThisModel(
    sdiModelName modelName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSynchronouslyFlushAllQueuesInThisModelU(
    sdiModelNameU modelName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSyncFlushIfNeededForRapidAccel(void);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiPurgeAllRunDataIfStepbackOverStartTime(
    sdiModelNameU modelName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiRollbackModelToPreviousTime(
    sdiModelName modelName,
    const double time);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiRollbackModelToPreviousTimeU(
    sdiModelNameU modelName,
    const double time);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiTerminateStreaming(
    sdiAsyncQueueHandle * hAsyncQueue);

ASYNCIOQUEUE_EXPORT_EXTERN_C boolean_T sdiIsQueueStreaming(const sdiAsyncQueueHandle);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiStartProfiling(
    const char * phase);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiStopProfiling(
    const char * phase);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetSignalNameForVirtualBus(
    sdiAsyncQueueHandle hAsyncQueue,
    const sdiSignalNameU signalName);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiDataTypeHandle sdiGetDataTypeHandle(
    sdiDataTypeTable dataTypeTable,
    const int sigDataTypeId,
    const int rawDataTypeId);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiDataTypeHandle sdiGetBuiltInDataTypeHandle(
    const sdiBuiltInDTypeId dataTypeClassification);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiDataTypeHandle sdiCreateAliasedDataType(
    sdiAliasedName aliasedName,
    const sdiBuiltInDTypeId dataTypeClassification);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiDataTypeHandle sdiCreateEnumDataType(
    sdiEnumName enumName,
    sdiEnumClassification enumClassification,
    const int numValues,
    const int *const values,
    sdiEnumLabels labels);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiDataTypeHandle sdiCreateFixedPointDataType_UnspecifiedScaling(
    const int numericType,
    const int signedness,
    const int wordLength);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiDataTypeHandle sdiCreateFixedPointDataType_BinaryPointScaling(
    const int numericType,
    const int signedness,
    const int wordLength,
    const int fractionLength);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiDataTypeHandle sdiCreateFixedPointDataType_SlopeBiasScaling(
    const int numericType,
    const int signedness,
    const int wordLength,
    const double slopeAdjFactor,
    const int fixedExponent,
    const double bias);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiSignalHandle sdiCreateSignal(
    const sdiSignalHandle *const hParent,
    const sdiDimsMode dimensionsMode,
    const sdiDims dims,
    const sdiComplexity complexity,
    const sdiSampleTimeContinuity sampleTimeContinuity,
    const sdiDataTypeHandle hDataType);
ASYNCIOQUEUE_EXPORT_EXTERN_C sdiSignalHandle sdiCreateSignalWithAutoExpand(
    const sdiSignalHandle * const hParent,
    const sdiDimsMode dimensionsMode,
    const sdiDims dims,
    const sdiComplexity complexity,
    const sdiSampleTimeContinuity sampleTimeContinuity,
    const sdiDataTypeHandle hDataType,
    const boolean_T bAutoExpand);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetSignalDomainType(
    sdiSignalHandle hSig,
    const char_T *const type);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiSignalHandle sdiCreateHierarchicalSignal(
    const sdiSignalHandle *const hParent,
    const sdiDimsMode dimensionsMode,
    const sdiDims dims);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetSourceInfo(
    const sdiSignalHandle hSig,
    const sdiSignalSourceInfo sigSourceInfo);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetSourceInfoU(
    const sdiSignalHandle hSig,
    const sdiSignalSourceInfoU sigSourceInfo);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoSetSourceInfoU(
    const sdiSignalHandle hSig,
    const sdiSignalSourceInfoU sigSourceInfo);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoSetCommonSignalPropertiesForBus(
    const sdiSignalHandle hRoot,
    const sdiModelNameU topModel);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncQueueHandle sdiRegisterSignalWithAsyncioQueue(
    const sdiSignalHandle hSig);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncQueueHandle sdiAsyncRepoRegisterSignalWithAsyncioQueue(
    const sdiSignalHandle hSig,
    const sdiSignalSourceInfoU sigSourceInfo,
    const sdiComplexity complexity,
    const sdiDims dims,
    const sdiDimsMode dimsMode,
    const sdiSampleTimeContinuity sampleTimeContinuity);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncQueueHandle sdiRegisterHierarchicalSignalWithAsyncioQueue(
    const sdiSignalHandle hSig,
    const int numLeafSignals,
    const sdiLeafElementInfo *const infoForAllLeafSignals,
    const int busSize);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncQueueHandle sdiAsyncRepoRegisterHierarchicalSignalWithAsyncioQueue(
    const sdiSignalHandle hSig,
    const sdiSignalSourceIntegerUUID sigSourceUUID,
    const int numLeafSignals,
    const sdiLeafElementInfo *const infoForAllLeafSignals,
    const int busSize,
    const int numBlockPathElems,
    const sdiFullBlkPathU fullBlockPath,
    const int portIndex,
    const sdiDims dims,
    const sdiDimsMode dimsMode,
    const sdiSampleTimeContinuity sampleTimeContinuity);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSetQueueIsForVerifySignalStreaming(
    sdiAsyncQueueHandle hQueue,
    const int isForVerifySignalStreaming);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiBindQueueWithTestResult(
    sdiAsyncQueueHandle hQueue,
    int *const pResultAddress);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoAddIntMetaData(
    sdiAsyncQueueHandle hQueue,
    sdiMetaDataName name,
    const int value);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoAddIntMetaDataForNVleaf(
    sdiAsyncQueueHandle hQueue,
    sdiMetaDataName name,
    const int value,
    const sdiSignalPathToLeafElemU leafName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAddIntMetaData(
    const sdiSignalHandle hSig,
    sdiMetaDataName name,
    const int value);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoAddFloatMetaData(
    sdiAsyncQueueHandle hQueue,
    sdiMetaDataName name,
    const double value);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoAddFloatMetaDataForNVleaf(
    sdiAsyncQueueHandle hQueue,
    sdiMetaDataName name,
    const double value,
    const sdiSignalPathToLeafElemU leafName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAddFloatMetaData(
    const sdiSignalHandle hSig,
    sdiMetaDataName name,
    const double value);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoAddStringMetaData(
    sdiAsyncQueueHandle hQueue,
    sdiMetaDataName name,
    sdiStringMetaDataValue value);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoAddStringMetaDataU(
    sdiAsyncQueueHandle hQueue,
    sdiMetaDataName name,
    sdiStringMetaDataValueU value);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoAddStringMetaDataForNVleaf(
    sdiAsyncQueueHandle hQueue,
    sdiMetaDataName name,
    sdiStringMetaDataValue value,
    const sdiSignalPathToLeafElemU leafName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoAddStringMetaDataUForNVleaf(
    sdiAsyncQueueHandle hQueue,
    sdiMetaDataName name,
    sdiStringMetaDataValueU value,
    const sdiSignalPathToLeafElemU leafName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAddStringMetaData(
    const sdiSignalHandle hSig,
    sdiMetaDataName name,
    sdiStringMetaDataValue value);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAddStringMetaDataU(
    const sdiSignalHandle hSig,
    sdiMetaDataName name,
    sdiStringMetaDataValueU value);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAsyncRepoHideInSDI(
    sdiAsyncQueueHandle hQueue,
    const sdiSignalPathToLeafElemU leafName);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiAsyncQueueHandle sdiCreateForEachParent(
    const sdiSignalSourceInfoU * sigSourceInfo,
    const char_T *const modelRefPath,
    const sdiDims * dims,
    sdiSignalNameU loggedName,
    sdiSignalNameU origSignalName,
    sdiSignalNameU propName,
    sdiDims * mdlRefDims);

ASYNCIOQUEUE_EXPORT_EXTERN_C sdiLabelU sdiUpdateForEachLeafName(
    sdiSignalSourceInfoU * sigSourceInfo,
    sdiAsyncQueueHandle hParent);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiAttachForEachIterationToParent(
    sdiAsyncQueueHandle hParent,
    sdiAsyncQueueHandle hChild,
    const sdiDims * iterIdx);

ASYNCIOQUEUE_EXPORT_EXTERN_C void* sdiGetLoggingIntervals(
    const char_T *const fullPath);

ASYNCIOQUEUE_EXPORT_EXTERN_C void* sdiGetSlioCatalogue(
    const char_T *const fullPath);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSlioSdiWriteSignal(
    sdiAsyncQueueHandle hAsyncQueue,
    void* accessor,
    const int portIdx,
    const double time,
    const void *const data);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiSlioIsLoggingSignal(
    const char *modelBlockPath,
    const char *localBlockPath,
    const unsigned int portIdx,
    const char *loggedName);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiEnableRemoteInstrumentationForModel(sdiSignalSourceInfoU signalInfo);

ASYNCIOQUEUE_EXPORT_EXTERN_C void sdiDisableRemoteInstrumentationForModel(sdiSignalSourceInfoU signalInfo);

#endif
#endif

