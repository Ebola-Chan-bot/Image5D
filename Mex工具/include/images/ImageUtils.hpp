/**
* @file         ImageUtils.hpp
*
* Purpose       Utility functions 
*
* Authors       Dinesh Iyer
*
* Copyright 2019-2021 The MathWorks, Inc.
*
*/

#ifndef imagesdatatypesimage_utils_header
#define imagesdatatypesimage_utils_header

#include "ImageDefs.hpp"

#include <cstring>
#include <string>
#include <map>

template<typename MapType, typename InputType>
static typename MapType::mapped_type findValueInMap(const MapType& map, const InputType& key) {
    typename MapType::const_iterator it = map.find(key);
    return it->second;
}

namespace images 
{
    namespace datatypes 
    {
        namespace utils 
        {
            static images::datatypes::UnderlyingType getUnderlyingTypeFromName(const std::string& className) 
            {
                using namespace images::datatypes;

                typedef std::map<std::string, UnderlyingType> ClassMap;
                
                static const ClassMap classMap {
                    {"logical", UnderlyingType::Logical},
                    {"uint8", UnderlyingType::Uint8},
                    {"int8", UnderlyingType::Int8},
                    {"uint16", UnderlyingType::Uint16},
                    {"int16", UnderlyingType::Int16},
                    {"uint32", UnderlyingType::Uint32},
                    {"int32", UnderlyingType::Int32},
                    {"single", UnderlyingType::Single},
                    {"double", UnderlyingType::Double} };

                    return findValueInMap(classMap, className);
            }

            static std::string getUnderlyingTypeName(const images::datatypes::UnderlyingType dtype) 
            {

                using namespace images::datatypes;

                typedef std::map<UnderlyingType, std::string> DatatypeNameMap;

                static const DatatypeNameMap datatypeNameMap {
                    {UnderlyingType::Logical, "logical"},
                    {UnderlyingType::Uint8, "uint8"},
                    {UnderlyingType::Int8, "int8"},
                    {UnderlyingType::Uint16, "uint16"},
                    {UnderlyingType::Int16, "int16"},
                    {UnderlyingType::Uint32, "uint32"},
                    {UnderlyingType::Int32, "int32"},
                    {UnderlyingType::Single, "single"},
                    {UnderlyingType::Double, "double"} };

                    return findValueInMap(datatypeNameMap, dtype);
            }

            static uint8_t getUnderlyingTypeValue(const images::datatypes::UnderlyingType dtype)
            {
                return static_cast<uint8_t>( dtype );
            }

            static images::datatypes::ColorFormat getColorFormatFromName(const std::string& pixelLayout) 
            {
                using namespace images::datatypes;

                typedef std::map<std::string, ColorFormat> ColorFormatMap;
                
                static const ColorFormatMap formatMap {
                    {"BGRA", ColorFormat::BGRA},
                    {"RGB", ColorFormat::RGB},
                    {"BGR", ColorFormat::BGR},
                    {"Grayscale",  ColorFormat::Grayscale},
                    {"Binary", ColorFormat::Binary},
                    {"Generic", ColorFormat::Generic} };

                    return findValueInMap(formatMap, pixelLayout);
            }

            static std::string getColorFormatName(const images::datatypes::ColorFormat format) 
            {

                using namespace images::datatypes;

                typedef std::map<ColorFormat, std::string> ColorFormatNameMap;

                static const ColorFormatNameMap formatNameMap {
                    {ColorFormat::BGRA, "BGRA"},
                    {ColorFormat::RGB, "RGB"},
                    {ColorFormat::BGR, "BGR"},
                    {ColorFormat::Grayscale, "Grayscale"},
                    {ColorFormat::Binary, "Binary"}, 
                    {ColorFormat::Generic, "Generic"} };

                    return findValueInMap(formatNameMap, format);
            }

            static uint8_t getColorFormatValue(const images::datatypes::ColorFormat format)
            {
                return static_cast<uint8_t>( format );
            }

            static images::datatypes::Layout getLayoutFromName(const std::string& layout) 
            {

                using namespace images::datatypes;
                
                typedef std::map<std::string, Layout> LayoutMap;

                static const LayoutMap layoutMap {
                    {"RowMajor", Layout::RowMajor},
                    {"ColumnMajor", Layout::ColumnMajor} };

                    return findValueInMap(layoutMap, layout);
            }

            static std::string getLayoutName(const images::datatypes::Layout layout) 
            {

                using namespace images::datatypes;

                typedef std::map<Layout, std::string> LayoutNameMap;

                static const LayoutNameMap layoutNameMap {
                    {Layout::RowMajor, "RowMajor"},
                    {Layout::ColumnMajor, "ColumnMajor"} };

                    return findValueInMap(layoutNameMap, layout);
            }

            static uint8_t getLayoutValue(const images::datatypes::Layout layout)
            {
                return static_cast<uint8_t>( layout );
            }                        
        }
    }
}

#if defined(_MSC_VER)

// suppress warning for "unreferenced local function has been removed"
// in files that include this file
#pragma warning(disable: 4505)

#endif


#endif  //imagesdatatypesmwimage_utils_header
