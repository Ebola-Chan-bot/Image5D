/* Copyright 2021 The MathWorks, Inc. */

#ifndef RMS_ARRAY_INDEX_HPP_
#define RMS_ARRAY_INDEX_HPP_

namespace matlab {
namespace data {
namespace detail {

/**
 * This class keeps track of the current index into a TypedArray. It is a helper class for iterators
 * that iterate in an order that is different than memory order.
 * @template IT_ORDER - specifies which order to move index
 */
template <typename IT_ORDER>
class ArrayIndex {
  public:
    /**
     * ArrayIndex constructor
     * @param dims the dimensions of the array that is being iterated over
     * @param isEnd indicates if this iterator indicates the end of the array
     * @returns instance of ArrayIndex
     * @throw none
     */
    ArrayIndex(const matlab::data::ArrayDimensions& dims,
               bool isEnd) noexcept
        : nDims_(dims.size())
        , index_(nDims_, 0)
        , dims_(dims)
        , currentInMemoryLinearIndex_(isEnd ? matlab::data::getNumElements(dims) : 0)
    {
        if (isEnd) {
            if (getNumElements(dims) > 0) {
                IT_ORDER::setEnd(dims, nDims_, index_);
            }
        }
    }

    /**
     * ArrayIndex move constructor
     * @param rhs the original ArrayIndex
     * @returns instance of ArrayIndex
     * @throw none
     */
    ArrayIndex(ArrayIndex&& rhs) noexcept
        : nDims_(std::move(rhs.nDims_))
        , index_(std::move(rhs.index_))
        , dims_(std::move(rhs.dims_))
        , currentInMemoryLinearIndex_(std::move(rhs.currentInMemoryLinearIndex_))
    {}
    
    /**
     * ArrayIndex copy constructor
     * @param rhs the original ArrayIndex
     * @returns instance of ArrayIndex
     * @throw none
     */
    ArrayIndex(const ArrayIndex& rhs) noexcept
        : nDims_(rhs.nDims_)
        , index_(rhs.index_)
        , dims_(rhs.dims_)
        , currentInMemoryLinearIndex_(rhs.currentInMemoryLinearIndex_)
    {}

    /**
     * ArrayIndex assignment operator
     * @param rhs the original ArrayIndex
     * @returns instance of ArrayIndex
     * @throw none
     */
    ArrayIndex& operator=(const ArrayIndex& rhs) noexcept {
        nDims_ = rhs.nDims_;
        index_ = rhs.index_;
        dims_ = rhs.dims_;
        currentInMemoryLinearIndex_ = rhs.currentInMemoryLinearIndex_;
        return *this;
    }

    /**
     * ArrayIndex move assignment operator
     * @param rhs the original ArrayIndex
     * @returns instance of ArrayIndex
     * @throw none
     */
    ArrayIndex& operator=(ArrayIndex&& rhs) noexcept {
        nDims_ = std::move(rhs.nDims_);
        index_ = std::move(rhs.index_);
        dims_ = std::move(rhs.dims_);
        currentInMemoryLinearIndex_ = std::move(rhs.currentInMemoryLinearIndex_);
        return *this;
    }

    /**
     * Compares two ArrayIndicies for equality 
     * @param rhs the other ArrayIndex
     * @returns true if both point to the same element, otherwise false
     * @throw none
     */
    bool operator==(const ArrayIndex<IT_ORDER>& rhs) const noexcept {
        for (size_t i = 0; i < nDims_; ++i) {
            if (index_[i] != rhs.index_[i]) {
                return false;
            }
        }
        return true;
    }

    /**
     * Checks to see if this ArrayIndex is less than the input
     * @param rhs the other ArrayIndex
     * @returns true if this one is less than the input, otherwise false
     * @throw none
     */
    bool operator<(const ArrayIndex<IT_ORDER>& rhs) const noexcept {
        return IT_ORDER::lessThan(index_, rhs.index_, nDims_, false);
    }

    /**
     * Checks to see if this ArrayIndex is greater than the input
     * @param rhs the other ArrayIndex
     * @returns true if this one is greater than the input, otherwise false
     * @throw none
     */
    bool operator>(const ArrayIndex<IT_ORDER>& rhs) const noexcept {
        return IT_ORDER::greaterThan(index_, rhs.index_, nDims_, false);
    }

    /**
     * Checks to see if this ArrayIndex is less than or equal to the input
     * @param rhs the other ArrayIndex
     * @returns true if this one is less than or equal to the input, otherwise false
     * @throw none
     */
    bool operator<=(const ArrayIndex<IT_ORDER>& rhs) const noexcept {
        return IT_ORDER::lessThan(index_, rhs.index_, nDims_, true);
    }

    /**
     * Checks to see if this ArrayIndex is greater than or equal to the input
     * @param rhs the other ArrayIndex
     * @returns true if this one is less than or greater to the input, otherwise false
     * @throw none
     */
    bool operator>=(const ArrayIndex<IT_ORDER>& rhs) const noexcept {
        return IT_ORDER::greaterThan(index_, rhs.index_, nDims_, true);
    }

    /**
     * Moves the ArrayIndex by the amount specificed by 'd' and returns the actual 
     * amount that the index was moved by based on the memory layout of the array
     * @param d the amount to move the ArrayIndex
     * @returns the distance in memory that the index was moved
     * @throw none
     */
    std::ptrdiff_t advance(std::ptrdiff_t d) noexcept {
        const std::ptrdiff_t prevIndex = currentInMemoryLinearIndex_;
        if (d > 0) {
            for (std::ptrdiff_t i = 0; i < d; ++i) {
                IT_ORDER::next(nDims_, dims_, index_);
            }
        }
        else if (d < 0) {
            for (std::ptrdiff_t i = d; i < 0; ++i) {
                IT_ORDER::prev(nDims_, dims_, index_);
            }
        }
        currentInMemoryLinearIndex_ = IT_ORDER::getInMemoryIndex(nDims_, index_, dims_);
        return (currentInMemoryLinearIndex_ - prevIndex);
    }

  private:
    size_t nDims_;
    std::vector<ptrdiff_t> index_;
    matlab::data::ArrayDimensions dims_;
    ptrdiff_t currentInMemoryLinearIndex_;
};

}
}
}

#endif
