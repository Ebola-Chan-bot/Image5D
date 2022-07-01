#pragma once
#include <Mex工具.h>
#include "Image5D.h"
StructArray 异常转换(const Image5D异常& 异常);
const StructArray 成功结构 = 异常转换(成功异常);