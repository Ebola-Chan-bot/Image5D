/* Copyright 2017-2020 The MathWorks, Inc. */

#ifndef __MEX_API_ADAPTER_HPP__
#define __MEX_API_ADAPTER_HPP__

#include "MatlabDataArray.hpp"
#include "mex.hpp"
#include "mexEngineUtilImpl.hpp"
#include "mexExceptionType.hpp"
#include <vector>
#include <iostream>
#include "assert.h"
#include "../mexMatlabEngine.hpp"

LIBMWMEX_API_EXTERN_C{

    void mexApiFeval(void*, const int, void**, const int, void**, const char*, int*, void**, void*, void*, void(*)(void*, const char16_t*, size_t), void(*)(void*));

    void* mexApiGetVariable(void*, const char*, const char16_t*, int*, void**);

    void* mexApiSetVariable(void*, void*, const char*, const char16_t*, int*, void**);

    void* mexApiGetProperty(void*, void*, size_t, const char16_t*, int*, void**);

    void* mexApiSetProperty(void*, void*, size_t, const char16_t*, void*, int*, void**);

    uintptr_t mexApiFevalAsync(void*, const int, const int, void**, const char*, bool, void(*)(void*, int, bool, matlab::data::impl::ArrayImpl**), void(*)(void*, int, bool, int, void*), void*, void*, void*, void(*)(void*, const char16_t*, size_t), void(*)(void*));

    uintptr_t mexApiEvalAsync(void*, const char16_t*, void(*)(void*), void(*)(void*, int, void*), void*, void*, void*, void(*)(void*, const char16_t*, size_t), void(*)(void*));

    uintptr_t mexApiGetVariableAsync(void*, const char*, const char16_t*, void(*)(void*, int, bool, matlab::data::impl::ArrayImpl**), void(*)(void*, int, bool, int, void*), void*);

    uintptr_t mexApiSetVariableAsync(void*, void*, const char*, const char16_t*, void(*)(void*, int, bool, matlab::data::impl::ArrayImpl**), void(*)(void*, int, bool, int, void*), void*);

    uintptr_t mexApiGetPropertyAsync(void*, void*, size_t, const char16_t*, void(*)(void*, int, bool, matlab::data::impl::ArrayImpl**), void(*)(void*, int, bool, int, void*), void*);

    uintptr_t mexApiSetPropertyAsync(void*, void*, size_t, const char16_t*, void*, void(*)(void*, int, bool, matlab::data::impl::ArrayImpl**), void(*)(void*, int, bool, int, void*), void*);

    bool mexApiCancelFevalWithCompletion(uintptr_t, bool);

    void mexApiToWaitForPendingfevalEvalToComplete(uintptr_t);
}

namespace matlab {
    namespace engine {
        namespace detail {


            template<typename T>
            inline void validateTIsSupported() {
                using U = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
                static_assert(
                    std::is_same<U, bool>::value
                    || std::is_same<U, int>::value
                    || std::is_same<U, int8_t>::value
                    || std::is_same<U, int16_t>::value
                    || std::is_same<U, int32_t>::value
                    || std::is_same<U, int64_t>::value
                    || std::is_same<U, uint8_t>::value
                    || std::is_same<U, uint16_t>::value
                    || std::is_same<U, uint32_t>::value
                    || std::is_same<U, uint64_t>::value
                    || std::is_same<U, float>::value
                    || std::is_same<U, double>::value, "Attempted to use unsupported types.");
            }

            template<class T>
            matlab::data::Array createRhs(matlab::data::ArrayFactory& factory, T&& value) {
                validateTIsSupported<T>();
                using U = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
                return factory.createArray<U>({ 1, 1 }, { value });
            }

            template<typename T, typename A>
            matlab::data::Array createRhs(matlab::data::ArrayFactory& factory, std::vector <T, A>&& value) {
                validateTIsSupported<T>();
                return factory.createArray({ 1, value.size() }, value.begin(), value.end());
            }

            template <std::size_t ...Ints>
            struct index_sequence {
                using value_type = std::size_t;
                static std::size_t size() { return sizeof...(Ints); }
            };

