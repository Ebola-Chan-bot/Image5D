/* Copyright 2017 The MathWorks, Inc. */

#ifndef __MEX_IO_ADAPTER_HPP__
#define __MEX_IO_ADAPTER_HPP__

#include "MatlabDataArray.hpp"

#include <vector>
#include "assert.h"

namespace matlab {
    namespace mex {
        template <typename iterator_type>
        class MexIORange {

            iterator_type begin_;

            iterator_type end_;

            size_t size_;
          public:

            MexIORange(iterator_type b, iterator_type e, size_t size) : begin_(b), end_(e), size_(size) {}

            size_t size() {
                return size_;
            }

            typename std::iterator_traits<iterator_type>::difference_type internal_size() {
                return std::distance(begin_, end_);
            }

            iterator_type begin() {
                return begin_;
            }

            iterator_type end() {
                return end_;
            }

            bool empty() {
                return size() == 0;
            }

            typename std::iterator_traits<iterator_type>::reference operator[](size_t i) {
                if (static_cast<int>(i) + 1 > internal_size())
                    throw matlab::engine::Exception("ArgumentList index out of range.");

                return *(begin_ + i);
            }
        };
    }
}





void implToArray(int na, void* va[], std::vector<matlab::data::Array>& pa);

void arrayToImpl(int na, void* va[], const std::vector<matlab::data::Array>& pa);

void arrayToImplOutput(int nlhs, std::vector<matlab::data::Array>& edi_plhs, void (*callbackOutput)(int, void**));
#endif