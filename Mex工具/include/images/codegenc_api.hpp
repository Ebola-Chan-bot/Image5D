/**
* @file         codegenc_api.h
*
* Purpose       Entry points for C codegen support
*
* Authors       Dinesh Iyer
*
* Copyright     2019-2021 The MathWorks, Inc.
*
*/

#ifndef _IMAGES_DATATYPE_CODEGENC_API_HPP_
#define _IMAGES_DATATYPE_CODEGENC_API_HPP_

#ifdef IMAGEC_IN_CODEGEN_MODE
#include "codegenc_util.hpp" /* To deal with the fact that PackNGo has no include file hierarchy during test */
#else
#include "images/datatypes/codegenc/codegenc_util.hpp"
#endif

/* For uint64_t */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* ImagePtr;

IMAGESDATATYPESCODEGENC_API ImagePtr createNullImage();

IMAGESDATATYPESCODEGENC_API ImagePtr createEmptyImage();

IMAGESDATATYPESCODEGENC_API ImagePtr createImageUsingProps( unsigned int height, 
                                                            unsigned int width,
                                                            unsigned int numChannels,
                                                            unsigned char datatype, 
                                                            unsigned char colorFormat,
                                                            unsigned char layout,
                                                            char* error );

IMAGESDATATYPESCODEGENC_API ImagePtr createImageUsingPropsAndData( unsigned char* dataUnderlying,
                                                                   unsigned int height, 
                                                                   unsigned int width,
                                                                   unsigned int numChannels,
                                                                   unsigned char datatype, 
                                                                   unsigned char colorFormat,
                                                                   unsigned char layout,
                                                                   char* error );


IMAGESDATATYPESCODEGENC_API unsigned int getHeight(const ImagePtr imPtr);

IMAGESDATATYPESCODEGENC_API unsigned int getWidth(const ImagePtr imPtr);

IMAGESDATATYPESCODEGENC_API unsigned int getNumChannels(const ImagePtr imPtr);

IMAGESDATATYPESCODEGENC_API unsigned char getUnderlyingType(const ImagePtr imPtr);

IMAGESDATATYPESCODEGENC_API unsigned char getColorFormat(const ImagePtr imPtr);

IMAGESDATATYPESCODEGENC_API unsigned char getLayout(const ImagePtr imPtr);

IMAGESDATATYPESCODEGENC_API unsigned char* getUnderlyingData(const ImagePtr imPtr);

IMAGESDATATYPESCODEGENC_API ImagePtr deepCopy(const ImagePtr imPtr, char* error);

IMAGESDATATYPESCODEGENC_API void destroyImage(ImagePtr imPtr);

#ifdef __cplusplus
}
#endif


#endif  // _IMAGES_DATATYPE_CODEGENC_API_HPP_