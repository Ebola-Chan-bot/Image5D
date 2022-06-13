// Copyright 2017-2020 The MathWorks, Inc.

#ifndef MATLAB_DATA_VISITORS_HPP
#define MATLAB_DATA_VISITORS_HPP

#include <complex>
#include <type_traits>
 
#include "ArrayType.hpp"
#include "TypedArray.hpp"
#include "Struct.hpp"
#include "Object.hpp"
#include "ObjectArray.hpp"
#include "StructArray.hpp"
#include "EnumArray.hpp"
#include "CharArray.hpp"
#include "SparseArray.hpp"
#include "Exception.hpp"
#include "TypedArrayRef.hpp"
#include "SparseArrayRef.hpp"

#if (__cplusplus >= 201703L) || (_MSVC_LANG >= 201703L)
    // Using C++17, where std::is_invocable is available. 
    namespace matlab { namespace data { namespace detail {
        template<typename... Args> using is_invocable = std::is_invocable<Args...>;
    }}}
#else
    // Unable to find a usable invocable implementation.
    // Passing Array to the visitor by non-const lvalue reference will be disabled but passing
    // - by value
    // - as const&
    // - as &&
    // - as const&&
    // is still supported.
    namespace matlab { namespace data { namespace detail {
        template<typename... Args> using is_invocable = std::false_type;
    }}}
#endif

#include <exception>

namespace matlab {
    namespace data {
        namespace detail {
            template<typename T> struct is_reference : public std::false_type {};
            template<typename T> struct is_reference<matlab::data::Reference<T>> : public std::true_type {};

            #define AV_MSG_MUST_BE_ARRAY_TYPE "The array argument must be a matlab::data::Array type."

            /// If the visitor allows an rvalue reference, this version will be invoked.
            template<typename A, typename V, bool = matlab::data::detail::is_invocable<V, typename std::remove_reference<A>::type>::value>
            struct InvokeVisitor
            {
                static_assert(std::is_base_of<Array, typename std::remove_reference<A>::type>::value 
                              || is_reference<typename std::remove_reference<A>::type>::value,
                              AV_MSG_MUST_BE_ARRAY_TYPE);

                auto operator()(A a, V visitor) const -> decltype(std::forward<V>(visitor)(std::move(a)))
                {
                    return std::forward<V>(visitor)(std::move(a));
                }                    
            };

            /// If the visitor does not allow an rvalue reference, this version will be invoked.
            template<typename A, typename V>
            struct InvokeVisitor<A, V, false>
            {
                static_assert(std::is_base_of<Array, typename std::remove_reference<A>::type>::value 
                              || is_reference<typename std::remove_reference<A>::type>::value,
                              AV_MSG_MUST_BE_ARRAY_TYPE);

                auto operator()(A a, V visitor) const -> decltype(std::forward<V>(visitor)(a))
                {
                    return std::forward<V>(visitor)(a);
                }
            };

            template<typename A, typename V>
            auto invokeVisitor(A&& a, V&& visitor)
            -> decltype(InvokeVisitor<decltype(a), decltype(visitor)>()(std::forward<A>(a), std::forward<V>(visitor)))
            {
                return InvokeVisitor<decltype(a), decltype(visitor)>()(std::forward<A>(a), std::forward<V>(visitor));
            }
            
            #define AV_MSG_INCONSISTENT_TYPE "Visitor is returning inconsistent types."
            
