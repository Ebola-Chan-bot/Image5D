/* Copyright 2021 The MathWorks, Inc. */

#ifndef ORDERED_ITERATOR_HPP_
#define ORDERED_ITERATOR_HPP_

#include "ArrayIndex.hpp"

namespace matlab {
namespace data {
namespace detail {

/**
 * Helper class to used to manage ArrayIndex in a row major order
 */
class RowMajorOrder {
  public:
    static const MemoryLayout order = MemoryLayout::ROW_MAJOR;

    static void setEnd(const ArrayDimensions& dims,
                       const size_t nDims,
                       std::vector<ptrdiff_t>& index) {
        index[0] = dims[0];
    }
                       
    /**
     * Compare 2 indicies
     * @param lhs vector of indicies
     * @param rhs vector of indicies
     * @param nDims the number of dimensions
     * @param equal what to return if lhs == rhs
     * 
     * @return if lhs < rhs return true;
     *         if lhs > rhs return false;
     *         if lhs == rhs return equal flag
     */
    static bool lessThan(const std::vector<ptrdiff_t>& lhs,
                         const std::vector<ptrdiff_t>& rhs,
                         const size_t nDims,
                         bool equal) {
        for (size_t i = 0; i < nDims; ++i) {
            if (lhs[i] > rhs[i]) {
                return false;
            }
            else if (lhs[i] < rhs[i]) {
                return true;
            }
        }
        return equal;
    }
    
    /**
     * Compare 2 indicies
     * @param lhs vector of indicies
     * @param rhs vector of indicies
     * @param nDims the number of dimensions
     * @param equal what to return if lhs == rhs
     * 
     * @return if lhs > rhs return true;
     *         if lhs < rhs return false;
     *         if lhs == rhs return equal flag
     */
    static bool greaterThan(const std::vector<ptrdiff_t>& lhs,
                            const std::vector<ptrdiff_t>& rhs,
                            const size_t nDims,
                            bool equal) {
        for (size_t i = 0; i < nDims; ++i) {
            if (lhs[i] < rhs[i]) {
                return false;
            }
            else if (lhs[i] > rhs[i]) {
                return true;
            }
        }
        return equal;
    }

    /**
     * Calculate the in memory index based on the current array index
     * @param nDims number of indicies
     * @param index vector of indicies
     * @param dims dimensions of the array
     * @return the in memory offset
     */
    static size_t getInMemoryIndex(const size_t nDims,
                                   const std::vector<ptrdiff_t>& index,
                                   const ArrayDimensions& dims) {
        size_t idx = 0;
        size_t offset = 1;
        // loop through each of the current indicies, started with
        // the first since we are iterating in row major order
        // when the in memory layout is column major
        for (size_t i = 0; i < nDims; ++i) {
            // calculate the in memory index by adding up:
            // first index +
            // second index * first dimension +
            // third index * (first dimension * second dimension) +
            // fourth index * (first dimension * second dimension * third dimension)
            // etc.
            idx += (index[i] * offset);
            offset *= dims[i];
        }
        return idx;
    }

    /**
     * Move to the next index
     * @param nDims number of dimensions
     * @param dims array dimensions
     * @param index the array index
     * @returns nothing
     */
    static void next(const size_t nDims,
                     const ArrayDimensions& dims,
                     std::vector<ptrdiff_t>& index) {
        
        // since this is row major order, start by incrementing the last index and move towards the first index
        for (size_t i = nDims; i > 0; --i) {
            if (++index[i-1] < static_cast<ptrdiff_t>(dims[i-1])) {
                // bumped up the index without going out of range
                // so we are done
                return;
            }
            else {
                // index went out of range
                // so zero it out and move to the next one
                index[i-1] = 0;
            }
        }
        // got all the way through the indicies
        // iterator now points to the end
        // so set the index to one past the end
        setEnd(dims, nDims, index);
    }

    /**
     * Move to the previous index
     * @param nDims number of dimensions
     * @param dims array dimensions
     * @param index the array index
     * @returns nothing
     */
    static void prev(const size_t nDims,
                     const ArrayDimensions& dims,
                     std::vector<ptrdiff_t>& index) {
        // since this is row major order, start by decrementing the last index and move towards the first index
        for (size_t i = nDims; i > 0; --i) {
            if (index[i-1] == 0) {
                // if this index is at 0, reset it to its max value (which is one less that its dimension)
                index[i-1] = dims[i-1]-1;
            }
            else {
                // index is not at zero, just decrement and we are done
                --index[i-1];
                return;
            }
        }
        // got to the end, the index is going negative
        // clear out the index and set the first index to -1
        index = std::vector<ptrdiff_t>(nDims, 0);
        index[nDims-1] = -1;
    }
};

/**
 * Helper class to used to manage ArrayIndex in a column major order
 */
class ColumnMajorOrder {
  public:
    static const MemoryLayout order = MemoryLayout::COLUMN_MAJOR;

