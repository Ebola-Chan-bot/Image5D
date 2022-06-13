/* Copyright 2021 The MathWorks, Inc. */

#ifndef ITERATOR_FACTORY_HPP_
#define ITERATOR_FACTORY_HPP_

namespace matlab {
namespace data {
namespace detail {

/**
 * This helper class is used to create const TypedIterators
 */
template <typename T>
class Const {
  public:
    using type = typename std::add_const<T>::type;

    /**
     * create a const iterator to the beginning of a TypedArray
     * @param arr the TypedArray to create the iterator from
     * @returns TypedIterator
     */
    static TypedIterator<type> begin(const TypedArray<T>& arr) {
        return arr.cbegin();
    }

    /**
     * create a const iterator to the end of a TypedArray
     * @param arr the TypedArray to create the iterator from
     * @returns TypedIterator
     */
    static TypedIterator<type> end(const TypedArray<T>& arr) {
        return arr.cend();
    }

    /**
     * create a const iterator to the beginning of a TypedArrayRef
     * @param arr the TypedArrayRef to create the iterator from
     * @returns TypedIterator
     */
    static TypedIterator<type> begin(const TypedArrayRef<T>& arr) {
        return arr.cbegin();
    }

    /**
     * create a const iterator to the end of a TypedArrayRef
     * @param arr the TypedArrayRef to create the iterator from
     * @returns TypedIterator
     */
    static TypedIterator<type> end(const TypedArrayRef<T>& arr) {
        return arr.cend();
    }
};

/**
 * This helper class is used to create non-const TypedIterators
 */
template <typename T>
class NonConst {
  public:
    using type = T;

    /**
     * create a non-const iterator to the beginning of a TypedArray
     * @param arr the TypedArray to create the iterator from
     * @returns TypedIterator
     */
    static TypedIterator<type> begin(TypedArray<T>& arr) {
        return arr.begin();
    }

    /**
     * create a non-const iterator to the end of a TypedArray
     * @param arr the TypedArray to create the iterator from
     * @returns TypedIterator
     */
    static TypedIterator<type> end(TypedArray<T>& arr) {
        return arr.end();
    }

    /**
     * create a non-const iterator to the beginning of a TypedArrayRef
     * @param arr the TypedArrayRef to create the iterator from
     * @returns TypedIterator
     */
    static TypedIterator<type> begin(TypedArrayRef<T>& arr) {
        return arr.begin();
    }

    /**
     * create a non-const iterator to the end of a TypedArrayRef
     * @param arr the TypedArrayRef to create the iterator from
     * @returns TypedIterator
     */
    static TypedIterator<type> end(TypedArrayRef<T>& arr) {
        return arr.end();
    }

    /**
     * convert the result of indexing operation (operator[]) to a TypedArrayRef
     * @param ref the result of the indexing operation
     * @returns TypedArrayRef
     */
    static TypedArrayRef<T> convertRef(ArrayElementTypedRef<Array, false>&& ref) {
        const TypedArrayRef<T> retVal = std::move(ref);
        return retVal;
    }   
};

/**
 * This class can create either row major or column major const or non-const iterators
 */
template<template<class> class IteratorType>
class IteratorFactory {
  private:
    static MemoryLayout getMemoryLayout(const matlab::data::Array& arr) {
        // check to see if installed version supports a getMemoryLayout
        typedef int (*ArrayGetMemoryLayoutFcnPtr)(matlab::data::impl::ArrayImpl*, int*);
        static const ArrayGetMemoryLayoutFcnPtr fcn = (ArrayGetMemoryLayoutFcnPtr)get_function_ptr(static_cast<int>(FunctionType::ARRAY_GET_MEMORY_LAYOUT));
        if (fcn == nullptr) {
            // if not supported, this must be column major
            return MemoryLayout::COLUMN_MAJOR;
        }
        int layout;
        fcn(matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(arr), &layout);
        return static_cast<MemoryLayout>(layout);
    }
    
    static MemoryLayout getMemoryLayout(const matlab::data::Reference<matlab::data::Array>& ref) {
        const matlab::data::Array arr(ref);
        return getMemoryLayout(arr);
    }
    
  public:

    /**
     * Create a non-const iterator to the beginning of the input array
     * @param arr Array to be used to create the iterator
     * @returns IteratorType<T>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T>
    static IteratorType<typename NonConst<T>::type> begin(matlab::data::Array& arr) {
        if (TypedArray<T>::type != arr.getType()) {
            detail::throwIfError(static_cast<int>(matlab::data::ExceptionType::InvalidDataType));
        }
        auto r = matlab::data::getWritableElements<T>(arr);
        return IteratorType<typename NonConst<T>::type>(std::move(r.begin()),
                                                        false,
                                                        arr.getDimensions(),
                                                        getMemoryLayout(arr));
    }

    /**
     * Create a non-const iterator to the beginning of the input array
     * @param arr TypedArray to be used to create the iterator
     * @returns IteratorType<T>
     */
    template<typename T>
    static IteratorType<typename NonConst<T>::type> begin(matlab::data::TypedArray<T>& arr) {
        return IteratorType<typename NonConst<T>::type>(NonConst<T>::begin(arr),
                                                        false,
                                                        arr.getDimensions(),
                                                        getMemoryLayout(arr));
    }

    /**
     * Create a non-const iterator to the beginning of the input array reference
     * @param ref Reference<Array> to be used to create the iterator
     * @returns IteratorType<T>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T>
    static IteratorType<typename NonConst<T>::type> begin(Reference<Array>& ref) {
        typedef int(*TypedArrayReferenceValidateTypeFcnPtr)(matlab::data::detail::ReferenceImpl*, int, bool);
        static const TypedArrayReferenceValidateTypeFcnPtr fcn = matlab::data::detail::resolveFunction<TypedArrayReferenceValidateTypeFcnPtr>
            (matlab::data::detail::FunctionType::TYPED_ARRAY_REFERENCE_VALIDATE_TYPE);
        matlab::data::detail::throwIfError(fcn(matlab::data::detail::Access::getImpl<matlab::data::detail::ReferenceImpl>(ref), static_cast<int>(GetArrayType<T>::type), true));
        auto r = matlab::data::getWritableElements<T>(ref);
        return IteratorType<typename NonConst<T>::type>(std::move(r.begin()),
                                                        false,
                                                        ref.getDimensions(),
                                                        getMemoryLayout(ref));
    }
    
    /**
     * Create a non-const iterator to the beginning of the input array reference
     * @param arr TypedArrayRef to be used to create the iterator
     * @returns IteratorType<T>
     */
    template<typename T>
    static IteratorType<typename NonConst<T>::type> begin(TypedArrayRef<T>& arr) {
        return IteratorType<typename NonConst<T>::type>(NonConst<T>::begin(arr),
                                                        false,
                                                        arr.getDimensions(),
                                                        getMemoryLayout(arr));
    }

