#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/define.hpp"
#include <boost/container/vector.hpp>
#include <memory>


namespace lue {

    /*!
        @brief      Class for managing a contiguous buffer of elements which
                    can be shared between instances

        Instances contain a shared pointer to a, possibly empty, array
        of elements.
    */
    template<typename Element>
    class SharedBuffer
    {

        public:

            using Elements = boost::container::vector<Element>;

            using Pointer = std::shared_ptr<Elements>;

            using Iterator = typename Elements::iterator;

            using ConstIterator = typename Elements::const_iterator;

            using Size = lue::Size;


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


            SharedBuffer(SharedBuffer const&) = default;


            SharedBuffer(SharedBuffer&& other):

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

            SharedBuffer& operator=(SharedBuffer const&) = default;


            SharedBuffer& operator=(SharedBuffer&& other)
            {
                // This class requires that a _ptr always points to a, possibly
                // empty, container

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
            Element* data()
            {
                assert_invariants();

                return (*_ptr).data();
            }


            /*!
                @overload
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
                @brief      Return whether the buffer is empty
            */
            bool empty() const
            {
                assert_invariants();

                return (*_ptr).empty();
            }


            /*!
                @brief      Return an iterator to the beginning of the buffer
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
                @overload
            */
            ConstIterator begin() const
            {
                assert_invariants();

                return (*_ptr).begin();
            }


            /*!
                @overload
            */
            ConstIterator end() const
            {
                assert_invariants();

                return (*_ptr).end();
            }


            /*!
                @brief      Access an element by index
            */
            Element& operator[](Index const idx)
            {
                assert_invariants();
                lue_hpx_assert(idx < static_cast<Index>((*_ptr).size()));

                return (*_ptr)[idx];
            }


            /*!
                @overload
            */
            Element const& operator[](Index const idx) const
            {
                assert_invariants();
                lue_hpx_assert(idx < static_cast<Index>((*_ptr).size()));

                return (*_ptr)[idx];
            }


            /*!
                @brief      Resize the buffer
            */
            void resize(Size const size)
            {
                assert_invariants();

                (*_ptr).resize(size, boost::container::default_init_t{});

                assert_invariants();
            }


            void clear()
            {
                assert_invariants();

                (*_ptr).clear();

                assert_invariants();
            }


            long use_count() const
            {
                return _ptr.use_count();
            }


            /*!
                @brief      Return whether or not this instance equals @a other

                Returns true when both the buffer sizes are equal and the
                elements themselves are equal.
            */
            bool operator==(SharedBuffer const& other) const
            {
                assert_invariants();
                other.assert_invariants();

                return this->size() == other.size() && std::equal(this->begin(), this->end(), other.begin());
            }


            /*!
                @brief      Remove elements from the array

                Trailing elements will be moved to the front, to take in the
                place of the removed elements. The memory buffer will not
                be resized.
            */
            void erase(Iterator const begin, Iterator const end)
            {
                assert_invariants();

                auto& container{*_ptr};

                lue_hpx_assert(begin >= container.begin());
                lue_hpx_assert(end <= container.end());

                container.erase(begin, end);

                assert_invariants();
            }


        private:

            void allocate(Size const size)
            {
                lue_hpx_assert(!_ptr);

                // Default initialization of the elements. In case of
                // numeric values, the values are indeterminate
                _ptr = std::make_shared<Elements>(size, boost::container::default_init_t{});

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