    static void setEnd(const ArrayDimensions& dims,
                       const size_t nDims,
                       std::vector<ptrdiff_t>& index) {
        index[nDims-1] = dims[nDims-1];
    }

    /**
     * Compare 2 indicies
     * @param lhs vector of indicies
     * @param rhs vector of indicies
     * @param nDims the number of dimensions
     * @param equal what to return if lhs == rhs
     * 
     * @return if lhs < rhs return true;
     *         if lhs > rhs return false;
     *         if lhs == rhs return equal flag
     */
    static bool lessThan(const std::vector<ptrdiff_t>& lhs,
                         const std::vector<ptrdiff_t>& rhs,
                         const size_t nDims,
                         bool equal) {
        for (size_t i = nDims; i > 0; --i) {
            if (lhs[i-1] > rhs[i-1]) {
                return false;
            }
            else if (lhs[i-1] < rhs[i-1]) {
                return true;
            }
        }
        return equal;
    }

    /**
     * Compare 2 indicies
     * @param lhs vector of indicies
     * @param rhs vector of indicies
     * @param nDims the number of dimensions
     * @param equal what to return if lhs == rhs
     * 
     * @return if lhs > rhs return true;
     *         if lhs < rhs return false;
     *         if lhs == rhs return equal flag
     */
    static bool greaterThan(const std::vector<ptrdiff_t>& lhs,
                            const std::vector<ptrdiff_t>& rhs,
                            const size_t nDims,
                            bool equal) {
        for (size_t i = nDims; i > 0; --i) {
            if (lhs[i-1] < rhs[i-1]) {
                return false;
            }
            else if (lhs[i-1] > rhs[i-1]) {
                return true;
            }
        }
        return equal;
    }

    /**
     * Calculate the in memory index based on the current array index
     * @param nDims number of indicies
     * @param index vector of indicies
     * @param dims dimensions of the array
     * @return the in memory offset
     */
    static size_t getInMemoryIndex(const size_t nDims,
                                   const std::vector<ptrdiff_t>& index,
                                   const ArrayDimensions& dims) {
        size_t idx = 0;
        size_t offset = 1;
        // loop through each of the current indicies, started with
        // the last since we are iterating in column major order
        // when the in memory layout is row major
        for (size_t i = nDims; i > 0; --i) {
            // calculate the in memory index by adding up:
            // last index +
            // last-1 index * last dimension +
            // last-2 index * (last dimension * last-1 dimension) +
            // last-3 index * (last dimension * last-1 dimension * last-2 dimension)
            // etc.
            idx += (index[i-1] * offset);
            offset *= dims[i-1];
        }
        return idx;
    }

    /**
     * Move to the next index
     * @param nDims number of dimensions
     * @param dims array dimensions
     * @param index the array index
     * @returns nothing
     */
    static void next(const size_t nDims,
                     const ArrayDimensions& dims,
                     std::vector<ptrdiff_t>& index) {
        // since this is column major order, start by incrementing the first index and move towards the last index
        for (size_t i = 0; i < nDims; ++i) {
            if (index[i] == static_cast<ptrdiff_t>(dims[i]-1)) {
                // index is at the max
                index[i] = 0;
            }
            else {
                ++index[i];
                return;
            }
        }
        // got all the way through the indicies
        // iterator now points to the end
        // so set the index to one past the end
        setEnd(dims, nDims, index);
    }

    /**
     * Move to the previous index
     * @param nDims number of dimensions
     * @param dims array dimensions
     * @param index the array index
     * @returns nothing
     */
    static void prev(const size_t nDims,
                     const ArrayDimensions& dims,
                     std::vector<ptrdiff_t>& index) {
        // since this is column major order, start by decrementing the first index and move towards the last index
        for (size_t i = 0; i < nDims; ++i) {
            if (index[i] == 0) {
                index[i] = dims[i]-1;
            }
            else {
                // index is not at zero, just decrement and we are done
                --index[i];
                return;
            }
        }
        // got to the end, the index is going negative
        // clear out the index and set the first index to -1
        index = std::vector<ptrdiff_t>(nDims, 0);
        index[0] = -1;
    }

};

template <typename T, typename ITORDER>
class OrderedIterator {
  public:

    typedef std::random_access_iterator_tag iterator_category;
    typedef typename matlab::data::TypedIterator<T>::value_type value_type;
    typedef std::ptrdiff_t difference_type;
    typedef typename matlab::data::TypedIterator<T>::reference reference;
    typedef typename matlab::data::TypedIterator<T>::pointer pointer;
    typedef typename matlab::data::TypedIterator<T>::ref_type ref_type;
    typedef typename matlab::data::TypedIterator<T>::impl_type impl_type;
    
    /**
     * OrderedIterator constructor 
     * @param rhs OrderedIterator object to assign to this instance.
     * @throw none
     */
    OrderedIterator(matlab::data::TypedIterator<T>&& it,
                    bool isEnd,
                    const matlab::data::ArrayDimensions& dims,
                    MemoryLayout arrayOrder)
        : it_(std::move(it))
        , matchingOrder_((arrayOrder == ITORDER::order) || (isVector(dims)))
        , currentArrayIndex_(dims, isEnd)
        , ref_(createTempRef<impl_type>())
    {}
        
    /**
     * OrderedIterator copy constructor 
     * @param rhs OrderedIterator object to assign to this instance.
     * @throw none
     */
    OrderedIterator(const OrderedIterator<T, ITORDER> &rhs) noexcept
        : it_(rhs.it_)
        , matchingOrder_(rhs.matchingOrder_)
        , currentArrayIndex_(rhs.currentArrayIndex_)
        , ref_(rhs.ref_)
    {}

    /**
     * OrderedIterator assignment operator
     * @param rhs OrderedIterator object to assign to this instance.
     * @throw none
     */
    OrderedIterator& operator=(const OrderedIterator<T, ITORDER> &rhs) noexcept {
        it_ = rhs.it_;
        matchingOrder_ = rhs.matchingOrder_;
        currentArrayIndex_ = rhs.currentArrayIndex_;
        ref_ = rhs.ref_;
        return *this;
    }

    /**
     * OrderedIterator move constructor 
     * @param rhs OrderedIterator object to assign to this instance.
     * @throw none
     */
    OrderedIterator(OrderedIterator<T, ITORDER>&& rhs) noexcept
        : it_(std::move(rhs.it_))
        , matchingOrder_(std::move(rhs.matchingOrder_))
        , currentArrayIndex_(std::move(rhs.currentArrayIndex_))
        , ref_(std::move(rhs.ref_))
    {}