    /**
     * Create a non-const iterator to the beginning of the input array reference
     * @param ref result of operator[] to be used to create the iterator
     * @returns IteratorType<T>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T,
             typename ARRAY_TYPE = typename matlab::data::GetReturnType<T>::type>
    static IteratorType<typename NonConst<T>::type> begin(matlab::data::ArrayElementTypedRef<Array, false>&& ref) {
        matlab::data::TypedArrayRef<T> arr = std::move(ref);
        return IteratorType<typename NonConst<T>::type>(NonConst<T>::begin(arr),
                                                        false,
                                                        arr.getDimensions(),
                                                        getMemoryLayout(arr));
    }
    
    /**
     * Create a const iterator to the beginning of the input array
     * @param arr Array to be used to create the iterator
     * @returns IteratorType<T const>
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> cbegin(const matlab::data::TypedArray<T>& arr) {
        return begin(arr);
    }

    /**
     * Create a const iterator to the beginning of the input array
     * @param arr Array to be used to create the iterator
     * @returns IteratorType<T const>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> cbegin(const matlab::data::Array& arr) {
        if (TypedArray<T>::type != arr.getType()) {
            detail::throwIfError(static_cast<int>(matlab::data::ExceptionType::InvalidDataType));
        }
        const TypedArray<T> t_arr(arr);
        return begin(t_arr);
    }

    /**
     * Create a const iterator to the beginning of the input array
     * @param arr Array to be used to create the iterator
     * @returns IteratorType<T const>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> cbegin(const matlab::data::Reference<matlab::data::Array>& arr) {
        const TypedArrayRef<T> t_arr(arr);
        return begin(t_arr);
    }

    /**
     * Create a const iterator to the beginning of the input array
     * @param arr Array to be used to create the iterator
     * @returns IteratorType<T const>
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> begin(const matlab::data::TypedArray<T>& arr) {
        return IteratorType<typename Const<T>::type>(Const<T>::begin(arr),
                                                     false,
                                                     arr.getDimensions(),
                                                     getMemoryLayout(arr));
    }

    /**
     * Create a const iterator to the beginning of the input array
     * @param arr Array to be used to create the iterator
     * @returns IteratorType<T const>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> begin(const matlab::data::Array& arr) {
        if (TypedArray<T>::type != arr.getType()) {
            detail::throwIfError(static_cast<int>(matlab::data::ExceptionType::InvalidDataType));
        }
        const TypedArray<T> t_arr(arr);
        return begin(t_arr);
    }

    /**
     * Create a const iterator to the beginning of the input array
     * @param arr Array to be used to create the iterator
     * @returns IteratorType<T const>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> begin(const matlab::data::Reference<matlab::data::Array>& arr) {
        const TypedArrayRef<T> t_arr(arr);
        return begin(t_arr);
    }

    /**
     * Create a const iterator to the beginning of the input array reference
     * @param arr TypedArrayRef to be used to create the iterator
     * @returns IteratorType<T const>
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> cbegin(const matlab::data::TypedArrayRef<T>& arr) {
        return begin(arr);
    }

    /**
     * Create a const iterator to the beginning of the input array reference
     * @param arr TypedArrayRef to be used to create the iterator
     * @returns IteratorType<T const>
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> begin(const matlab::data::TypedArrayRef<T>& arr) {
        return IteratorType<typename Const<T>::type>(Const<T>::begin(arr),
                                                     false,
                                                     arr.getDimensions(),
                                                     getMemoryLayout(arr));
    }

    /**
     * Create a const iterator to the beginning of the input array reference
     * @param ref result of operator[] to be used to create the iterator
     * @returns IteratorType<T const>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T,
             typename ARRAY_TYPE = typename GetReturnType<T>::type>
    static IteratorType<typename Const<T>::type> begin(matlab::data::ArrayElementTypedRef<Array, true>&& ref) {
        const matlab::data::TypedArray<T> arr = std::move(ref);
        return IteratorType<typename Const<T>::type>(Const<T>::begin(arr),
                                                     false,
                                                     arr.getDimensions(),
                                                     getMemoryLayout(arr));
    }
    
    /**
     * Create a const iterator to the beginning of the input array reference
     * @param ref result of operator[] to be used to create the iterator
     * @throw TypeMismatchException - if the array does not contain T's
     * @returns IteratorType<T const>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T, bool isConst>
    static IteratorType<typename Const<T>::type> cbegin(matlab::data::ArrayElementTypedRef<Array, isConst>&& ref) {
        const matlab::data::TypedArray<T> arr = std::move(ref);
        return IteratorType<typename Const<T>::type>(Const<T>::begin(arr),
                                                     false,
                                                     arr.getDimensions(),
                                                     getMemoryLayout(arr));
    }

    /**
     * Create a non-const iterator to the end of the input array
     * @param arr Array to be used to create the iterator
     * @returns IteratorType<T>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T>
    static IteratorType<typename NonConst<T>::type> end(matlab::data::Array& arr) {
        if (TypedArray<T>::type != arr.getType()) {
            detail::throwIfError(static_cast<int>(matlab::data::ExceptionType::InvalidDataType));
        }
        auto r = matlab::data::getWritableElements<T>(arr);
        return IteratorType<typename NonConst<T>::type>(std::move(r.end()),
                                                        true,
                                                        arr.getDimensions(),
                                                        getMemoryLayout(arr));
    }

    /**
     * Create a non-const iterator to the end of the input array
     * @param arr TypedArray to be used to create the iterator
     * @returns IteratorType<T>
     */
    template<typename T>
    static IteratorType<typename NonConst<T>::type> end(matlab::data::TypedArray<T>& arr) {
        return IteratorType<typename NonConst<T>::type>(NonConst<T>::end(arr),
                                                      true,
                                                      arr.getDimensions(),
                                                      getMemoryLayout(arr));
    }

