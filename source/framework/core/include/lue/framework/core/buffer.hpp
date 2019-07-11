#pragma once
#include <cassert>
#include <functional>
#include <memory>


namespace lue {

template<
    typename Element // ,
    // typename Allocator=std::allocator<Element>
>
class Buffer
{

public:

    using Count = std::size_t;

private:

    static void do_not_delete(
        Element*)
    {
    }

    template<
        typename Deallocator>
    static void do_delete(
        Element* pointer,
        Deallocator deallocator,
        Count const size)
    {
        deallocator.deallocate(pointer, size);
    }

public:

    using Pointer = std::shared_ptr<Element[]>;

    // Some standard types
    using value_type = Element;
    using size_type = Count;
    using iterator = Element*;
    using const_iterator = Element const*;

    enum class Mode
    {
        //! Copy the elements
        copy = 0,

        //! Use the elements
        use = 1,

        //! Own the elements
        own = 2
    };

    explicit Buffer():
        // Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{0} // ,
        // _allocator{allocator}

    {
        assert_invariants();
    }

    explicit Buffer(
        Count const size):
        // Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{size} // ,
        // _allocator{allocator}

    {
        allocate(size);

        assert_invariants();
    }

    Buffer(
        Element const* elements,
        Count const size,
        Mode const mode=Mode::copy):
        // Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{size} // ,
        // _allocator{allocator}

    {
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

    Buffer(
        Element* elements,
        Count const size,
        Mode const mode=Mode::copy):
        // Allocator const& allocator=Allocator{}):

        _ptr{},
        _size{size}  // ,
        // _allocator{allocator}

    {
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
                //     Buffer::do_delete<Allocator>, _1, _allocator, _size);
                _ptr = Pointer{elements};  // , do_delete};
                break;
            }
        }

        assert_invariants();
    }

    Buffer(
        Buffer const& other):

        Buffer{other.data(), other._size, Mode::copy}  // , other._allocator}

    {
        assert_invariants();
    }

    Buffer(
        Buffer&& other):

        _ptr{std::move(other._ptr)},
        _size{other._size}  // ,
        // _allocator{std::move(other._allocator)}

    {
        other._size = 0;

        assert_invariants();
    }

    Buffer& operator=(
        Buffer const& other)
    {
        // _ptr.reset();
        // _allocator = other._allocator;
        resize(other._size);
        std::copy(other.begin(), other.end(), begin());

        assert_invariants();

        return *this;
    }

    Buffer& operator=(
        Buffer&& other)
    {
        _ptr = std::move(other._ptr);
        _size = other._size;
        // _allocator = std::move(other._allocator);

        other._size = 0;

        assert_invariants();

        return *this;
    }

    ~Buffer()=default;

    Element* data()
    {
        return _ptr.get();
    }

    Element const* data() const
    {
        return _ptr.get();
    }

    Count size() const
    {
        return _size;
    }

    iterator begin()
    {
        return data();
    }

    iterator end()
    {
        return data() + _size;
    }

    const_iterator begin() const
    {
        return data();
    }

    const_iterator end() const
    {
        return data() + _size;
    }

    Element& operator[](
        std::size_t const idx)
    {
        assert(idx < _size);

        return _ptr[idx];
    }

    Element const& operator[](
        std::size_t const idx) const
    {
        assert(idx < _size);

        return _ptr[idx];
    }

    bool operator==(
        Buffer const& other) const
    {
        return
            _size == other._size &&
            std::equal(begin(), end(), other.begin());
    }

    void resize(
        std::size_t const size)
    {
        if(_size != size) {
            allocate(size);
            _size = size;
        }

        assert(_size == size);
    }

private:

    Pointer        _ptr;

    Count          _size;

    // Allocator      _allocator;

    void allocate(
        std::size_t const size)
    {
        // using namespace std::placeholders;

        // auto do_delete = std::bind(
        //     Buffer::do_delete<Allocator>, _1, _allocator, size);

        // _ptr.reset(_allocator.allocate(size));  // , do_delete);
        // _ptr = std::make_shared<Element>(size);

        _ptr.reset(new Element[size]);
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
