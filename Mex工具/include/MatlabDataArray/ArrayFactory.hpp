/* Copyright 2014-2021 The MathWorks, Inc. */

#ifndef MATLAB_DATA_ARRAY_FACTORY_HPP_
#define MATLAB_DATA_ARRAY_FACTORY_HPP_

#define _HAS_ROW_MAJOR_SUPPORT_

#include "TypedArray.hpp"
#include "ArrayDimensions.hpp"
#include "CharArray.hpp"
#include "StructArray.hpp"
#include "SparseArray.hpp"
#include "ObjectArray.hpp"
#include "Object.hpp"
#include "Enumeration.hpp"
#include "EnumArray.hpp"
#include "Exception.hpp"
#include "GetArrayType.hpp"
#include "String.hpp"
#include "matlab_extdata_defs.hpp"
#include "TypedIterator.hpp"
#include "Optional.hpp"
#include "GetReturnType.hpp"
#include "MemoryLayout.hpp"
#include "InputLayout.hpp"
#include "RowMajorIterator.hpp"
#include "ColumnMajorIterator.hpp"

#include "detail/ArrayFactoryHelpers.hpp"
#include "detail/StringHelpers.hpp"
#include "detail/HelperFunctions.hpp"
#include "detail/FunctionType.hpp"
#include "detail/ExceptionHelpers.hpp"
#include "detail/publish_util.hpp"

#include <string>
#include <initializer_list>
#include <stdint.h>
#include <iterator>
#include <vector>

namespace matlab {
namespace data {
namespace impl {
class ArrayFactoryImpl;
class ObjectImpl;
}

namespace detail {
class NameListImpl;
}

/**
 * The ArrayFactory base class provides all of the general APIs that
 * each of the concrete factories needs to support.
 */
class ArrayFactory {
  public:
    /**
     * Construct an ArrayFactory
     *
     * @throw FailedToLoadLibMatlabDataArrayException if necessary libraries are not found
     */
    ArrayFactory() {
        typedef int (*CreateArrayFactoryFcnPtr)(typename impl::ArrayFactoryImpl**);
        static const CreateArrayFactoryFcnPtr fcn =
            detail::resolveFunction<CreateArrayFactoryFcnPtr>(
                detail::FunctionType::CREATE_ARRAY_FACTORY);

        impl::ArrayFactoryImpl* impl = nullptr;
        detail::throwIfError(fcn(&impl));
        pImpl = std::shared_ptr<impl::ArrayFactoryImpl>(impl, [](impl::ArrayFactoryImpl* ptr) {
            typedef void (*DestroyArrayFactoryFcnPtr)(typename impl::ArrayFactoryImpl*);
            static const DestroyArrayFactoryFcnPtr destroyFcn =
                detail::resolveFunction<DestroyArrayFactoryFcnPtr>(
                    detail::FunctionType::DESTROY_ARRAY_FACTORY);
            destroyFcn(ptr);
        });
    }

    /**
     * ArrayFactory destructor
     *
     * @throw none
     */
    ~ArrayFactory() MW_NOEXCEPT {
    }

    /**
     * Creates a TypedArray<T> with the given dimensions
     *
     * @param dims - the dimensions for the Array
     *
     * @return TypedArray<T> - an Array with the appropriate type and dimensions
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     * @throw InvalidArrayType - if array type of the new array is not recognized as valid
     * @throw NumberOfElementsExceedsMaximumException - if the ArrayDimensions create an Array which exceeds the max
     */
    template <typename T>
    TypedArray<T> createArray(ArrayDimensions dims) {
        typedef int (*CreateArrayWithDimsFcnPtr)(typename impl::ArrayFactoryImpl*, int, size_t*, size_t,
                                                 typename impl::ArrayImpl**);
        static const CreateArrayWithDimsFcnPtr fcn =
            detail::resolveFunction<CreateArrayWithDimsFcnPtr>(
                detail::FunctionType::CREATE_ARRAY_WITH_DIMS);
        impl::ArrayImpl* impl = nullptr;
        detail::throwIfError(fcn(pImpl.get(), static_cast<int>(GetArrayType<T>::type), &dims[0],
                                 dims.size(), &impl));
        return detail::Access::createObj<TypedArray<T>>(impl);
    }

