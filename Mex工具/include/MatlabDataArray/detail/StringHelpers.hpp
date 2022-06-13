/* Copyright 2015-2017 The MathWorks, Inc. */

#ifndef STRING_HELPERS_HPP_
#define STRING_HELPERS_HPP_

#include <stdint.h>
#include <string>
#include <algorithm>
#include <iterator>

namespace matlab {
    namespace data {
        namespace detail {
            inline bool isAscii7(const String &str) {
                return (std::find_if(str.begin(), str.end(), [](CHAR16_T ch) {return (ch & 0xFF80) != 0; }) == str.end());
            }

            inline bool isAscii7(const std::string &str) {
                return (std::find_if(str.begin(), str.end(), [](char ch) {return (ch & 0x80) != 0; }) == str.end());
            }

            inline std::string toAsciiHelper(const char16_t* in, size_t inLen) {
                if (in == nullptr) {
                    return std::string();
                }
                std::string out;
                std::transform(in, in + inLen, std::back_inserter(out),
                               [](char16_t ch) -> char { return static_cast<char>(ch); });
                return out;
            }
        }
    }
}


#endif