    /**
     * Create a non-const iterator to the end of the input array reference
     * @param ref Reference<Array> to be used to create the iterator
     * @returns IteratorType<T>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T>
    static IteratorType<typename NonConst<T>::type> end(Reference<Array>& ref) {
        typedef int(*TypedArrayReferenceValidateTypeFcnPtr)(matlab::data::detail::ReferenceImpl*, int, bool);
        static const TypedArrayReferenceValidateTypeFcnPtr fcn = matlab::data::detail::resolveFunction<TypedArrayReferenceValidateTypeFcnPtr>
            (matlab::data::detail::FunctionType::TYPED_ARRAY_REFERENCE_VALIDATE_TYPE);
        matlab::data::detail::throwIfError(fcn(matlab::data::detail::Access::getImpl<matlab::data::detail::ReferenceImpl>(ref), static_cast<int>(GetArrayType<T>::type), true));
        auto r = matlab::data::getWritableElements<T>(ref);
        return IteratorType<typename NonConst<T>::type>(std::move(r.end()),
                                                        true,
                                                        ref.getDimensions(),
                                                        getMemoryLayout(ref));
    }

    /**
     * Create a non-const iterator to the end of the input array reference
     * @param arr TypedArrayRef to be used to create the iterator
     * @returns IteratorType<T>
     */
    template<typename T>
    static IteratorType<typename NonConst<T>::type> end(TypedArrayRef<T>& arr) {
        return IteratorType<typename NonConst<T>::type>(NonConst<T>::end(arr),
                                                      true,
                                                      arr.getDimensions(),
                                                      getMemoryLayout(arr));
    }

    /**
     * Create a non-const iterator to the beginning of the input array reference
     * @param ref result of operator[] to be used to create the iterator
     * @returns IteratorType<T>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T,
             typename ARRAY_TYPE = typename matlab::data::GetReturnType<T>::type>
    static IteratorType<typename NonConst<T>::type> end(matlab::data::ArrayElementTypedRef<Array, false>&& ref) {
        matlab::data::TypedArrayRef<T> arr = std::move(ref);
        return IteratorType<typename NonConst<T>::type>(NonConst<T>::end(arr),
                                                        true,
                                                        arr.getDimensions(),
                                                        getMemoryLayout(arr));
    }

    /**
     * Create a const iterator to the end of the input arr
     * @param arr TypedArray to be used to create the iterator
     * @returns IteratorType<T const>
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> end(const matlab::data::TypedArray<T>& arr) {
        return cend(arr);
    }

    /**
     * Create a const iterator to the end of the input arr
     * @param arr TypedArray to be used to create the iterator
     * @returns IteratorType<T const>
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> cend(const matlab::data::TypedArray<T>& arr) {
        return IteratorType<typename Const<T>::type>(Const<T>::end(arr),
                                                     true,
                                                     arr.getDimensions(),
                                                     getMemoryLayout(arr));
    }

    /**
     * Create a const iterator to the end of the input arr
     * @param arr TypedArray to be used to create the iterator
     * @returns IteratorType<T const>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> cend(const matlab::data::Array& arr) {
        if (TypedArray<T>::type != arr.getType()) {
            detail::throwIfError(static_cast<int>(matlab::data::ExceptionType::InvalidDataType));
        }
        const TypedArray<T> t_arr(arr);
        return end(t_arr);
    }

    /**
     * Create a const iterator to the end of the input arr
     * @param arr TypedArray to be used to create the iterator
     * @returns IteratorType<T const>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> cend(const matlab::data::Reference<matlab::data::Array>& arr) {
        const TypedArrayRef<T> t_arr(arr);
        return end(t_arr);
    }

    /**
     * Create a const iterator to the end of the input ref
     * @param ref TypedArrayRef to be used to create the iterator
     * @returns IteratorType<T const>
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> end(const matlab::data::TypedArrayRef<T>& arr) {
        return cend(arr);
    }

    /**
     * Create a const iterator to the end of the input ref
     * @param ref TypedArrayRef to be used to create the iterator
     * @returns IteratorType<T const>
     */
    template <typename T>
    static IteratorType<typename Const<T>::type> cend(const matlab::data::TypedArrayRef<T>& arr) {
        return IteratorType<typename Const<T>::type>(Const<T>::end(arr),
                                                     true,
                                                     arr.getDimensions(),
                                                     getMemoryLayout(arr));
    }