    /**
     * Creates a TypedArray<T> with the given dimensions and filled in with the
     * data specified by the begin and end iterators. Data is copied and can be in either row major
     * or column order. The data type for the array is determined by the value_type of the iterator.
     *
     * @param dims - the dimensions for the Array
     * @param begin - start of the user supplied data
     * @param end - end of the user supplied data
     * @param inputLayout - the layout of the input data; either row major or column major
     *
     * @return TypedArray<T> - an Array with the appropriate type, dimensions and data
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     * @throw InvalidArrayType - if array type of the new array is not recognized as valid
     * @throw NumberOfElementsExceedsMaximumException - if the ArrayDimensions create an Array which exceeds the max
     */
    template <typename DATA_IT,
              typename T = typename std::remove_cv<typename std::iterator_traits<DATA_IT>::value_type>::type,
              typename ARRAY_TYPE = typename matlab::data::GetReturnType<T>::type>
    typename std::enable_if<!std::is_same<T, Object>::value, TypedArray<ARRAY_TYPE>>::type  
    createArray(ArrayDimensions dims,
                DATA_IT begin,
                const DATA_IT& end,
                InputLayout inputLayout = InputLayout::COLUMN_MAJOR) {
        
        auto retVal = createArray<ARRAY_TYPE>(std::move(dims));
        if (retVal.getNumberOfElements() != 0) {
            if (inputLayout == InputLayout::ROW_MAJOR) {
                fillInArrayImpl<RowMajorIterator, RowMajor>(retVal, begin, end);
            }
            else {
                fillInArrayImpl<ColumnMajorIterator, ColumnMajor>(retVal, begin, end);
            }
        }
        return retVal;
    }
    
    template <typename DATA_IT,
              typename T = typename std::remove_cv<typename std::iterator_traits<DATA_IT>::value_type>::type,
              typename ARRAY_TYPE = typename matlab::data::GetReturnType<T>::type>
    typename std::enable_if<std::is_same<T, Object>::value, TypedArray<ARRAY_TYPE>>::type  
    createArray(ArrayDimensions dims,
                DATA_IT begin,
                const DATA_IT& end,
                InputLayout inputLayout = InputLayout::COLUMN_MAJOR) {

        if (begin == end) {
            detail::throwIfError(static_cast<int>(ExceptionType::InvalidNumberOfElementsProvided));
        }
        
        matlab::data::impl::ArrayImpl* aImpl = nullptr;
        
        typedef int(*CreateObjectArrayWithObjsFcnPtr)(matlab::data::impl::ArrayFactoryImpl*, 
                                                      size_t*, 
                                                      size_t,
                                                      matlab::data::impl::ObjectImpl*,
                                                      matlab::data::impl::ArrayImpl**);
        
        static const CreateObjectArrayWithObjsFcnPtr fcn = detail::resolveFunction<CreateObjectArrayWithObjsFcnPtr>
            (detail::FunctionType::CREATE_OBJECT_ARRAY_WITH_OBJ);
        detail::throwIfError(fcn(pImpl.get(),
                                 &dims[0],
                                 dims.size(),
                                 detail::Access::getImpl<matlab::data::impl::ObjectImpl>(*begin),
                                 &aImpl));
        
        ObjectArray retVal = detail::Access::createObj<ObjectArray>(aImpl);
        auto temp = retVal.getDimensions();

        if (inputLayout == InputLayout::ROW_MAJOR) {
            fillInObjectArrayImpl<RowMajor>(retVal, begin, end);
        }
        else {
            fillInObjectArrayImpl<ColumnMajor>(retVal, begin, end);
        }

        return retVal;
    }

    /**
     * Creates a TypedArray<T> with the given dimensions and filled in with the
     * supplied data in the initializer list. Data must be in column major order.
     *
     * @param dims - the dimensions for the Array
     * @param data - initializer list containing the data
     *
     * @return TypedArray<T> - an Array with the appropriate type, dimensions and data
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     * @throw InvalidArrayType - if array type of the new array is not recognized as valid
     * @throw NumberOfElementsExceedsMaximumException - if the ArrayDimensions create an Array which exceeds the max
     */
    template <typename T>
    TypedArray<typename GetReturnType<T>::type> createArray(ArrayDimensions dims,
                                                            std::initializer_list<T> data) {
        return detail::createArrayWithIterator(pImpl.get(), std::move(dims), data.begin(),
                                               data.end());
    };