            template<std::size_t N, std::size_t... Values>
            struct make_index_sequence_impl {
                using type = typename make_index_sequence_impl<N - 1, Values..., sizeof...(Values)>::type;
            };

            template<std::size_t... Values>
            struct make_index_sequence_impl < 0, Values... > {
                using type = index_sequence < Values... >;
            };

            template<std::size_t N>
            using make_index_sequence = typename make_index_sequence_impl<N>::type;

            template<typename T>
            struct createLhs {
                static const size_t nlhs = 1;

                T operator()(std::vector<matlab::data::Array>&& lhs) const {
                    if (lhs.empty()) {
                        throw matlab::engine::TypeConversionException("The result is empty.");
                    }

                    T value;
                    try {
                        value = (*this)(matlab::data::TypedArray<T>(std::move(lhs.front())));
                    }
                    catch (const std::exception& e) {
                        throw matlab::engine::TypeConversionException(e.what());
                    }
                    return value;
                }

                T operator()(matlab::data::TypedArray<T> lhs) const {
                    validateTIsSupported<T>();
                    auto const begin = lhs.begin();
                    auto const end = lhs.end();
                    if (begin == end) {
                        throw matlab::engine::TypeConversionException("The result is empty.");
                    }
                    return *begin;
                }
            };

            template<>
            struct createLhs < void > {
                static const size_t nlhs = 0;
                void operator()(std::vector<matlab::data::Array>&& lhs) const {}
            };

            template<typename... TupleTypes>
            struct createLhs < std::tuple<TupleTypes...> > {
                static const size_t nlhs = sizeof...(TupleTypes);
                using T = std::tuple < TupleTypes... >;

                T operator()(std::vector<matlab::data::Array>&& lhs) const {
                    //we are not validating the LHS here as it can be any combinations of types for std::tuple.
                    if (lhs.size() < sizeof...(TupleTypes)) { throw std::runtime_error(""); }
                    return (*this)(std::move(lhs), detail::make_index_sequence<sizeof...(TupleTypes)>());
                }

            private:
                template<size_t Index>
                using TupleElement = typename std::remove_cv<typename std::remove_reference<typename std::tuple_element<Index, std::tuple<TupleTypes...> >::type>::type>::type;

                template<size_t... IndexList>
                std::tuple <TupleTypes...> operator()(std::vector<matlab::data::Array>&& lhs, detail::index_sequence<IndexList...>) const {
                    return std::tuple <TupleTypes...>(createLhs<TupleElement<IndexList>>()(std::move(lhs[IndexList]))...);
                }
            };
        }
    }
}

void implDeleter(matlab::data::impl::ArrayImpl** impl);

inline void writeToStreamBuffer(void* buffer, const char16_t* stream, size_t n) {
    std::shared_ptr<matlab::engine::StreamBuffer>* output = reinterpret_cast<std::shared_ptr<matlab::engine::StreamBuffer>*>(buffer);
    output->get()->sputn(stream, n);
}

inline void deleteStreamBufferImpl(void* impl) {
    delete static_cast<std::shared_ptr<matlab::engine::StreamBuffer>*>(impl);
}


template<class ReturnType, typename...RhsArgs>
ReturnType matlab::engine::MATLABEngine::feval(const std::u16string &function,
                                               const std::shared_ptr<StreamBuffer> &output,
                                               const std::shared_ptr<StreamBuffer> &error,
                                               RhsArgs&&... rhsArgs) {
    matlab::data::ArrayFactory factory;
    std::vector<matlab::data::Array> rhsList({
        detail::createRhs(factory, std::forward<RhsArgs>(rhsArgs))...
    });

    auto const nlhs = detail::createLhs<ReturnType>::nlhs;
    std::vector<matlab::data::Array> f = feval(function, nlhs, rhsList, output, error);
    detail::createLhs<ReturnType> lhsFactory;
    return lhsFactory(std::move(f));
}

template<class ReturnType, typename...RhsArgs>
ReturnType matlab::engine::MATLABEngine::feval(const std::string &function,
                                               const std::shared_ptr<StreamBuffer> &output,
                                               const std::shared_ptr<StreamBuffer> &error,
                                               RhsArgs&&... rhsArgs) {
    return feval(std::u16string(function.begin(), function.end()), output, error, std::forward<RhsArgs>(rhsArgs)...);
}