    /**
     * Create a const iterator to the beginning of the input array reference
     * @param ref result of operator[] to be used to create the iterator
     * @returns IteratorType<T const>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T>
    static IteratorType<typename Const<T>::type> end(matlab::data::ArrayElementTypedRef<Array, true>&& ref) {
        const matlab::data::TypedArray<T> arr = std::move(ref);
        return IteratorType<typename Const<T>::type>(Const<T>::end(arr),
                                                     true,
                                                     arr.getDimensions(),
                                                     getMemoryLayout(arr));
    }

    /**
     * Create a const iterator to the beginning of the input array reference
     * @param ref result of operator[] to be used to create the iterator
     * @returns IteratorType<T const>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T, bool isConst>
    static IteratorType<typename Const<T>::type> cend(matlab::data::ArrayElementTypedRef<Array, isConst>&& ref) {
        const matlab::data::TypedArray<T> arr = std::move(ref);
        return IteratorType<typename Const<T>::type>(Const<T>::end(arr),
                                                     true,
                                                     arr.getDimensions(),
                                                     getMemoryLayout(arr));
    }

    
    /**
     * Create a Range containing const iterators from the input TypedArray
     * @param arr TypedArray used to create the iterators
     * @returns Range<IteratorType<T const>>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T> 
    static Range<IteratorType, typename Const<T>::type>
    readOnlyElements(const Array& arr) {
        if (TypedArray<T>::type != arr.getType()) {
            detail::throwIfError(static_cast<int>(matlab::data::ExceptionType::InvalidDataType));
        }
        const TypedArray<T> t_arr(arr);
        return Range<IteratorType, typename Const<T>::type>(begin(t_arr),
                                                            end(t_arr));
    }

    /**
     * Create a Range containing const iterators from the input TypedArray
     * @param arr TypedArray used to create the iterators
     * @returns Range<IteratorType<T const>>
     */
    template<typename T> 
    static Range<IteratorType, typename Const<T>::type>
    readOnlyElements(const matlab::data::TypedArray<T>& arr) {
        return Range<IteratorType, typename Const<T>::type>(begin(arr),
                                                            end(arr));
    }

    /**
     * Create a Range containing const iterators from the input array reference
     * @param ref result of call to operator[] used to create the iterators
     * @returns Range<IteratorType<T const>>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T> 
    static Range<IteratorType, typename Const<T>::type>
    readOnlyElements(matlab::data::ArrayElementTypedRef<Array, true>&& ref) {
        const matlab::data::TypedArray<T> arr = std::move(ref);
        return Range<IteratorType, typename Const<T>::type>(begin(arr),
                                                            end(arr));
    }
    
    /**
     * Create a Range containing const iterators from the input array reference
     * @param ref result of call to operator[] used to create the iterators
     * @returns Range<IteratorType<T const>>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T> 
    static Range<IteratorType, typename Const<T>::type>
    readOnlyElements(matlab::data::ArrayElementTypedRef<Array, false>&& ref) {
        const matlab::data::TypedArrayRef<T> arr = std::move(ref);
        return Range<IteratorType, typename Const<T>::type>(cbegin(arr),
                                                            cend(arr));
    }

    /**
     * Create a Range containing const iterators from the input array reference
     * @param arr TypedArrayRef used to create the iterators
     * @returns Range<IteratorType<T const>>
     */
    template<typename T> 
    static Range<IteratorType, typename Const<T>::type>
    readOnlyElements(const matlab::data::TypedArrayRef<T>& arr) {
        return Range<IteratorType, typename Const<T>::type>(cbegin(arr),
                                                            cend(arr));
    }