    /**
     * Creates a TypedArray<T> with the given dimensions and filled in with the
     * data specified by C-style begin and end pointers. Data is copied and must be in column major
     * order. This methods supports all arithmetic types.
     *
     * @param dims - the dimensions for the Array
     * @param T*  - start of the user supplied data
     * @param T*  - end of the user supplied data
     *
     * @return TypedArray<T> - an Array with the appropriate type, dimensions and data
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     * @throw InvalidArrayType - if array type of the new array is not recognized as valid
     * @throw NumberOfElementsExceedsMaximumException - if the ArrayDimensions create an Array which exceeds the max
     */
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value || is_complex<T>::value, TypedArray<T>>::type
    createArray(ArrayDimensions dims, const T* const begin, const T* const end) {
        typedef int (*CreateArrayWithDimsAndDataFcnPtr)(
            typename impl::ArrayFactoryImpl * impl, int arrayType, size_t* dims, size_t numDims,
            const void* const dataStart, size_t numEl, typename impl::ArrayImpl**);
        static const CreateArrayWithDimsAndDataFcnPtr fcn =
            detail::resolveFunction<CreateArrayWithDimsAndDataFcnPtr>(
                detail::FunctionType::CREATE_ARRAY_WITH_DIMS_AND_DATA);

        impl::ArrayImpl* impl = nullptr;
        detail::throwIfError(fcn(pImpl.get(), static_cast<int>(GetArrayType<T>::type), &dims[0],
                                 dims.size(), begin, (end - begin), &impl));
        return detail::Access::createObj<TypedArray<T>>(impl);
    }


    /**
     * Creates a scalar TypedArray<T> with the given value. This methods supports
     * all arithmetic types.
     *
     * @param val - the value to be inserted into the scalar
     *
     * @return TypedArray<T> - an Array with the appropriate type and data
     *
     * @throw InvalidArrayType - if array type of the new array is not recognized as valid
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     */
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, TypedArray<T>>::type createScalar(
        const T val) {
        typedef int (*CreateScalarArrayFcnPtr)(typename impl::ArrayFactoryImpl * impl, int arrayType,
                                               const void* data, typename impl::ArrayImpl**);
        static const CreateScalarArrayFcnPtr fcn = detail::resolveFunction<CreateScalarArrayFcnPtr>(
            detail::FunctionType::CREATE_SCALAR_ARRAY);
        impl::ArrayImpl* impl = nullptr;
        detail::throwIfError(
            fcn(pImpl.get(), static_cast<int>(GetArrayType<T>::type), &val, &impl));
        return detail::Access::createObj<TypedArray<T>>(impl);
    }

    /**
     * Creates a scalar TypedArray<T> with the given value. This methods supports
     * all complex types.
     *
     * @param val - the value to be inserted into the scalar
     *
     * @return TypedArray<T> - an Array with the appropriate type and data
     *
     * @throw InvalidArrayType - if array type of the new array is not recognized as valid
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     */
    template <typename T>
    typename std::enable_if<is_complex<T>::value, TypedArray<T>>::type createScalar(const T val) {
        auto arr = createArray<T>({1, 1});
        arr[0] = val;
        return arr;
    }

    /**
     * Creates a scalar TypedArray<MATLABString> with the given value
     *
     * @param val - the string to be inserted into the scalar
     *
     * @return TypedArray<MATLABString> - a newly created TypedArray of MATLABString
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     */
    TypedArray<MATLABString> createScalar(const String val) {
        typedef int (*CreateScalarStringFcnPtr)(typename impl::ArrayFactoryImpl * impl, const char16_t* data,
                                                size_t strlen, typename impl::ArrayImpl**);
        static const CreateScalarStringFcnPtr fcn =
            detail::resolveFunction<CreateScalarStringFcnPtr>(
                detail::FunctionType::CREATE_SCALAR_STRING);
        impl::ArrayImpl* impl = nullptr;
        detail::throwIfError(fcn(pImpl.get(), val.c_str(), val.size(), &impl));
        return detail::Access::createObj<TypedArray<MATLABString>>(impl);
    }

    /**
     * Creates a scalar TypedArray<MATLABString> with the given value
     *
     * @param val - the string to be inserted into the scalar
     *
     * @return TypedArray<MATLABString> - an Array containing data of MATLABString type
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     */
    TypedArray<MATLABString> createScalar(const MATLABString val) {
        impl::ArrayImpl* impl = nullptr;
        if (val) {
            typedef int (*CreateScalarStringFcnPtr)(typename impl::ArrayFactoryImpl * impl,
                                                    const char16_t* data, size_t strlen,
                                                    typename impl::ArrayImpl**);
            static const CreateScalarStringFcnPtr fcn =
                detail::resolveFunction<CreateScalarStringFcnPtr>(
                    detail::FunctionType::CREATE_SCALAR_STRING);
            String str = *val;
            detail::throwIfError(fcn(pImpl.get(), str.c_str(), str.size(), &impl));
        } else {
            typedef int (*CreateScalarMissingStringFcnPtr)(typename impl::ArrayFactoryImpl * impl,
                                                           typename impl::ArrayImpl**);
            static const CreateScalarMissingStringFcnPtr fcn2 =
                detail::resolveFunction<CreateScalarMissingStringFcnPtr>(
                    detail::FunctionType::CREATE_SCALAR_MISSING_STRING);
            detail::throwIfError(fcn2(pImpl.get(), &impl));
        }
        return detail::Access::createObj<TypedArray<MATLABString>>(impl);
    }

