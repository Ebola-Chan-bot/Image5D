/* Copyright 2017-2018 The MathWorks, Inc. */

#ifndef __MEX_FUNCTION_ADAPTER_HPP__
#define __MEX_FUNCTION_ADAPTER_HPP__

#include "MatlabDataArray.hpp"
#include "mex.hpp"
#include "mexEngineUtilImpl.hpp"
#include <vector>
#include <iostream>
#include "assert.h"

LIBMWMEX_API_EXTERN_C {

    void* cppmex_getEngine(void*);

    void* cppmex_mexLock(void*);

    void* cppmex_mexUnlock(void*);

    void* cppmex_mexLock_with_error_check(void*, int*);

    void* cppmex_mexUnlock_with_error_check(void*, int*);

    const char16_t* cppmex_getFunctionName(void*);

    void mexReleaseMemory(char*, char16_t*);

    void* mexGetFunctionImpl();

    void mexDestroyFunctionImpl(void*);
}


template <typename T>
matlab::mex::Function * mexCreatorUtil() {
    static_assert(std::is_base_of<matlab::mex::Function, T>::value, "MexFunction class must be a subclass of matlab::mex::Function.");
    matlab::mex::Function* mexFunction = new T();
    return mexFunction;
}

void mexDestructorUtil(matlab::mex::Function* t);

void mexHandleException(void (*callbackErrHandler)(const char*, const char*));

class MexFunction;

EXTERN_C
void mexFunction();

MEXFUNCTION_LINKAGE
void* mexCreateMexFunction(void (*callbackErrHandler)(const char*, const char*));

MEXFUNCTION_LINKAGE
void mexDestroyMexFunction(void* mexFunc,
    void (*callbackErrHandler)(const char*, const char*));

MEXFUNCTION_LINKAGE
void mexFunctionAdapter(int nlhs_,
    int nlhs,
    int nrhs,
    void* vrhs[],
    void* mFun,
    void (*callbackOutput)(int, void**),
    void (*callbackErrHandler)(const char*, const char*));

/*** matlab::mex::Function ***/









#endif
