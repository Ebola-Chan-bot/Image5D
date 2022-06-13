/* Copyright 2014-2020 The MathWorks, Inc. */

#ifndef ParamTuningTgtAppSvcCIntrf_h
#define ParamTuningTgtAppSvcCIntrf_h

#include "ParamTuningTgtAppSvc_dll.hpp"

#ifdef SL_INTERNAL
  #include "simulinkcoder_capi/rtw_modelmap.h"
  #include "sl_pointer_util/BasicOperations.hpp"
#else
  #include "rtw_modelmap.h"
  #include "mwslpointerutil.h"
#endif

PARAMTUNINGTGTAPPSVC_API_C int  startParamTuningTgtAppSvc();
PARAMTUNINGTGTAPPSVC_API_C int  terminateParamTuningTgtAppSvc();
PARAMTUNINGTGTAPPSVC_API_C void tunePendingParameterChanges(rtwCAPI_ModelMappingInfo *pMMI);

#endif
