/* Copyright 2020 The MathWorks, Inc. */

#ifndef ASYNCIO_QUEUE_CTYPES_H
#define ASYNCIO_QUEUE_CTYPES_H

#include "tmwtypes.h"

typedef INT64_T sdiSignalHandle;
typedef INT64_T sdiDataTypeHandle;
typedef void * sdiAsyncQueueHandle;
typedef const CHAR16_T * sdiModelNameU;
typedef const char *const * sdiFullBlkPath;
typedef const CHAR16_T *const * sdiFullBlkPathU;
typedef const char *const * sdiSignalID;
typedef const CHAR16_T *const * sdiSignalIDU;
typedef const char * sdiSignalName;
typedef const CHAR16_T * sdiSignalNameU;
typedef CHAR16_T * sdiLabelU;
typedef const char * sdiSignalSourceUUID;
typedef INT64_T sdiSignalSourceIntegerUUID;
typedef char * sdiSignalPathToLeafElem;
typedef const CHAR16_T * sdiSignalPathToLeafElemU;
typedef const char * sdiAliasedName;
typedef const char * sdiEnumName;
typedef const char * sdiEnumClassification;
typedef const char ** const sdiEnumLabels;
typedef const char * sdiMetaDataName;
typedef const char * sdiStringMetaDataValue;
typedef const CHAR16_T * sdiStringMetaDataValueU;
typedef const CHAR16_T * sdiUnitsU;
typedef const char * sdiSubPath;
typedef const CHAR16_T * sdiSubPathU;

typedef const void * sdiAsyncRepoDataTypeHandle;
typedef void * sdiDataTypeTable;
typedef void * sdiHierarchyDefinition;
typedef const void * sdiHierarchyDefinitionInteral;

typedef enum {
    DATA_TYPE_DOUBLE = 0,
    DATA_TYPE_SINGLE,
    DATA_TYPE_INT8,
    DATA_TYPE_UINT8,
    DATA_TYPE_INT16,
    DATA_TYPE_UINT16,
    DATA_TYPE_INT32,
    DATA_TYPE_UINT32,
    DATA_TYPE_BOOLEAN,
    DATA_TYPE_INT64,
    DATA_TYPE_UINT64
} sdiBuiltInDTypeId;

typedef enum {
    DIMENSIONS_MODE_FIXED,
    DIMENSIONS_MODE_VARIABLE
} sdiDimsMode;

typedef enum {
    REAL,
    COMPLEX
} sdiComplexity;

typedef struct {
    sdiFullBlkPath fullBlockPath;
    sdiSubPath subPath;
    sdiSignalID SID;
    unsigned int numBlockPathElems;
    int portIndex;
    sdiSignalName signalName;
    sdiSignalSourceUUID sigSourceUUID;
} sdiSignalSourceInfo;

typedef struct {
    sdiFullBlkPathU fullBlockPath;
    sdiSubPathU subPath;
    sdiSignalIDU SID;
    unsigned int numBlockPathElems;
    int portIndex;
    sdiSignalNameU signalName;
    sdiSignalSourceIntegerUUID sigSourceUUID;
} sdiSignalSourceInfoU;

typedef struct {
    int nDims;
    int *dimensions;
} sdiDims;

typedef enum {
    SAMPLE_TIME_CONTINUOUS,
    SAMPLE_TIME_DISCRETE
} sdiSampleTimeContinuity;

typedef struct {
    const void * data;
    int blobLength;
} sdiBlobDescription;

typedef struct {
    int numericDT;
    int signedness;
    int wordLength;
} sdiFxpPropsUnspecifiedScaling;

typedef struct {
    int numericDT;
    int signedness;
    int wordLength;
    int fractionLength;
} sdiFxpPropsBinaryPointScaling;

typedef struct {
    int numericDT;
    int signedness;
    int wordLength;
    double slpAdjustmentFactor;
    int fixedExponent;
    double bias;
} sdiFxpPropsSlpBiasScaling;

typedef enum {
    ENUM_INT8   = 2,
    ENUM_UINT8  = 3,
    ENUM_INT16  = 4,
    ENUM_UINT16 = 5,
    ENUM_INT32  = 6,
    ENUM_UINT32 = 7,
    NOT_AN_ENUM = 100
} sdiEnumerationBaseClass;

typedef enum {
    LEAF_BUILT_IN,
    LEAF_FIXED_POINT,
    LEAF_ENUM,
    LEAF_STRING,
    LEAF_HALF
} sdiLeafSignalDataTypeClass;

typedef struct {
    sdiSignalPathToLeafElem signalPath;
    sdiBuiltInDTypeId builtInDataTypeId;
    sdiFxpPropsUnspecifiedScaling * fxpPropsUnspecifiedScaling;
    sdiFxpPropsBinaryPointScaling * fxpPropsBinaryPointScaling;
    sdiFxpPropsSlpBiasScaling * fxpPropsSlpBiasScaling;
    sdiEnumerationBaseClass enumerationBaseClass;
    sdiLeafSignalDataTypeClass signalDataTypeClass;
    int byteOffset;
    sdiComplexity complexity;
    sdiDims dims;
    sdiDimsMode dimsMode;
    int isLinearInterp;
} sdiLeafElementInfo;

typedef struct {
    sdiSignalPathToLeafElemU signalPathU;
    sdiBuiltInDTypeId builtInDataTypeId;
    sdiFxpPropsUnspecifiedScaling * fxpPropsUnspecifiedScaling;
    sdiFxpPropsBinaryPointScaling * fxpPropsBinaryPointScaling;
    sdiFxpPropsSlpBiasScaling * fxpPropsSlpBiasScaling;
    sdiEnumerationBaseClass enumerationBaseClass;
    sdiLeafSignalDataTypeClass signalDataTypeClass;
    int sigDataTypeId;
    int rawDataTypeId;
    sdiAsyncRepoDataTypeHandle hDataType;
    int byteOffset;
    sdiComplexity complexity;
    sdiDims dims;
    sdiDimsMode dimsMode;
    int isLinearInterp;
    sdiUnitsU unitsU;
} sdiLeafElementInfoU;

typedef struct {
    sdiSignalNameU signalName;
    sdiAsyncRepoDataTypeHandle hDataType;
    sdiDims dims;
    sdiDimsMode dimsMode;
    sdiComplexity complexity;
    int isLinearInterp;
    const char * units;
} sdiVirtualBusLeafElementInfoU;

typedef enum {
    ROLLBACK_NORMAL = 0,
    ROLLBACK_KEEP_INIT_VALUES = 1
} sdiRollbackMode;

typedef enum {
    NOT_SHARED,
    GLOBAL_DSM,
    LOCAL_DSM,
    MERGE_BLOCK
} sdiSharedMemoryType;

#endif
