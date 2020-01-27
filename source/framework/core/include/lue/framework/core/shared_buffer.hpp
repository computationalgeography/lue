#pragma once
#include "lue/framework/core/define.hpp"
#include <cassert>
#include <functional>
#include <memory>


namespace lue {

enum class CopyMode
{

    //! Share buffer with new instance
    share,

    //! Copy buffer to new instance
    copy

};


/*!
    @brief      Class template for shared buffers
    @tparam     Element Type of elements in the underlying array

    A buffer is a contiguous array of elements of the same type. This
    class allows instances to share this array, to prevent copying and
    the associated allocation of memory. What sets this class apart from
    the common std::shared_ptr is that upon construction, a pointer to
    an existing array can be passed in, of which the ownership can be
    explicitly set (see SharedBuffer::Mode). Also, instances can be either
    shallowly copy constructed (sharing the array, default), or deeply
    copy constructed, in which case the layered array itself is also
    copied. Move construction, copy assignment and move assignment behave
    the same as in the case of a std::shared_ptr.

    After construction, the instance behaves as a shared pointer to the
    underlying array. This does not depend on whether or not the instance
    owns the array or not.
*/
template<
    typename Element,
    typename Allocator=std::allocator<Element>
>
class SharedBuffer
{

public:

    using Size = lue::Size;

private:

    static void do_not_delete(
        Element* /* ptr */)
    {
    }

    static void do_delete(
        Element* ptr,
        Allocator allocator,
        Size const size)
    {
        std::destroy_n(ptr, size);  // Destruct each instance
        allocator.deallocate(ptr, size);  // Release memory
    }

public:

    // Pointer type which allows a custom destructor to be set
    using Pointer = std::shared_ptr<Element>;

    // Some standard types
    using value_type = Element;
    using size_type = Size;
    using iterator = Element*;
    using const_iterator = Element const*;

    /*!
        @brief      Default-construct an instance

        The layered array will be empty and the size zero.
    */
    explicit SharedBuffer(
        Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{0},
        _allocator{allocator}

    {
        assert_invariants();
    }

    /*!
        @brief      Construct an instance with an array of a certain @a size
        @param      size Size of array to allocate

        The elements in the array are default-constructed.
    */
    explicit SharedBuffer(
        Size const size,
        Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{size},
        _allocator{allocator}

    {
        allocate(size);
        std::uninitialized_default_construct_n(_ptr.get(), size);

        assert_invariants();
    }

    // SharedBuffer(SharedBuffer const& other)=delete;

    /*!
        @brief      Copy-construct an instance
        @param      other Instance to copy from
        @param      mode Whether or not we should shallowly copy
                    (CopyMode::share) or deeply copy (CopyMode::copy)
                    the instance passed in

        By default, copies share the underlying array (we're a shared
        buffer after all). As an extension, callers can request a deeply
        copied instance.
    */
    SharedBuffer(
        SharedBuffer const& other,
        CopyMode const mode=CopyMode::share,
        Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{other._size},
        _allocator{allocator}

    {
        switch(mode) {
            case CopyMode::copy: {

                // Create new array and copy elements
                allocate(_size);
                std::uninitialized_copy(other.begin(), other.end(), begin());
                break;

            }
            case CopyMode::share: {

                // Share array between instances
                _ptr = other._ptr;
                break;

            }
        }

        assert_invariants();
    }

    SharedBuffer(SharedBuffer&& other)=default;

    SharedBuffer& operator=(SharedBuffer const& other)=default;

    SharedBuffer& operator=(SharedBuffer&& other)=default;

    ~SharedBuffer()=default;

    /*!
        @brief      Return a pointer to the underlying array
    */
    Element* data()
    {
        return _ptr.get();
    }

    /*!
        @brief      Return a pointer to the underlying array
    */
    Element const* data() const
    {
        return _ptr.get();
    }

    /*!
        @brief      Return the number of elements in the buffer
    */
    Size size() const
    {
        return _size;
    }

    /*!
        @brief      Return an iterator to the beginning of the buffer
    */
    iterator begin()
    {
        return data();
    }

    /*!
        @brief      Return an iterator to the end of the buffer
    */
    iterator end()
    {
        return data() + _size;
    }

    /*!
        @brief      Return an iterator to the beginning of the buffer
    */
    const_iterator begin() const
    {
        return data();
    }

    /*!
        @brief      Return an iterator to the end of the buffer
    */
    const_iterator end() const
    {
        return data() + _size;
    }

    /*!
        @brief      Access an element by index
    */
    Element& operator[](
        std::size_t const idx)
    {
        assert(idx < static_cast<std::size_t>(_size));

        return _ptr.get()[idx];
    }

    /*!
        @brief      Access an element by index
    */
    Element const& operator[](
        std::size_t const idx) const
    {
        assert(idx < static_cast<std::size_t>(_size));

        return _ptr.get()[idx];
    }

    /*!
        @brief      Return whether or not this instance equals @a other

        Returns true when both the buffer sizes are equal and the
        elements themselves are equal.
    */
    bool operator==(
        SharedBuffer const& other) const
    {
        return
            _size == other._size &&
            std::equal(begin(), end(), other.begin());
    }

    /*!
        @brief      Resize the buffer
        @param      size New size of the buffer

        The buffer will only be resized if the new size is different
        from the current size.
    */
    void resize(
        Size const size)
    {
        if(_size != size) {
            allocate(size);
            std::uninitialized_default_construct_n(_ptr.get(), size);
            _size = size;
        }

        assert(_size == size);
    }

private:

    //! Pointer to contiguous array of Element instances
    Pointer        _ptr;

    //! Number of Element instances in the buffer
    Size           _size;

    //! Allocator to use for managing memory of the buffer
    Allocator      _allocator;

    void allocate(
        Size const size)
    {
        // Allocate memory and install a deleter

        // FIXME Deleter assumes instances are created, which is not
        //   done here but at the call sites
        auto deleter =
            [allocator=this->_allocator, size](
                Element* ptr)
            {
                do_delete(ptr, std::move(allocator), size);
            };

        _ptr.reset(size > 0 ? _allocator.allocate(size) : nullptr, deleter);

        assert(
            // Either the pointer is not set and the size is zero, or ...
            (!_ptr && size == 0) ||
            // ... the pointer is set and the size is larger than zero
            (_ptr && size > 0));
    }

    void assert_invariants()
    {
        assert(
            // Either the pointer is not set and the size is zero, or ...
            (!_ptr && _size == 0) ||
            // ... the pointer is set and the size is larger than zero
            (_ptr && _size > 0));
    }

};

}  // namespace lue
