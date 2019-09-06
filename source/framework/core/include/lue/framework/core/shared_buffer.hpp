#pragma once
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
    typename Element // ,
    // typename Allocator=std::allocator<Element>
>
class SharedBuffer
{

public:

    using Count = std::size_t;

private:

    static void do_not_delete(
        Element* /* ptr */)
    {
    }

    static void do_delete(
        Element* ptr)
    {
        delete[] ptr;
    }

    // template<
    //     typename Deallocator>
    // static void do_delete(
    //     Element* pointer,
    //     Deallocator deallocator,
    //     Count const size)
    // {
    //     deallocator.deallocate(pointer, size);
    // }

public:

    using Pointer = std::shared_ptr<Element>;

    // Some standard types
    using value_type = Element;
    using size_type = Count;
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
    explicit SharedBuffer():
        // Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{0} // ,
        // _allocator{allocator}

    {
        assert_invariants();
    }

    /*!
        @brief      Construct an instance with an array of a certain @a size
        @param      size Size of array to allocate

        The elements in the array are default-constructed.
    */
    explicit SharedBuffer(
        Count const size):
        // Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{size} // ,
        // _allocator{allocator}

    {
        allocate(size);

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
        Count const size,
        Mode const mode=Mode::copy):
        // Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{size} // ,
        // _allocator{allocator}

    {
        assert(
            (size == 0 && elements == nullptr) ||
            (size > 0 && elements != nullptr));

        switch(mode) {
            case(Mode::copy): {
                // Create new array and copy elements
                allocate(size);
                std::copy(elements, elements + size, begin());
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
        Count const size,
        Mode const mode=Mode::copy):
        // Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{size}  // ,
        // _allocator{allocator}

    {
        assert(
            (size == 0 && elements == nullptr) ||
            (size > 0 && elements != nullptr));

        using namespace std::placeholders;

        switch(mode) {
            case(Mode::copy): {
                // Create new array and copy elements
                allocate(size);
                std::copy(elements, elements + size, begin());
                break;
            }
            case(Mode::use): {
                // Use the elements passed in, and do not delete at the end
                _ptr = Pointer{elements, do_not_delete};
                break;
            }
            case(Mode::own): {
                // Use the elements passed in, and delete at the end
                // auto do_delete = std::bind(
                //     SharedBuffer::do_delete<Allocator>, _1, _allocator, _size);
                // _ptr = Pointer{elements};  // , do_delete};
                _ptr = Pointer{elements, do_delete};
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
        _size{other._size}

    {
        switch(mode) {
            case CopyMode::copy: {
                // Create new array and copy elements
                allocate(_size);
                std::copy(other.begin(), other.end(), begin());
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

    // SharedBuffer(
    //     SharedBuffer&& other):

    //     _ptr{std::move(other._ptr)},
    //     _size{other._size}  // ,
    //     // _allocator{std::move(other._allocator)}

    // {
    //     other._size = 0;

    //     assert_invariants();
    // }

    SharedBuffer& operator=(SharedBuffer const& other)=default;

    // SharedBuffer& operator=(
    //     SharedBuffer const& other)
    // {
    //     // _ptr.reset();
    //     // _allocator = other._allocator;
    //     resize(other._size);
    //     std::copy(other.begin(), other.end(), begin());

    //     assert_invariants();

    //     return *this;
    // }

    SharedBuffer& operator=(SharedBuffer&& other)=default;

    // SharedBuffer& operator=(
    //     SharedBuffer&& other)
    // {
    //     _ptr = std::move(other._ptr);
    //     _size = other._size;
    //     // _allocator = std::move(other._allocator);

    //     other._size = 0;

    //     assert_invariants();

    //     return *this;
    // }

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
    Count size() const
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
        assert(idx < _size);

        return _ptr.get()[idx];
    }

    /*!
        @brief      Access an element by index
    */
    Element const& operator[](
        std::size_t const idx) const
    {
        assert(idx < _size);

        // return _ptr[idx];
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
        from the new size.
    */
    void resize(
        std::size_t const size)
    {
        if(_size != size) {
            allocate(size);
            _size = size;
        }

        assert(_size == size);
    }

    // template<
    //     typename Archive>
    // void serialize(
    //     Archive& archive,
    //     unsigned int const /* version */)
    // {
    //     archive & hpx::serialization::make_array(begin(), size()) & _size;
    // }

private:

    Pointer        _ptr;

    Count          _size;

    // Allocator      _allocator;

    void allocate(
        std::size_t const size)
    {
        // using namespace std::placeholders;

        // auto do_delete = std::bind(
        //     SharedBuffer::do_delete<Allocator>, _1, _allocator, size);

        // _ptr.reset(_allocator.allocate(size));  // , do_delete);
        // _ptr = std::make_shared<Element>(size);

        // _ptr.reset(new Element[size]);

        // Default construct all elements
        _ptr.reset(new Element[size](), do_delete);
    }

    void assert_invariants()
    {
        assert(
            // Either the pointer is not set and the size is zero, or ...
            (!_ptr && _size == 0) ||
            // ... the pointer is set and the size is non-zero
            (_ptr && _size > 0));
    }

};

}  // namespace lue