    /**
     * Creates a CellArray with specified dimensions.
     *
     * @param dims - the dimensions of the CellArray
     *
     * @return CellArray - newly created Cell Array
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     * @throw NumberOfElementsExceedsMaximumException - if the ArrayDimensions create an Array which exceeds the max
     */
    CellArray createCellArray(ArrayDimensions dims) {
        return createArray<Array>(std::move(dims));
    }

    /**
     * Creates a CellArray with the data specified. Data is specified in column major order
     *
     * @param dims - the dimensions of the CellArray
     * @params data - elements to be inserted into the cell array. Primitive types, strings and
     *                Arrays are allowable inputs.
     *
     * @return CellArray - newly created Cell Array
     *
     * @throw NumberOfElementsExceedsMaximumException - if the ArrayDimensions create an Array which exceeds the max
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     */
    template <typename... Targs>
    TypedArray<Array> createCellArray(ArrayDimensions dims, Targs... data) {
        auto retVal = createArray<Array>(std::move(dims));
        if (retVal.getNumberOfElements() != 0) {
            setCellValues(retVal.begin(), retVal.end(), data...);
        }
        return retVal;
    }

    /**
     * Creates a scalar TypedArray<MATLABString> from 7-bit ascii input data
     * Input is converted to UTF16 prior to creating a TypedArray<MATLABString>
     *
     * @param val - std::string to be inserted into the scalar StringArray
     *
     * @return TypedArray<MATLABString> - a TypedArray with data of type MATLABString
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     * @throw NonAsciiCharInInputDataException - if input contains non-ascii data
     */
    TypedArray<MATLABString> createScalar(const std::string& val) {
        if (!detail::isAscii7(val)) {
            throw NonAsciiCharInInputDataException(
                std::string("Input data can only contain ASCII characters"));
        }
        return createScalar(String(val.begin(), val.end()));
    }

    /**
     * Creates a scalar ObjectArray
     *
     * @param val - val Object to be used to create the ObjectArray scalar
     *
     * @return ObjectArray - ObjectArray containing the object
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     */
    ObjectArray createScalar(const Object& val) {
        impl::ArrayImpl* impl = nullptr;
        typedef int (*CreateScalarObjectFcnPtr)(typename impl::ArrayFactoryImpl* impl,
                                                impl::ObjectImpl* objImpl,
                                                typename impl::ArrayImpl**);
        static const CreateScalarObjectFcnPtr fcn =
            detail::resolveFunction<CreateScalarObjectFcnPtr>(
                detail::FunctionType::CREATE_SCALAR_OBJECT);
        detail::throwIfError(fcn(pImpl.get(),
                                 detail::Access::getImpl<impl::ObjectImpl>(val),
                                 &impl));
        return detail::Access::createObj<ObjectArray>(impl);
    }

    /**
     * Creates a 1xn CharArray from the specified char16_t string, where n is the
     * string length
     *
     * @param str - String containing the data to be filled into the Array
     *
     * @return CharArray - a CharArray object containing data copied from str
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     */
    CharArray createCharArray(String str) {
        typedef int (*CreateCharArrayFromChar16FcnPtr)(
            typename impl::ArrayFactoryImpl * impl, const char16_t* data, size_t strlen, 
            typename impl::ArrayImpl**);
        static const CreateCharArrayFromChar16FcnPtr fcn =
            detail::resolveFunction<CreateCharArrayFromChar16FcnPtr>(
                detail::FunctionType::CREATE_CHAR_ARRAY_FROM_CHAR16);
        impl::ArrayImpl* impl = nullptr;
        detail::throwIfError(fcn(pImpl.get(), str.c_str(), str.size(), &impl));
        return detail::Access::createObj<CharArray>(impl);
    }