            template<typename A, typename V, bool = std::is_base_of<Array, typename std::remove_reference<A>::type>::value>
            struct ApplyVisitor
            {
                using return_type = decltype(invokeVisitor(std::declval<CharArray>(), std::declval<V>()));

                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<MATLABString>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<bool>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<double>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<float>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<int8_t>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<uint8_t>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<int16_t>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<uint16_t>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<int32_t>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<uint32_t>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<int64_t>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<uint64_t>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<std::complex<double>>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<std::complex<float>>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<std::complex<int8_t>>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<std::complex<uint8_t>>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<std::complex<int16_t>>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<std::complex<uint16_t>>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<std::complex<int32_t>>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<std::complex<uint32_t>>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<std::complex<int64_t>>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<std::complex<uint64_t>>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(TypedArray<Array>(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(StructArray(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);
                static_assert(std::is_convertible<decltype(invokeVisitor(EnumArray(std::declval<A>()),std::declval<V>())),return_type>::value,
                              AV_MSG_INCONSISTENT_TYPE);

                return_type operator()(A a, V visitor) const
                {
                    switch (a.getType()) {
                      case ArrayType::CHAR:                  return invokeVisitor(CharArray(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::MATLAB_STRING:         return invokeVisitor(TypedArray<MATLABString>(std::forward<A>(a)), std::forward<V>(visitor));            
                      case ArrayType::LOGICAL:               return invokeVisitor(TypedArray<bool>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::DOUBLE:                return invokeVisitor(TypedArray<double>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::SINGLE:                return invokeVisitor(TypedArray<float>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::INT8:                  return invokeVisitor(TypedArray<int8_t>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::UINT8:                 return invokeVisitor(TypedArray<uint8_t>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::INT16:                 return invokeVisitor(TypedArray<int16_t>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::UINT16:                return invokeVisitor(TypedArray<uint16_t>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::INT32:                 return invokeVisitor(TypedArray<int32_t>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::UINT32:                return invokeVisitor(TypedArray<uint32_t>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::INT64:                 return invokeVisitor(TypedArray<int64_t>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::UINT64:                return invokeVisitor(TypedArray<uint64_t>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::COMPLEX_DOUBLE:        return invokeVisitor(TypedArray<std::complex<double>>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::COMPLEX_SINGLE:        return invokeVisitor(TypedArray<std::complex<float>>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::COMPLEX_INT8:          return invokeVisitor(TypedArray<std::complex<int8_t>>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::COMPLEX_UINT8:         return invokeVisitor(TypedArray<std::complex<uint8_t>>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::COMPLEX_INT16:         return invokeVisitor(TypedArray<std::complex<int16_t>>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::COMPLEX_UINT16:        return invokeVisitor(TypedArray<std::complex<uint16_t>>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::COMPLEX_INT32:         return invokeVisitor(TypedArray<std::complex<int32_t>>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::COMPLEX_UINT32:        return invokeVisitor(TypedArray<std::complex<uint32_t>>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::COMPLEX_INT64:         return invokeVisitor(TypedArray<std::complex<int64_t>>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::COMPLEX_UINT64:        return invokeVisitor(TypedArray<std::complex<uint64_t>>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::CELL:                  return invokeVisitor(TypedArray<Array>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::STRUCT:                return invokeVisitor(StructArray(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::VALUE_OBJECT:          return invokeVisitor(ObjectArray(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::HANDLE_OBJECT_REF:     return invokeVisitor(ObjectArray(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::ENUM:                  return invokeVisitor(EnumArray(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::SPARSE_LOGICAL:        return invokeVisitor(SparseArray<bool>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::SPARSE_DOUBLE:         return invokeVisitor(SparseArray<double>(std::forward<A>(a)), std::forward<V>(visitor));
                      case ArrayType::SPARSE_COMPLEX_DOUBLE: return invokeVisitor(SparseArray<std::complex<double>>(std::forward<A>(a)), std::forward<V>(visitor));
                      
                      default:
                        throw InvalidArrayTypeException("Invalid type of array passed to apply_visitor.");
                        break;

                    } // switch
                }
            };
            
            template<typename A, typename V>
            struct ApplyVisitor<A, V, false>
            {
                using return_type = decltype(invokeVisitor(std::declval<CharArray>(), std::declval<V>()));

                return_type operator()(A a, V visitor) const
                {
                    Array array = std::forward<A>(a);
                    return ApplyVisitor<Array&&, V>()(std::move(array), std::forward<V>(visitor));
                }
            };
        }

        /**
         * apply_visitor allows the caller to pass in an instance of Array (or one of
         * its subclasses) and a visitor functor, and invokes the operator() method, which must be
         * defined in the user-defined functor, with the appropriate concrete array type. The array,
         * and the visitor, can be passed in by value, by const lvalue ref, by nonconst lvalue ref,
         * or by rvalue ref. They will be copied, modified, or moved accordingly.
         *
         * The return type of a functor is void by default. Functors can optionally return values
         * of any arbitrary type. Note that the return type for all overloads must be convertible
         * to the type specified in the signature.
         *  
         * @param an Array to pass into the functor
         * @param visitor functor class
         * @return result type defined inside the functor or void, if none has been defined.
         * @throw UnknownMatlabArrayDataType exception
         */
        template<typename A, typename V>
        auto apply_visitor(A&& a, V&& visitor)
          -> decltype(detail::ApplyVisitor<decltype(a), decltype(visitor)>()(std::forward<A>(a), std::forward<V>(visitor)))
        {
            return detail::ApplyVisitor<decltype(a), decltype(visitor)>()(std::forward<A>(a), std::forward<V>(visitor));
        }
         

        /**
         * apply_numeric_visitor is like apply_visitor, but only applies to numeric types
         * (including char, logical, and complex). If types like StructArray need to be
         * covered, apply_visitor should be used. This is the lvalue ref version.
         *
         * @param an Array to pass into the functor
         * @param visitor functor class
         * @return result_type defined inside the functor, or void if none has been defined
         * @throw UnknownMatlabArrayDataType exception
         */
         
        template<typename A, typename V>
        auto apply_numeric_visitor(A&& a, V&& visitor,
          typename std::enable_if<std::is_base_of<Array, typename std::remove_reference<A>::type>::value, const void *>::type = nullptr) 
          -> decltype (detail::invokeVisitor(std::declval<CharArray>(), std::forward<V>(visitor)))
        {
            switch (a.getType()) {
              case ArrayType::CHAR:           return detail::invokeVisitor(CharArray(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::LOGICAL:        return detail::invokeVisitor(TypedArray<bool>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::DOUBLE:         return detail::invokeVisitor(TypedArray<double>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::SINGLE:         return detail::invokeVisitor(TypedArray<float>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::INT8:           return detail::invokeVisitor(TypedArray<int8_t>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::UINT8:          return detail::invokeVisitor(TypedArray<uint8_t>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::INT16:          return detail::invokeVisitor(TypedArray<int16_t>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::UINT16:         return detail::invokeVisitor(TypedArray<uint16_t>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::INT32:          return detail::invokeVisitor(TypedArray<int32_t>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::UINT32:         return detail::invokeVisitor(TypedArray<uint32_t>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::INT64:          return detail::invokeVisitor(TypedArray<int64_t>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::UINT64:         return detail::invokeVisitor(TypedArray<uint64_t>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::COMPLEX_DOUBLE: return detail::invokeVisitor(TypedArray<std::complex<double>>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::COMPLEX_SINGLE: return detail::invokeVisitor(TypedArray<std::complex<float>>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::COMPLEX_INT8:   return detail::invokeVisitor(TypedArray<std::complex<int8_t>>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::COMPLEX_UINT8:  return detail::invokeVisitor(TypedArray<std::complex<uint8_t>>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::COMPLEX_INT16:  return detail::invokeVisitor(TypedArray<std::complex<int16_t>>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::COMPLEX_UINT16: return detail::invokeVisitor(TypedArray<std::complex<uint16_t>>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::COMPLEX_INT32:  return detail::invokeVisitor(TypedArray<std::complex<int32_t>>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::COMPLEX_UINT32: return detail::invokeVisitor(TypedArray<std::complex<uint32_t>>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::COMPLEX_INT64:  return detail::invokeVisitor(TypedArray<std::complex<int64_t>>(std::forward<A>(a)), std::forward<V>(visitor));
              case ArrayType::COMPLEX_UINT64: return detail::invokeVisitor(TypedArray<std::complex<uint64_t>>(std::forward<A>(a)), std::forward<V>(visitor));

              default:
                throw InvalidArrayTypeException("Invalid type of array passed to apply_numeric_visitor.");
                break;

            } /* switch */
        } /* apply_numeric_visitor */

        template<typename A, typename V>
        auto apply_numeric_visitor(A&& a, V&& visitor, 
          typename std::enable_if<!std::is_base_of<Array, typename std::remove_reference<A>::type>::value, const void *>::type = nullptr)
          -> decltype(apply_visitor(std::move(a), std::forward<V>(visitor)))
        {
            Array array = a;
            return apply_visitor(std::move(array), std::forward<V>(visitor));
        }
        
        /**
          * apply_visitor_ref allows the caller to modify an element of a matlab::data::Array using a 
          * passed-in functor whose operator() for the appropriate type will be invoked.
          * @param a matlab::data::ArrayRef pointing to an element that is to be modified
          * @param visitor functor class whose operator() will be invoked
          * @return the return type defined within the functor, or void if none has been defined
          */
        template<typename V>
        auto apply_visitor_ref(const ArrayRef& a, V visitor) 
          -> decltype (visitor(static_cast<CharArrayRef>(a)))
        {
            switch (a.getType()) {
              case ArrayType::CHAR:                  return visitor(static_cast<CharArrayRef>(a));
              case ArrayType::MATLAB_STRING:         return visitor(static_cast<TypedArrayRef<MATLABString>>(a));
              case ArrayType::LOGICAL:               return visitor(static_cast<TypedArrayRef<bool>>(a));
              case ArrayType::DOUBLE:                return visitor(static_cast<TypedArrayRef<double>>(a));
              case ArrayType::SINGLE:                return visitor(static_cast<TypedArrayRef<float>>(a));
              case ArrayType::INT8:                  return visitor(static_cast<TypedArrayRef<int8_t>>(a));
              case ArrayType::UINT8:                 return visitor(static_cast<TypedArrayRef<uint8_t>>(a));
              case ArrayType::INT16:                 return visitor(static_cast<TypedArrayRef<int16_t>>(a));
              case ArrayType::UINT16:                return visitor(static_cast<TypedArrayRef<uint16_t>>(a));
              case ArrayType::INT32:                 return visitor(static_cast<TypedArrayRef<int32_t>>(a));
              case ArrayType::UINT32:                return visitor(static_cast<TypedArrayRef<uint32_t>>(a));
              case ArrayType::INT64:                 return visitor(static_cast<TypedArrayRef<int64_t>>(a));
              case ArrayType::UINT64:                return visitor(static_cast<TypedArrayRef<uint64_t>>(a));
              case ArrayType::COMPLEX_DOUBLE:        return visitor(static_cast<TypedArrayRef<std::complex<double>>>(a));
              case ArrayType::COMPLEX_SINGLE:        return visitor(static_cast<TypedArrayRef<std::complex<float>>>(a));
              case ArrayType::COMPLEX_INT8:          return visitor(static_cast<TypedArrayRef<std::complex<int8_t>>>(a));
              case ArrayType::COMPLEX_UINT8:         return visitor(static_cast<TypedArrayRef<std::complex<uint8_t>>>(a));
              case ArrayType::COMPLEX_INT16:         return visitor(static_cast<TypedArrayRef<std::complex<int16_t>>>(a));
              case ArrayType::COMPLEX_UINT16:        return visitor(static_cast<TypedArrayRef<std::complex<uint16_t>>>(a));
              case ArrayType::COMPLEX_INT32:         return visitor(static_cast<TypedArrayRef<std::complex<int32_t>>>(a));
              case ArrayType::COMPLEX_UINT32:        return visitor(static_cast<TypedArrayRef<std::complex<uint32_t>>>(a));
              case ArrayType::COMPLEX_INT64:         return visitor(static_cast<TypedArrayRef<std::complex<int64_t>>>(a));
              case ArrayType::COMPLEX_UINT64:        return visitor(static_cast<TypedArrayRef<std::complex<uint64_t>>>(a));
              case ArrayType::CELL:                  return visitor(static_cast<TypedArrayRef<Array>>(a));
              case ArrayType::STRUCT:                return visitor(static_cast<StructArrayRef>(a));
              case ArrayType::VALUE_OBJECT:          return visitor(static_cast<TypedArrayRef<Object>>(a));
              case ArrayType::HANDLE_OBJECT_REF:     return visitor(static_cast<TypedArrayRef<Object>>(a));
              case ArrayType::ENUM:                  return visitor(static_cast<EnumArrayRef>(a));
              case ArrayType::SPARSE_LOGICAL:        return visitor(static_cast<SparseArrayRef<bool>>(a));
              case ArrayType::SPARSE_DOUBLE:         return visitor(static_cast<SparseArrayRef<double>>(a));
              case ArrayType::SPARSE_COMPLEX_DOUBLE: return visitor(static_cast<SparseArrayRef<std::complex<double>>>(a));
                
              default:
                throw InvalidArrayTypeException("Invalid type of array passed to apply_visitor_ref.");
                break;

            } /* switch */
        } /* apply_visitor_ref */
    }
}

#endif
