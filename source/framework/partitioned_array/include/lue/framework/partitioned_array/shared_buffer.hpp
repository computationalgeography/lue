#pragma once
#include "lue/framework/core/array.hpp"


namespace lue {

    /*!
        @brief      Class for managing a contiguous buffer of elements which can be shared between instances

        Instances contain a shared pointer to a, possibly empty, array of elements.
    */
    template<typename Element>
    class SharedBuffer
    {

        public:

            //! The type of the collection of elements
            using Elements = Array<Element, 1>;  // A 1D array containing the actual buffer

            //! The type of the reference counted pointer to the collection of elements
            using Pointer = std::shared_ptr<Elements>;

            using Iterator = Elements::Iterator;

            using ConstIterator = Elements::ConstIterator;


            SharedBuffer():

                _ptr{}

            {
                allocate(0);

                assert_invariants();
            }


            /*!
                @brief      Construct an instance pointing to a buffer with room
                            for @a size elements
            */
            explicit SharedBuffer(Size const size):

                _ptr{}

            {
                allocate(size);

                assert_invariants();
            }


            /*!
                @brief      Copy-construct an instance

                The layered collection of elements will be shared.
            */
            SharedBuffer(SharedBuffer const& other) = default;


            /*!
                @brief      Move-construct an instance
            */
            SharedBuffer(SharedBuffer&& other) noexcept:

                SharedBuffer{}

            {
                // This class requires that a _ptr always points to a, possibly
                // empty, container

                std::swap(other._ptr, _ptr);
                assert_invariants();

                other.assert_invariants();
                lue_hpx_assert(other.size() == 0);
            }


            ~SharedBuffer() = default;


            /*!
                @brief      Copy-assign @a other to this instance

                The layered collection of elements will be shared.
            */
            auto operator=(SharedBuffer const& other) -> SharedBuffer& = default;


            /*!
                @brief      Move-assign @a other to this instance
            */
            auto operator=(SharedBuffer&& other) noexcept -> SharedBuffer&
            {
                // This class requires that a _ptr always points to a, possibly empty, collection of elements

                std::swap(other._ptr, _ptr);
                assert_invariants();

                other._ptr->clear();
                other.assert_invariants();
                lue_hpx_assert(other.size() == 0);

                return *this;
            }


            /*!
                @brief      Return a pointer to the underlying array
            */
            auto data() -> Element*
            {
                assert_invariants();

                return (*_ptr).data();
            }


            /*!
                @overload
            */
            auto data() const -> Element const*
            {
                assert_invariants();

                return (*_ptr).data();
            }


            /*!
                @brief      Return the number of elements in the buffer
            */
            auto size() const -> Size
            {
                assert_invariants();

                return (*_ptr).nr_elements();
            }


            /*!
                @brief      Return whether the buffer is empty
            */
            auto empty() const -> bool
            {
                assert_invariants();

                return (*_ptr).empty();
            }


            /*!
                @brief      Return an iterator to the beginning of the buffer
            */
            auto begin() -> Iterator
            {
                assert_invariants();

                return (*_ptr).begin();
            }


            /*!
                @brief      Return an iterator to the end of the buffer
            */
            auto end() -> Iterator
            {
                assert_invariants();

                return (*_ptr).end();
            }


            /*!
                @overload
            */
            auto begin() const -> ConstIterator
            {
                assert_invariants();

                return (*_ptr).begin();
            }


            /*!
                @overload
            */
            auto end() const -> ConstIterator
            {
                assert_invariants();

                return (*_ptr).end();
            }


            /*!
                @brief      Access an element by index
            */
            auto operator[](Index const idx) -> Element&
            {
                assert_invariants();
                lue_hpx_assert(idx < static_cast<Index>((*_ptr).nr_elements()));

                return (*_ptr)[idx];
            }


            /*!
                @overload
            */
            auto operator[](Index const idx) const -> Element const&
            {
                assert_invariants();
                lue_hpx_assert(idx < static_cast<Index>((*_ptr).nr_elements()));

                return (*_ptr)[idx];
            }


            /*!
                @brief      Resize the buffer
            */
            void resize(Size const size)
            {
                (*_ptr).reshape(typename Elements::Shape{size});

                // (*_ptr).resize(size, boost::container::default_init_t{});

                assert_invariants();
            }


            // void clear()
            // {
            //     assert_invariants();
            //
            //     (*_ptr).clear();
            //
            //     assert_invariants();
            // }


            auto use_count() const -> long
            {
                return _ptr.use_count();
            }


            /*!
                @brief      Return whether or not this instance equals @a other

                Returns true when both the buffer sizes are equal and the elements themselves are equal.
            */
            auto operator==(SharedBuffer const& other) const -> bool
            {
                assert_invariants();
                other.assert_invariants();

                return this->size() == other.size() && std::equal(this->begin(), this->end(), other.begin());
            }


            /*!
                @brief      Remove elements from the array

                Trailing elements will be moved to the front, to take in the place of the removed elements.
                The memory buffer will not be resized.
            */
            void remove(Iterator begin, Iterator end)
            {
                auto& container{*_ptr};

                lue_hpx_assert(begin >= container.begin());
                lue_hpx_assert(end <= container.end());

                container.remove(begin, end);

                assert_invariants();
            }


        private:

            void allocate(Size const size)
            {
                lue_hpx_assert(!_ptr);

                // Default initialization of the elements. In case of
                // numeric values, the values are indeterminate
                // _ptr = std::make_shared<Elements>(size, boost::container::default_init_t{});
                _ptr = std::make_shared<Elements>(typename Elements::Shape{size});

                assert_invariants();
            }


            void assert_invariants() const
            {
                // _ptr should always be set, but the size of the vector can
                // be zero
                lue_hpx_assert(_ptr);
            }


            //! Pointer to contiguous array of Element instances
            Pointer _ptr;
    };

}  // namespace lue