    /**
     * Creates a 1xn CharArray from the specified std::string, where n is the
     * string length.  Data is checked for 7-bit ascii and converted from
     * std::string<char> to std::string<CHAR16_T>.
     *
     * @param str - std::string containing the data to be filled into the Array
     *
     * @return CharArray - a CharArray object containing data copied from str
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     * @throw NonAsciiCharInInputDataException - if data contains non-ascii characters
     */
    CharArray createCharArray(const std::string& str) {
        if (!detail::isAscii7(str)) {
            throw NonAsciiCharInInputDataException(
                std::string("Input data can only contain ASCII characters"));
        }
        typedef int (*CreateCharArrayFromStringFcnPtr)(
            typename impl::ArrayFactoryImpl * impl, const char* data, size_t strlen, 
            typename impl::ArrayImpl**);
        static const CreateCharArrayFromStringFcnPtr fcn =
            detail::resolveFunction<CreateCharArrayFromStringFcnPtr>(
                detail::FunctionType::CREATE_CHAR_ARRAY_FROM_STRING);
        impl::ArrayImpl* impl = nullptr;
        detail::throwIfError(fcn(pImpl.get(), str.c_str(), str.size(), &impl));
        return detail::Access::createObj<CharArray>(impl);
    }

    /**
     * Creates a StructArray with the given dimensions and fieldnames
     *
     * @param dims - the dimensions for the Array
     * @param fieldNames - vector of the field names for the struct
     *
     * @return StructArray - an Array with the appropriate dimensions and fieldNames
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     * @throw NumberOfElementsExceedsMaximumException - if the ArrayDimensions create an Array which exceeds the max
     * @throw DuplicateFieldNameInStructArrayException - if duplicate field names are specified
     */
    StructArray createStructArray(ArrayDimensions dims, std::vector<std::string> fieldNames) {
        typedef detail::NameListImpl* (*CreateNamesFcnPtr)(size_t num);
        static const CreateNamesFcnPtr fcn3 =
            detail::resolveFunction<CreateNamesFcnPtr>(detail::FunctionType::CREATE_NAMES);
        NameList field_names(fcn3(fieldNames.size()));
        for (const auto& field : fieldNames) {
            typedef void (*AddNameFcnPtr)(typename detail::NameListImpl * impl, const char* name,
                                          size_t nameLen);
            static const AddNameFcnPtr fcn =
                detail::resolveFunction<AddNameFcnPtr>(detail::FunctionType::ADD_NAME);
            fcn(field_names.getImpl(), field.c_str(), field.size());
        }
        impl::ArrayImpl* impl = nullptr;
        typedef int (*CreateStructArrayFcnPtr)(typename impl::ArrayFactoryImpl * impl, size_t * dims,
                                               size_t numDims, typename detail::NameListImpl * names,
                                               typename impl::ArrayImpl**);
        static const CreateStructArrayFcnPtr fcn2 =
            detail::resolveFunction<CreateStructArrayFcnPtr>(
                detail::FunctionType::CREATE_STRUCT_ARRAY);
        detail::throwIfError(
            fcn2(pImpl.get(), &dims[0], dims.size(), field_names.getImpl(), &impl));
        return detail::Access::createObj<StructArray>(impl);
    }

    /**
     * Creates an Enumeration array. The class of the EnumArray is indicated by className.
     * The array is initialized with the list of enumeration names provided by the caller.
     *
     * @param dims array dimensions
     * @param className class name of the enumeration array
     * @param enums List of the enumeration names
     *
     * @return EnumArray - the EnumArray that was created
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     * @throw MustSpecifyClassNameException - if an empty class name is specified
     * @throw WrongNumberOfEnumsSuppliedException - if the wrong number of enumerations is provided
     * @throw NumberOfElementsExceedsMaximumException - if the ArrayDimensions create an Array which exceeds the max
     */
    EnumArray createEnumArray(ArrayDimensions dims,
                              std::string className,
                              std::vector<std::string> enums) {
        typedef detail::NameListImpl* (*CreateNamesFcnPtr)(size_t num);
        static const CreateNamesFcnPtr fcn3 =
            detail::resolveFunction<CreateNamesFcnPtr>(detail::FunctionType::CREATE_NAMES);
        NameList enum_names(fcn3(enums.size()));
        for (const auto& e : enums) {
            typedef void (*AddNameFcnPtr)(typename detail::NameListImpl * impl, const char* name,
                                          size_t nameLen);
            static const AddNameFcnPtr fcn =
                detail::resolveFunction<AddNameFcnPtr>(detail::FunctionType::ADD_NAME);
            fcn(enum_names.getImpl(), e.c_str(), e.size());
        }
        impl::ArrayImpl* impl = nullptr;
        typedef int (*CreateEnumArrayWithEnumsFcnPtr)(
            typename impl::ArrayFactoryImpl * impl, size_t * dims, size_t numDims, const char* cls,
            size_t clsSize, typename detail::NameListImpl* names, typename impl::ArrayImpl** retVal);
        static const CreateEnumArrayWithEnumsFcnPtr fcn2 =
            detail::resolveFunction<CreateEnumArrayWithEnumsFcnPtr>(
                detail::FunctionType::CREATE_ENUM_ARRAY_WITH_ENUMS);
        detail::throwIfError(fcn2(pImpl.get(), &dims[0], dims.size(), className.c_str(),
                                  className.size(), enum_names.getImpl(), &impl));
        return detail::Access::createObj<EnumArray>(impl);
    }