template<class ReturnType, typename...RhsArgs>
ReturnType matlab::engine::MATLABEngine::feval(const std::u16string &function,
                                               RhsArgs&&... rhsArgs) {
    const std::shared_ptr<StreamBuffer> defaultStream;
    return feval<ReturnType>(function, defaultStream, defaultStream, std::forward<RhsArgs>(rhsArgs)...);
}

template<class ReturnType, typename...RhsArgs>
ReturnType matlab::engine::MATLABEngine::feval(const std::string &function,
                                               RhsArgs&&... rhsArgs) {
    return feval<ReturnType>(std::u16string(function.begin(), function.end()), std::forward<RhsArgs>(rhsArgs)...);
}








namespace {
    template<typename T>
    void set_promise_exception(void *p, int excTypeNumber, void* msg){
        std::promise<T>* prom = reinterpret_cast<std::promise<T>*>(p);
        try {
            throwIfError(excTypeNumber, msg);
        }
        catch (const matlab::engine::MATLABSyntaxException& ex) {
            prom->set_exception(std::make_exception_ptr<matlab::engine::MATLABSyntaxException>(ex));
        }
        catch (const matlab::engine::MATLABExecutionException& ex) {
            prom->set_exception(std::make_exception_ptr<matlab::engine::MATLABExecutionException>(ex));
        }
        catch (const matlab::engine::CancelledException& ex) {
            prom->set_exception(std::make_exception_ptr<matlab::engine::CancelledException>(ex));
        }
        catch (const matlab::engine::InterruptedException& ex) {
            prom->set_exception(std::make_exception_ptr<matlab::engine::InterruptedException>(ex));
        }
        catch (const matlab::engine::MATLABException& ex) {
            prom->set_exception(std::make_exception_ptr<matlab::engine::MATLABException>(ex));
        }
        catch (const std::exception& ex) {
            prom->set_exception(std::make_exception_ptr<std::exception>(ex));
        }
        delete prom;
    }

    inline void set_eval_promise_data(void *p) {
        std::promise<void>* prom = reinterpret_cast<std::promise<void>*>(p);
        prom->set_value();
        delete prom;
    }

    inline void set_eval_promise_exception(void *p, int excTypeNumber, void* msg) {
        std::promise<void>* prom = reinterpret_cast<std::promise<void>*>(p);
        try {
            throwIfError(excTypeNumber, msg);
        }
        catch (const matlab::engine::MATLABSyntaxException& ex) {
            prom->set_exception(std::make_exception_ptr<matlab::engine::MATLABSyntaxException>(ex));
        }
        catch (const matlab::engine::MATLABExecutionException& ex) {
            prom->set_exception(std::make_exception_ptr<matlab::engine::MATLABExecutionException>(ex));
        }
        catch (const matlab::engine::CancelledException& ex) {
            prom->set_exception(std::make_exception_ptr<matlab::engine::CancelledException>(ex));
        }
        catch (const matlab::engine::InterruptedException& ex) {
            prom->set_exception(std::make_exception_ptr<matlab::engine::InterruptedException>(ex));
        }
        catch (const matlab::engine::MATLABException& ex) {
            prom->set_exception(std::make_exception_ptr<matlab::engine::MATLABException>(ex));
        }
        catch (const std::exception& ex) {
            prom->set_exception(std::make_exception_ptr<std::exception>(ex));
        }
        delete prom;
    }

    inline void set_feval_promise_data(void *p, int nlhs, bool straight, matlab::data::impl::ArrayImpl** plhs) {

        if (nlhs == 0 && straight) {
            std::promise<void>* prom = reinterpret_cast<std::promise<void>*>(p);
            prom->set_value();
            delete prom;
            return;
        }

        if (nlhs == 1 && straight) {
            std::promise<matlab::data::Array>* prom = reinterpret_cast<std::promise<matlab::data::Array>*>(p);
            matlab::data::Array v_ = matlab::data::detail::Access::createObj<matlab::data::Array>(plhs[0]);
            prom->set_value(v_);
            delete prom;
            return;
        }

        std::promise<std::vector<matlab::data::Array> >* prom = reinterpret_cast<std::promise<std::vector<matlab::data::Array> >*>(p);
        std::vector<matlab::data::Array> result;
        for (int i = 0; i < nlhs; i++) {
            matlab::data::Array v_ = matlab::data::detail::Access::createObj<matlab::data::Array>(plhs[i]);
            result.push_back(v_);
        }
        prom->set_value(result);
        delete prom;
    }