    /**
     * Get a reference the element pointed to by this iterator
     *
     * @return reference - the element
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, reference>::type operator*() const noexcept {
        if (matchingOrder_) {
            return *ref_;
        }
        else {
            return *it_;
        }
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, reference>::type operator*() const noexcept {
        return *it_;
    }
        
    /**
     * pre-increment
     *
     * @return this
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>&>::type
    operator++() noexcept {
        if (matchingOrder_) {
            ++ref_;
        }
        else {
            it_ += currentArrayIndex_.advance(1);
        }
        return *this;
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>&>::type
    operator++() noexcept {
        if (matchingOrder_) {
            ++it_;
        }
        else {
            it_ += currentArrayIndex_.advance(1);
        }
        return *this;
    }

    /**
     * pre-decrement
     *
     * @return this
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>&>::type
    operator--() noexcept {
        if (matchingOrder_) {
            --ref_;
        }
        else {
            it_ += currentArrayIndex_.advance(-1);
        }
        return *this;
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>&>::type
    operator--() noexcept {
        if (matchingOrder_) {
            --it_;
        }
        else {
            it_ += currentArrayIndex_.advance(-1);
        }
        return *this;
    }
    
    /**
     * post-increment
     *
     * @return this
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>>::type
    operator++(int) noexcept {
        OrderedIterator<T, ITORDER> temp(*this);
        if (matchingOrder_) {
            ++ref_;
        }
        else {
            it_ += currentArrayIndex_.advance(1);
        }
        return temp;
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>>::type
    operator++(int) noexcept {
        OrderedIterator<T, ITORDER> temp(*this);
        if (matchingOrder_) {
            ++it_;
        }
        else {
            it_ += currentArrayIndex_.advance(1);
        }
        return temp;
    }
    
    /**
     * post-decrement
     *
     * @return this
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>>::type
    operator--(int) noexcept {
        OrderedIterator<T, ITORDER> temp(*this);
        if (matchingOrder_) {
            --ref_;
        }
        else {
            it_ += currentArrayIndex_.advance(-1);
        }
        return temp;
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>>::type
    operator--(int) noexcept {
        OrderedIterator<T, ITORDER> temp(*this);
        if (matchingOrder_) {
            --it_;
        }
        else {
            it_ += currentArrayIndex_.advance(-1);
        }
        return temp;
    }

    /**
     * Addition assignement
     *
     * @param d - amount to be added
     * @return this;
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>&>::type
    operator+=(difference_type d) noexcept {
        if (matchingOrder_) {
            ref_ += d;
        }
        else {
            it_ += currentArrayIndex_.advance(d);
        }
        return *this;
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>&>::type
    operator+=(difference_type d) noexcept {
        if (matchingOrder_) {
            it_ += d;
        }
        else {
            it_ += currentArrayIndex_.advance(d);
        }
        return *this;
    }

    /**
     * Subtraction assignement
     *
     * @param d - amount to be subtracted
     * @return this;
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>&>::type
    operator-=(difference_type d) noexcept {
        if (matchingOrder_) {
            ref_ -= d;
        }
        else {
            it_ += currentArrayIndex_.advance(-d);
        }
        return *this;
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>&>::type
    operator-=(difference_type d) noexcept {
        if (matchingOrder_) {
            it_ -= d;
        }
        else {
            it_ += currentArrayIndex_.advance(-d);
        }
        return *this;
    }

    /**
     * operator== 
     *
     * @param rhs - other iterator
     * @return true if both iterators are pointing to the same element
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, bool>::type
    operator==(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return ref_ == rhs.ref_;
        }
        else {
            return currentArrayIndex_ == rhs.currentArrayIndex_;
        }
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, bool>::type
    operator==(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return it_ == rhs.it_;
        }
        else {
            return currentArrayIndex_ == rhs.currentArrayIndex_;
        }
    }

    /**
     * operator!= 
     *
     * @param rhs - other iterator
     * @return true if both iterators are not pointing to the same element
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, bool>::type
    operator!=(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return ref_ != rhs.ref_;
        }
        else {
            return currentArrayIndex_ != rhs.currentArrayIndex_;
        }
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, bool>::type
    operator!=(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return it_ != rhs.it_;
        }
        else {
            return currentArrayIndex_ != rhs.currentArrayIndex_;
        }
    }

    /**
     * operator< 
     *
     * @param rhs - other iterator
     * @return true if lhs < rhs
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, bool>::type
    operator<(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return ref_ < rhs.ref_;
        }
        else {
            return currentArrayIndex_ < rhs.currentArrayIndex_;
        }
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, bool>::type
    operator<(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return it_ < rhs.it_;
        }
        else {
            return currentArrayIndex_ < rhs.currentArrayIndex_;
        }
    }

    /**
     * operator>
     *
     * @param rhs - other iterator
     * @return true if lhs > rhs
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, bool>::type
    operator>(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return ref_ > rhs.ref_;
        }
        else {
            return currentArrayIndex_ > rhs.currentArrayIndex_;
        }
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, bool>::type
    operator>(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return it_ > rhs.it_;
        }
        else {
            return currentArrayIndex_ > rhs.currentArrayIndex_;
        }
    }
    
    /**
     * operator<=
     *
     * @param rhs - other iterator
     * @return true if lhs <= rhs
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, bool>::type
    operator<=(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return ref_ <= rhs.ref_;
        }
        else {
            return currentArrayIndex_ <= rhs.currentArrayIndex_;
        }
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, bool>::type
    operator<=(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return it_ <= rhs.it_;
        }
        else {
            return currentArrayIndex_ <= rhs.currentArrayIndex_;
        }
    }

    /**
     * operator>=
     *
     * @param rhs - other iterator
     * @return true if lhs >= rhs
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, bool>::type
    operator>=(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return ref_ >= rhs.ref_;
        }
        else {
            return currentArrayIndex_ >= rhs.currentArrayIndex_;
        }
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, bool>::type
    operator>=(const OrderedIterator<T, ITORDER>& rhs) const noexcept {
        if (matchingOrder_) {
            return it_ >= rhs.it_;
        }
        else {
            return currentArrayIndex_ >= rhs.currentArrayIndex_;
        }
    }

    /**
     * operator+ - creates a new iterator that is added to this one by the amount passed in
     *
     * @param d - the amount to be added
     * @return newly created OrderedIterator
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>>::type
    operator+(difference_type d) noexcept {
        OrderedIterator<T, ITORDER> temp(*this);
        if (matchingOrder_) {
            temp.ref_ += d;
        }
        else {
            temp.it_ += temp.currentArrayIndex_.advance(d);
        }
        return temp;
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>>::type
    operator+(difference_type d) noexcept {
        OrderedIterator<T, ITORDER> temp(*this);
        if (matchingOrder_) {
            temp.it_ += d;
        }
        else {
            temp.it_ += temp.currentArrayIndex_.advance(d);
        }
        return temp;
    }

    /**
     * operator- - creates a new iterator that is decremented from this one by the amount passed in
     *
     * @param d - the amount to be decremented
     * @return newly created OrderedIterator
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>>::type
    operator-(difference_type d) const noexcept {
        OrderedIterator<T, ITORDER> temp(*this);
        if (matchingOrder_) {
            temp.ref_ -= d;
        }
        else {
            temp.it_ += temp.currentArrayIndex_.advance(-d);
        }
        return temp;
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, OrderedIterator<T, ITORDER>>::type
    operator-(difference_type d) const noexcept {
        OrderedIterator<T, ITORDER> temp(*this);
        if (matchingOrder_) {
            temp -= d;
        }
        else {
            temp.it_ += temp.currentArrayIndex_.advance(-d);
        }
        return temp;
    }

    /**
     * operator- - find the difference between 2 iterators
     *
     * @param rhs - other iterator
     * @return difference_type - the difference between the iterators
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, difference_type>::type
    operator-(const OrderedIterator<T, ITORDER> &rhs) const noexcept {
        if (matchingOrder_) {
            return ref_ - rhs.ref_;
        }
        else {
            return it_ - rhs.it_;
        }
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, difference_type>::type
    operator-(const OrderedIterator<T, ITORDER> &rhs) const noexcept {
        return it_ - rhs.it_;
    }

    /**
     * Get a reference using a linear index
     *
     * @return reference - the element
     * @throw none
     */
    reference operator[](const size_t& rhs) const noexcept {
        OrderedIterator<T, ITORDER> temp(*this);
        temp += rhs;
        return *temp;
    }

