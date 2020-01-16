#pragma once
#include "lue/framework/core/define.hpp"
#include <cassert>
#include <functional>
#include <memory>


// - Get rid of shared pointer? Simplifies destruction.
//     - No, we need to be able to copy buffers. The pointer knows when
//         to delete.
// - Make it possible to use a PartitionAllocator


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

    enum class Mode
    {
        /*!
            @brief      Copy the elements from the array

            Upon construction, we must allocate our own array and copy
            the elemtents from the array passed in.
        */
        copy = 0,

        /*!
            @brief      Use the elements in the array

            We're not allowed to delete the array.
        */
        use = 1,

        /*!
            @brief      Own the elements in the array

            After we're done using the array, we are responsible for
            deleting it.
        */
        own = 2
    };

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

    /*!
        @brief      Construct an instance based on an existing array
        @param      elements Pointer to existing array
        @param      count Size of array
        @param      mode Whether or not we should copy or use the array
                    passed in
        @exception  std::runtime_error If the @a mode passed in is
                    Mode::own. You cannot take ownership of the read-only
                    array passed in.
    */
    SharedBuffer(
        Element const* elements,
        Size const size,
        Mode const mode=Mode::copy,
        Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{size},
        _allocator{allocator}

    {
        assert(
            (size == 0 && elements == nullptr) ||
            (size > 0 && elements != nullptr));

        switch(mode) {
            case(Mode::copy): {
                // Create new array and copy elements
                allocate(size);
                std::uninitialized_copy(elements, elements + size, begin());
                break;
            }
            case(Mode::use): {
                // Use the elements passed in, and do not delete at the end
                _ptr = Pointer{const_cast<Element*>(elements), do_not_delete};
                break;
            }
            case(Mode::own): {
                throw std::runtime_error(
                    "Cannot take ownership of read-only buffer");
                break;
            }
        }

        assert_invariants();
    }

    /*!
        @brief      Construct an instance based on an existing array
        @param      elements Pointer to existing array
        @param      count Size of array
        @param      mode Whether or not we should copy, use, or own
                    the array passed in
    */
    SharedBuffer(
        Element* elements,
        Size const size,
        Mode const mode=Mode::copy,
        Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{size},
        _allocator{allocator}

    {
        assert(
            (size == 0 && elements == nullptr) ||
            (size > 0 && elements != nullptr));

        using namespace std::placeholders;

        switch(mode) {
            case(Mode::copy): {
                // Create new array and copy elements
                allocate(size);
                std::uninitialized_copy(elements, elements + size, begin());
                break;
            }
            case(Mode::use): {
                // Use the elements passed in, and do not delete at the end
                _ptr = Pointer{elements, do_not_delete};
                break;
            }
            case(Mode::own): {
                // Use the elements passed in, and delete at the end
                auto deleter =
                    [allocator=this->_allocator, size](Element* ptr)
                    {
                        do_delete(ptr, std::move(allocator), size);
                    };
                _ptr = Pointer{elements, deleter};
                break;
            }
        }

        assert_invariants();
    }

    // SharedBuffer(SharedBuffer const& other)=default;

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
        CopyMode const mode=CopyMode::share):

        _ptr{},
        _size{other._size},
        _allocator{other._allocator}

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
            [allocator=this->_allocator, size](Element* ptr)
            {
                do_delete(ptr, std::move(allocator), size);
            };

        _ptr.reset(size > 0 ? _allocator.allocate(size) : nullptr, deleter);
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
