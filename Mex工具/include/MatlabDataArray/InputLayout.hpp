/* Copyright 2021 The MathWorks, Inc. */

#ifndef _MATLAB_DATA_INPUT_LAYOUT_HPP_
#define _MATLAB_DATA_INPUT_LAYOUT_HPP_


namespace matlab {
namespace data {

enum class InputLayout : int { COLUMN_MAJOR = 0, ROW_MAJOR = 1, UNKNOWN = 2 };

}
} // namespace matlab

#endif