    template<class T>
    void set_exception(T p, std::exception_ptr e) {
        p->set_exception(e);
    }

    inline void set_feval_promise_exception(void *p, int nlhs, bool straight, int excTypeNumber, void* msg) {
        if (nlhs == 0 && straight) {
            set_promise_exception<void>(p, excTypeNumber, msg);
        }
        else if (nlhs == 1 && straight) {
            set_promise_exception<matlab::data::Array>(p, excTypeNumber, msg);
        }
        else {
            set_promise_exception<std::vector<matlab::data::Array>>(p, excTypeNumber, msg);
        }
    }
}



template<class ReturnType, typename...RhsArgs>
matlab::engine::FutureResult<ReturnType> matlab::engine::MATLABEngine::fevalAsync(const std::u16string &function,
    const std::shared_ptr<matlab::engine::StreamBuffer> &output,
    const std::shared_ptr<matlab::engine::StreamBuffer> &error,
    RhsArgs&&... rhsArgs) {
    matlab::data::ArrayFactory factory;
    std::vector<matlab::data::Array> rhsList({
        detail::createRhs(factory, std::forward<RhsArgs>(rhsArgs))...
    });

    auto const nlhs = detail::createLhs<ReturnType>::nlhs;

    size_t nrhs = rhsList.size();
    matlab::data::impl::ArrayImpl** argsImpl = new matlab::data::impl::ArrayImpl*[nrhs];
    size_t i = 0;
    for (auto e : rhsList) {
        argsImpl[i++] = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(e);
    }

    matlab::engine::FutureResult<std::vector<matlab::data::Array>> f = fevalAsync(function, nlhs, rhsList, output, error);

    // c++11 lambdas do not correctly handle move operations...
    // when c++14 is available, this should be:
    // auto convertToResultType = [copyableF = std::move(f)]()->ReturnType { ....... };
    auto copyableF = std::make_shared<FutureResult<std::vector<matlab::data::Array>>>(std::move(f));
    auto convertToResultType = [copyableF]() ->ReturnType {
        std::vector<matlab::data::Array> vec = copyableF->get();
        detail::createLhs<ReturnType> lhsFactory;
        return lhsFactory(std::move(vec));
    };

    std::future<ReturnType> future = std::async(std::launch::deferred, std::move(convertToResultType));
    return matlab::engine::FutureResult<ReturnType>(std::move(future), copyableF->getTaskReference());
}

template<class ReturnType, typename...RhsArgs>
matlab::engine::FutureResult<ReturnType> matlab::engine::MATLABEngine::fevalAsync(const std::string &function,
    const std::shared_ptr<matlab::engine::StreamBuffer> &output,
    const std::shared_ptr<matlab::engine::StreamBuffer> &error,
    RhsArgs&&... rhsArgs) {
    return fevalAsync<ReturnType>(std::u16string(function.begin(), function.end()), output, error, std::forward<RhsArgs>(rhsArgs)...);
}

template<class ReturnType, typename...RhsArgs>
matlab::engine::FutureResult<ReturnType> matlab::engine::MATLABEngine::fevalAsync(const std::u16string &function,
    RhsArgs&&... rhsArgs) {
    const std::shared_ptr<StreamBuffer> defaultStream;
    return fevalAsync<ReturnType>(function, defaultStream, defaultStream, std::forward<RhsArgs>(rhsArgs)...);
}

template<class ReturnType, typename...RhsArgs>
matlab::engine::FutureResult<ReturnType> matlab::engine::MATLABEngine::fevalAsync(const std::string &function,
    RhsArgs&&... rhsArgs) {
    return fevalAsync<ReturnType>(std::u16string(function.begin(), function.end()), std::forward<RhsArgs>(rhsArgs)...);
}



#endif