    /**
     * Get a pointer the element pointed to by this iterator
     *
     * @return pointer - the element
     * @throw none
     */
    template <typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, pointer>::type operator->() const noexcept {
        if (matchingOrder_) {
            return ref_;
        }
        else {
            return it_.operator->();
        }
    }
    template <typename U = T>
    typename std::enable_if<!std::is_arithmetic<U>::value, pointer>::type operator->() const noexcept {
        return it_.operator->();
    }
        
  private:
    typedef typename std::conditional<std::is_arithmetic<T>::value, T*, ref_type>::type TempRef;
    matlab::data::TypedIterator<T> it_;
    bool matchingOrder_;
    ArrayIndex<ITORDER> currentArrayIndex_;
    mutable TempRef ref_;

    template <typename U>
    typename std::enable_if<std::is_arithmetic<U>::value, U*>::type createTempRef() const noexcept {
        return const_cast<impl_type*>(it_.operator->());
    }            
    
    template <typename U>
    typename std::enable_if<!std::is_arithmetic<U>::value && !std::is_const<T>::value, Reference<U>>::type createTempRef() const noexcept {
        matlab::data::detail::ReferenceImpl* value = nullptr;
        return matlab::data::detail::Access::createObj<Reference<U>>(value);
    }
    
    template <typename U>
    typename std::enable_if<!std::is_arithmetic<U>::value && !std::is_same<Struct, U>::value && std::is_const<T>::value, ref_type>::type createTempRef() const noexcept {
        return ref_type();
    }
    
    template <typename U>
    typename std::enable_if<!std::is_arithmetic<U>::value && std::is_same<matlab::data::Struct, U>::value && std::is_const<T>::value, ref_type>::type createTempRef() const noexcept {
        return matlab::data::detail::Access::createObj<U>(std::shared_ptr<matlab::data::detail::ReferenceImpl>());
    }

    bool isVector(const matlab::data::ArrayDimensions& dims) {
        size_t nno = 0;
        for (const auto& d : dims) {
            if (d != 1) {
                ++nno;
                if (nno > 1) {
                    return false;
                }
            }
        }
        return true;
    }

};

}
}
}

#endif
