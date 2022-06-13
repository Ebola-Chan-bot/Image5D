/* Copyright 2021 The MathWorks, Inc. */

#ifndef COLUMN_MAJOR_ITERATOR_HPP_
#define COLUMN_MAJOR_ITERATOR_HPP_

#include "detail/OrderedIterator.hpp"
#include "detail/IteratorFactory.hpp"

namespace matlab {
namespace data {

template <typename T>
using ColumnMajorIterator = detail::OrderedIterator<T, detail::ColumnMajorOrder>;

using ColumnMajor = detail::IteratorFactory<ColumnMajorIterator>;

}
}

#endif
