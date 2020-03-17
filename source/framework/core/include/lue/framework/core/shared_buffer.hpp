#pragma once
#include "lue/framework/core/define.hpp"
#include <memory>
#include <boost/container/vector.hpp>


namespace lue {

template<
    typename Element
>
class SharedBuffer
{

public:

    using Elements = boost::container::vector<Element>;

    using Pointer = std::shared_ptr<Elements>;

    using Iterator = typename Elements::iterator;

    using ConstIterator = typename Elements::const_iterator;

    SharedBuffer():

        _ptr{}

    {
        allocate(0);
    }

    explicit SharedBuffer(
        Size const size):

        _ptr{}

    {
        allocate(size);
    }

    SharedBuffer(SharedBuffer const&)=default;

    SharedBuffer(
        SharedBuffer&& other):

        _ptr{std::move(other._ptr)}

    {
        // This class requires that _ptr always points to a, possibly
        // empty, container
        assert(!other._ptr);
        other.allocate(0);
    }

    ~SharedBuffer()=default;

    SharedBuffer& operator=(SharedBuffer const&)=default;

    SharedBuffer& operator=(
        SharedBuffer&& other)
    {
        _ptr = std::move(other._ptr);

        // This class requires that a _ptr always points to a, possibly
        // empty, container
        assert(!other._ptr);
        other.allocate(0);

        return *this;
    }

    /*!
        @brief      Return a pointer to the underlying array
    */
    Element* data()
    {
        assert_invariants();
        return (*_ptr).data();
    }

    /*!
        @brief      Return a pointer to the underlying array
    */
    Element const* data() const
    {
        assert_invariants();
        return (*_ptr).data();
    }

    /*!
        @brief      Return the number of elements in the buffer
    */
    Size size() const
    {
        assert_invariants();
        return (*_ptr).size();
    }

    /*!
        @brief      Return an iterator to the begining of the buffer
    */
    Iterator begin()
    {
        assert_invariants();
        return (*_ptr).begin();
    }

    /*!
        @brief      Return an iterator to the end of the buffer
    */
    Iterator end()
    {
        assert_invariants();
        return (*_ptr).end();
    }

    /*!
        @brief      Return an iterator to the begining of the buffer
    */
    ConstIterator begin() const
    {
        assert_invariants();
        return (*_ptr).begin();
    }

    /*!
        @brief      Return an iterator to the end of the buffer
    */
    ConstIterator end() const
    {
        assert_invariants();
        return (*_ptr).end();
    }

    /*!
        @brief      Access an element by index
    */
    Element& operator[](
        Index const idx)
    {
        assert_invariants();
        assert(idx < static_cast<Index>((*_ptr).size()));

        return (*_ptr)[idx];
    }

    /*!
        @brief      Access an element by index
    */
    Element const& operator[](
        Index const idx) const
    {
        assert_invariants();
        assert(idx < static_cast<Index>((*_ptr).size()));

        return (*_ptr)[idx];
    }

    void resize(
        Size const size)
    {
        assert_invariants();
        (*_ptr).resize(size, boost::container::default_init_t{});
    }

    /*!
        @brief      Return whether or not this instance equals @a other

        Returns true when both the buffer sizes are equal and the
        elements themselves are equal.
    */
    bool operator==(
        SharedBuffer const& other) const
    {
        assert_invariants();
        other.assert_invariants();

        return
            this->size() == other.size() &&
            std::equal(this->begin(), this->end(), other.begin());
    }

    /*!
        @brief      Remove elements from the array
        @param      .
        @return     .
        @exception  .

        Trailing elements will be moved to the front, to take in the
        place of the removed elements. The memory buffer will not
        be resized.
    */
    void erase(
            Iterator const begin,
            Iterator const end)
    {
        assert_invariants();

        auto& container{*_ptr};

        container.erase(begin, end);
    }

private:

    //! Pointer to contiguous array of Element instances
    Pointer        _ptr;

    void allocate(
        Size const size)
    {
        assert(!_ptr);

        _ptr.reset(new Elements(size, boost::container::default_init_t{}));

        assert_invariants();
    }

    void assert_invariants() const
    {
        // _ptr should always be set, but the size of the vector can
        // be zero
        assert(_ptr);
    }

};

}  // namespace lue