    /**
     * Creates an Enumeration array. The class of the EnumArray is indicated by className.
     *
     * @param dims array dimensions
     * @param className class name of the enumeration array
     *
     * @return EnumArray - the EnumArray that was created
     *
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     * @throw NumberOfElementsExceedsMaximumException - if the ArrayDimensions create an Array which exceeds the max
     * @throw MustSpecifyClassNameException - if an empty class name is specified
     */
    EnumArray createEnumArray(ArrayDimensions dims, std::string className) {
        typedef int (*CreateEnumArrayFcnPtr)(typename impl::ArrayFactoryImpl*, size_t*, size_t, const char*,
                                             size_t, typename impl::ArrayImpl**);
        static const CreateEnumArrayFcnPtr fcn =
            detail::resolveFunction<CreateEnumArrayFcnPtr>(detail::FunctionType::CREATE_ENUM_ARRAY);
        impl::ArrayImpl* impl = nullptr;
        detail::throwIfError(
            fcn(pImpl.get(), &dims[0], dims.size(), className.c_str(), className.size(), &impl));
        return detail::Access::createObj<EnumArray>(impl);
    }

    /**
     * Creates an empty Array. It contains no elements.
     *
     * @return Array - an empty Array
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     */
    Array createEmptyArray() {
        return createArray<double>({0, 0});
    }

    /**
     * Creates a buffer which can be passed into createArrayFromBuffer()
     * No data copies are made when creating an Array from a buffer. Data
     * must be in column major order.
     *
     * @param numberOfElements - the number of elements, not the actual buffer size
     *
     * @return buffer_ptr<T> - unique_ptr containing the buffer
     *
     * @throw InvalidArrayTypeException - if buffer type is not recognized as valid
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     */
    template <typename T>
    buffer_ptr_t<T> createBuffer(size_t numberOfElements) {
        void* buffer = nullptr;
        buffer_deleter_t deleter = nullptr;
        typedef int (*CreateBufferFcnPtr)(typename impl::ArrayFactoryImpl * impl, void** buffer,
                                          void (**deleter)(void*), int dataType,
                                          size_t numElements);
        static const CreateBufferFcnPtr fcn =
            detail::resolveFunction<CreateBufferFcnPtr>(detail::FunctionType::CREATE_BUFFER);
        detail::throwIfError(fcn(pImpl.get(), &buffer, &deleter,
                                 static_cast<int>(GetArrayType<T>::type), numberOfElements));
        return buffer_ptr_t<T>(static_cast<T*>(buffer), deleter);
    }