    /**
     * Create a Range containing const iterators from the input array reference
     * @param arr TypedArrayRef used to create the iterators
     * @returns Range<IteratorType<T const>>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T> 
    static Range<IteratorType, typename Const<T>::type>
    readOnlyElements(const Reference<Array>& arr) {
        return Range<IteratorType, typename Const<T>::type>(cbegin<T>(arr),
                                                            cend<T>(arr));
    }

    /**
     * Create a Range containing non-const iterators from the input TypedArray
     * @param arr TypedArray used to create the iterators
     * @returns Range<IteratorType<T>>
     */
    template<typename T> 
    static Range<IteratorType, typename NonConst<T>::type>
    writableElements(matlab::data::TypedArray<T>& arr) {
        return Range<IteratorType, typename NonConst<T>::type>(begin(arr),
                                                               end(arr));
    }
        
    /**
     * Create a Range containing non-const iterators from the input TypedArrayRef
     * @param ref TypedArrayRef used to create the iterators
     * @returns Range<IteratorType<T>>
     */
    template<typename T> 
    static Range<IteratorType, typename NonConst<T>::type>
    writableElements(matlab::data::TypedArrayRef<T>& ref) {
        return Range<IteratorType, typename NonConst<T>::type>(begin<T>(ref),
                                                               end<T>(ref));
    }

    /**
     * Create a Range containing non-const iterators from the input array reference
     * @param ref result of operator[] used to create the iterators
     * @returns Range<IteratorType<T>>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T> 
    static Range<IteratorType, typename NonConst<T>::type>
    writableElements(matlab::data::Reference<Array>& ref) {
        typedef int(*TypedArrayReferenceValidateTypeFcnPtr)(matlab::data::detail::ReferenceImpl*, int, bool);
        static const TypedArrayReferenceValidateTypeFcnPtr fcn = matlab::data::detail::resolveFunction<TypedArrayReferenceValidateTypeFcnPtr>
            (matlab::data::detail::FunctionType::TYPED_ARRAY_REFERENCE_VALIDATE_TYPE);
        matlab::data::detail::throwIfError(fcn(matlab::data::detail::Access::getImpl<matlab::data::detail::ReferenceImpl>(ref), static_cast<int>(GetArrayType<T>::type), true));
        auto r = matlab::data::getWritableElements<T>(ref);
        return Range<IteratorType, typename NonConst<T>::type>
            (IteratorType<typename NonConst<T>::type>(std::move(r.begin()),
                                                      false,
                                                      ref.getDimensions(),
                                                      getMemoryLayout(ref)),
             IteratorType<typename NonConst<T>::type>(std::move(r.end()),
                                                      true,
                                                      ref.getDimensions(),
                                                      getMemoryLayout(ref)));             
    }

    /**
     * Create a Range containing non-const iterators from the input Array
     * @param arr Array used to create the iterators
     * @returns Range<IteratorType<T>>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T> 
    static Range<IteratorType, typename NonConst<T>::type>
    writableElements(matlab::data::Array& arr) {
        if (TypedArray<T>::type != arr.getType()) {
            detail::throwIfError(static_cast<int>(matlab::data::ExceptionType::InvalidDataType));
        }
        auto r = matlab::data::getWritableElements<T>(arr);
        return Range<IteratorType, typename NonConst<T>::type>
            (IteratorType<typename NonConst<T>::type>(std::move(r.begin()),
                                                      false,
                                                      arr.getDimensions(),
                                                      getMemoryLayout(arr)),
             IteratorType<typename NonConst<T>::type>(std::move(r.end()),
                                                      true,
                                                      arr.getDimensions(),
                                                      getMemoryLayout(arr)));             
    }

    /**
     * Create a Range containing non-const iterators from the input array reference
     * @param ref result of operator[] used to create the iterators
     * @returns Range<IteratorType<T>>
     * @throw TypeMismatchException - if the array does not contain T's
     */
    template<typename T, bool isConst> 
    static Range<IteratorType, typename NonConst<T>::type>
    writableElements(matlab::data::ArrayElementTypedRef<Array, isConst>&& ref) {
        static_assert(!isConst, "Cannot get writable elements from a const array");
        auto arr = NonConst<T>::convertRef(std::move(ref));
        return writableElements<T>(arr);
    }
};

}
}
}

#endif
