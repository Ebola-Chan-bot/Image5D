/* Copyright 2021 The MathWorks, Inc. */

#ifndef ROW_MAJOR_ITERATOR_HPP_
#define ROW_MAJOR_ITERATOR_HPP_

#include "detail/OrderedIterator.hpp"
#include "detail/IteratorFactory.hpp"

namespace matlab {
namespace data {

template <typename T>
using RowMajorIterator = detail::OrderedIterator<T, detail::RowMajorOrder>;

using RowMajor = detail::IteratorFactory<RowMajorIterator>;

}
}

#endif