    /**
     * Creates a TypedArray<T> using the given buffer
     * No data copies are made when creating an Array from a buffer. The TypedArray<T>
     * takes ownership of the buffer.
     *
     * @param dims - the dimensions for the Array
     * @param buffer - buffer containing the data. Buffer will not be copied. Array takes ownership
     *
     * @return TypedArray<T> - TypedArray<T> which wraps the buffer
     *
     * @throw InvalidArrayTypeException - if buffer type is not recognized as valid
     * @throw FeatureNotSupportedException - if creating an array with a row major layout using a release prior to R2019a
     * @throw matlab::InvalidMemoryLayoutException - if the array could not be allocated
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     */
    template <typename T>
    TypedArray<T> createArrayFromBuffer(ArrayDimensions dims,
                                        buffer_ptr_t<T> buffer,
                                        MemoryLayout memoryLayout = MemoryLayout::COLUMN_MAJOR) {
        buffer_deleter_t deleter = buffer.get_deleter();
        impl::ArrayImpl* impl = nullptr;
        typedef int (*CreateArrayFromBufferV2FcnPtr)(
            typename impl::ArrayFactoryImpl * impl, int arrayType, size_t* dims, size_t numDims,
            void* buffer, void (*deleter)(void*), typename impl::ArrayImpl**, int memoryLayout);
        static const CreateArrayFromBufferV2FcnPtr fcn =
            detail::resolveFunctionNoExcept<CreateArrayFromBufferV2FcnPtr>(
                detail::FunctionType::CREATE_ARRAY_FROM_BUFFER_V2);
        if (fcn != nullptr) {
            detail::throwIfError(fcn(pImpl.get(), static_cast<int>(GetArrayType<T>::type), &dims[0],
                                     dims.size(), buffer.release(), deleter, &impl,
                                     static_cast<int>(memoryLayout)));
        }
        else {
            // new version is not available
            // if asking for a row-major need to throw
            if (memoryLayout == MemoryLayout::ROW_MAJOR) {
                throw FeatureNotSupportedException(std::string("Row-major buffers require ") +
                                                   std::string("2019a"));
            }
            typedef int (*CreateArrayFromBufferFcnPtr)(
                typename impl::ArrayFactoryImpl * impl, int arrayType, size_t* dims, size_t numDims,
                void* buffer, void (*deleter)(void*), typename impl::ArrayImpl**);
            static const CreateArrayFromBufferFcnPtr fcn =
                detail::resolveFunction<CreateArrayFromBufferFcnPtr>(
                    detail::FunctionType::CREATE_ARRAY_FROM_BUFFER);
            
            detail::throwIfError(fcn(pImpl.get(), static_cast<int>(GetArrayType<T>::type), &dims[0],
                                     dims.size(), buffer.release(), deleter, &impl));
        }
        return detail::Access::createObj<TypedArray<T>>(impl);
    }

    /**
     * Creates a SparseArray<T> with the given dimensions.
     * Only two dimensions are allowed for sparse arrays.
     *
     * @param dims - the dimensions for the Array
     * @param nnz - number of non-zero elements
     * @param data - buffer containing the non-zero elements. Buffer will not be copied. Array takes
     * ownership
     * @param rows - buffer containing the row value for each element. Buffer will not be copied.
     * Array takes ownership
     * @param cols - buffer containing the column value for each element. Buffer will not be copied.
     * Array takes ownership
     *
     * @return SparseArray<T> - a sparse array with the appropriate type and dimensions, and data
     *
     * @throw InvalidArrayTypeException - if array type is not recognized as valid
     * @throw InvalidDimensionsInSparseArrayException if more than two dimensions are specified.
     * @throw matlab::OutOfMemoryException - if the array could not be allocated
     *
     */
    template <typename T>
    typename std::enable_if<std::is_same<double, T>::value || std::is_same<bool, T>::value ||
                                std::is_same<std::complex<double>, T>::value,
                            SparseArray<T>>::type
    createSparseArray(ArrayDimensions dims,
                      size_t nnz,
                      buffer_ptr_t<T> data,
                      buffer_ptr_t<size_t> rows,
                      buffer_ptr_t<size_t> cols) {

        // If more than two dimensions are specified, throw an exception
        if (dims.size() > 2) {
            throw InvalidDimensionsInSparseArrayException(
                std::string("Sparse Array can only have 2 dimensions"));
        }

        buffer_deleter_t data_deleter = data.get_deleter();
        buffer_deleter_t rows_deleter = rows.get_deleter();
        buffer_deleter_t cols_deleter = cols.get_deleter();

        impl::ArrayImpl* impl = nullptr;
        typedef int (*CreateSparseArrayFromBufferFcnPtr)(
            typename impl::ArrayFactoryImpl * impl, int arrayType, size_t* dims, size_t numDims, size_t nnz,
            void* dataBuffer, void (*dataDeleter)(void*), size_t* rowsBuffer,
            void (*rowsDeleter)(void*), size_t* colsBuffer, void (*colsDeleter)(void*),
            typename impl::ArrayImpl**);
        static const CreateSparseArrayFromBufferFcnPtr fcn =
            detail::resolveFunction<CreateSparseArrayFromBufferFcnPtr>(
                detail::FunctionType::CREATE_SPARSE_ARRAY_FROM_BUFFER);
        detail::throwIfError(fcn(pImpl.get(), static_cast<int>(GetSparseArrayType<T>::type),
                                 &dims[0], dims.size(), nnz, data.release(), data_deleter,
                                 rows.release(), rows_deleter, cols.release(), cols_deleter,
                                 &impl));
        return detail::Access::createObj<SparseArray<T>>(impl);
    }


  protected:
    std::shared_ptr<impl::ArrayFactoryImpl> pImpl;

  private:
    ArrayFactory& operator=(ArrayFactory const& rhs) = delete;
    ArrayFactory(const ArrayFactory& rhs) = delete;

    class NameList {
      public:
        NameList(detail::NameListImpl* impl)
            : pImpl(std::shared_ptr<detail::NameListImpl>(impl, [](detail::NameListImpl* ptr) {
                typedef void (*DestroyNamesFcnPtr)(typename detail::NameListImpl * impl);
                static const DestroyNamesFcnPtr destroyFcn =
                    detail::resolveFunction<DestroyNamesFcnPtr>(
                        detail::FunctionType::DESTROY_NAMES);
                destroyFcn(ptr);
            })) {
        }

        detail::NameListImpl* getImpl() const {
            return pImpl.get();
        }

      private:
        std::shared_ptr<detail::NameListImpl> pImpl;
    };
    template <typename T>
    typename std::enable_if<!std::is_base_of<Array, T>::value, Array>::type createValue(T value) {
        return createScalar(std::move(value));
    }
    template <typename T>
    typename std::enable_if<std::is_base_of<Array, T>::value, Array>::type createValue(T value) {
        return matlab::data::Array(std::move(value));
    }
    template <typename T>
    void setCellValues(TypedIterator<Array> it, TypedIterator<Array> end, T value) {
        *it = createValue(std::move(value));
    }
    template <typename T, typename... Targs>
    void setCellValues(TypedIterator<Array> it, TypedIterator<Array> end, T value, Targs... args) {
        *it = createValue(std::move(value));
        if (++it != end) {
            setCellValues(std::move(it), std::move(end), args...);
        }
    }

    template <typename ORDER,
              typename DATA_IT>
    void fillInObjectArrayImpl(ObjectArray& retVal,
                               const DATA_IT& begin,
                               const DATA_IT& end) {
        
        auto values = ORDER::writableElements(retVal);
        auto it = begin;
        for (auto elem : values) {
            if (it == end) {
                detail::throwWrongNumberOfElementsException(begin, end, retVal.getDimensions());
            }
            elem = Object(*it);
            ++it;
        }
        if (it != end) {
            detail::throwWrongNumberOfElementsException(begin, end, retVal.getDimensions());
        }
    }
    
    template <template<class> class MDA_IT,
              typename ORDER,
              typename ARRAY_TYPE,
              typename DATA_IT,
              typename IT_VALUE = typename std::remove_cv<typename std::iterator_traits<DATA_IT>::value_type>::type>
    typename std::enable_if<!std::is_same<IT_VALUE, MATLABString>::value, void>::type
    fillInArrayImpl(matlab::data::TypedArray<ARRAY_TYPE>& retVal,
                    DATA_IT begin,
                    const DATA_IT& end) {
        
        auto values = ORDER::writableElements(retVal);
        
        for (typename MDA_IT<ARRAY_TYPE>::reference v : values) {
            v = *begin;
            if (++begin == end) {
                break;
            }
        }
    }
    template <template<class> class MDA_IT,
              typename ORDER,
              typename ARRAY_TYPE,
              typename DATA_IT,
              typename IT_VALUE = typename std::remove_cv<typename std::iterator_traits<DATA_IT>::value_type>::type>
    typename std::enable_if<std::is_same<IT_VALUE, MATLABString>::value, void>::type
    fillInArrayImpl(matlab::data::TypedArray<ARRAY_TYPE>& retVal,
                    DATA_IT begin,
                    const DATA_IT& end) {
        
        auto values = ORDER::writableElements(retVal);
        
        for (typename MDA_IT<ARRAY_TYPE>::reference v : values) {
            MATLABString x = *begin;
            if (x) {
                v = x;
            }
            if (++begin == end) {
                break;
            }
        }
    }
    
    template <template<class> class MDA_IT,
              typename ORDER,
              typename DATA_IT>
    void fillInArrayImpl(matlab::data::TypedArray<matlab::data::Array>& retVal,
                         DATA_IT begin,
                         const DATA_IT& end) {
        
        auto values = ORDER::writableElements(retVal);
        
        for (typename MDA_IT<matlab::data::Array>::reference v : values) {
            v = matlab::data::Array(*begin);
            ++begin;
            if (begin == end) {
                break;
            }
        }
    }

};
} // namespace data
} // namespace matlab

#endif
