#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/debug.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/framework/core/span.hpp"
#include "lue/configure.hpp"
#include <memory>


namespace lue {

    /*!
        @brief      Class template for representing arrays
        @tparam     Element Type for representing element values
        @tparam     rank Array rank

        The array elements are represented by 1D contiguous buffer of elements, in row-major order.
    */
    template<typename Element, Rank rank>
    class Array
    {

        public:

            using Span = DynamicSpan<Element, rank>;

            using Slice = std::pair<lue::Index, lue::Index>;

            using Shape = lue::Shape<lue::Index, rank>;

            using Elements = std::unique_ptr<Element[]>;

            using Iterator = Element*;

            using ConstIterator = Element const*;


            /*!
                @brief      Construct an empty array
            */
            Array():

                _shape{},
                _nr_elements{0},
                _buffer_size{0},
                _elements{nullptr},
                _span{}

            {
                // The shape is filled with indeterminate values! Explicitly set things up for the default
                // constructed instance.
                _shape.fill(0);
                _span = Span{_elements.get(), _shape};

                assert_invariants();
            }


            /*!
                @brief      Construct an array
                @param      shape Shape of the array

                The element values are value-initialized (zero-initialized in case Element is not a class
                type).
            */
            explicit Array(Shape const& shape):

                _shape{shape},
                _nr_elements{lue::nr_elements(shape)},
                _buffer_size{_nr_elements},
                _elements{_buffer_size == 0 ? nullptr : std::make_unique<Element[]>(_buffer_size)},
                _span{_elements.get(), _shape}

            {
                assert_invariants();
            }


            /*!
                @brief      Construct an array
                @param      shape Shape of the array
                @param      value Value to initialize all element values with
            */
            Array(Shape const& shape, Element const value):

                Array{shape}

            {
                std::fill(begin(), end(), value);
            }


            /*!
                @brief      Construct an array and move @values into the elements
                @warning    The number of elements in @a values must be equal to the number of elements
                            in @a shape
            */
            Array(Shape const& shape, std::initializer_list<Element> values):

                Array{shape}

            {
                lue_hpx_assert(std::distance(values.begin(), values.end()) == _nr_elements);

                std::move(values.begin(), values.end(), begin());
            }


            /*!
                @brief      Construct an array and move values in range [@a begin - @a end) into the elements
                @warning    The number of elements in the range must be equal to the number of elements
                            in @a shape
            */
            template<typename InputIterator>
            Array(Shape const& shape, InputIterator begin, InputIterator end):

                Array{shape}

            {
                lue_hpx_assert(std::distance(begin, end) == _nr_elements);

                std::move(begin, end, begin());
            }


            /*!
                @brief      Copy-construct an array
            */
            Array(Array const& other):

                Array{other._shape}

            {
                std::copy(other.begin(), other.end(), begin());
            }


            /*!
                @brief      Move-construct an array
            */
            Array(Array&& other) noexcept:

                _shape{std::move(other._shape)},
                _nr_elements{other._nr_elements},
                _buffer_size{other._buffer_size},
                _elements{std::move(other._elements)},
                _span{_elements.get(), _shape}

            {
                other.clear();
                other.assert_invariants();
                assert_invariants();
            }


            ~Array() = default;


            /*!
                @brief      Copy-assign @a other to this array
            */
            auto operator=(Array const& other) -> Array&
            {
                if (this != &other)
                {
                    reshape(other._shape);
                    std::copy(other.begin(), other.end(), begin());
                }

                assert_invariants();

                return *this;
            }


            /*!
                @brief      Move-assign @a other to this array
            */
            auto operator=(Array&& other) noexcept -> Array&
            {
                if (this != &other)
                {
                    _shape = std::move(other._shape);
                    _nr_elements = other._nr_elements;
                    _buffer_size = other._buffer_size;
                    _elements = std::move(other._elements);
                    _span = Span{_elements.get(), _shape};

                    other.clear();
                }

                other.assert_invariants();
                lue_hpx_assert(other.empty());
                assert_invariants();

                return *this;
            }


            /*!
                @brief      Return the number of elements in the array
            */
            auto nr_elements() const -> Count
            {
                return _nr_elements;
            }


            /*!
                @brief      Return whether the array is empty
            */
            auto empty() const -> bool
            {
                return _nr_elements == 0;
            }


            /*!
                @brief      Return the array shape
            */
            auto shape() const -> Shape const&
            {
                return _shape;
            }


            /*!
                @brief      Return the extent of the array along the @a dimension_idx
                @warning    @a dimension_idx must be less than the rank
            */
            auto extent(Rank const dimension_idx) const -> lue::Index
            {
                lue_hpx_assert(dimension_idx < rank);

                return _shape[dimension_idx];
            }


            /*!
                @brief      Return begin iterator
            */
            auto begin() const -> ConstIterator
            {
                return _elements.get();
            }


            /*!
                @overload
            */
            auto begin() -> Iterator
            {
                return _elements.get();
            }


            /*!
                @brief      Return end iterator
            */
            auto end() const -> ConstIterator
            {
                return begin() + _nr_elements;
            }


            /*!
                @overload
            */
            auto end() -> Iterator
            {
                return begin() + _nr_elements;
            }


            /*!
                @brief      Remove elements in range [@a begin - @a end)

                Elements located after the range to remove are moved into range of removed elements. The
                underlying buffer is not resized.
            */
            void remove(Iterator begin, Iterator end)
            {
                lue_hpx_assert(std::distance(begin, end) <= _nr_elements);

                std::move(end, this->end(), begin);
                _nr_elements -= std::distance(begin, end);
            }


            /*!
                @brief      Clear the array

                The underlying buffer is reset.
            */
            void clear()
            {
                _shape.fill(0);
                _nr_elements = 0;
                _buffer_size = 0;
                _elements.reset();
                _span = Span{_elements.get(), _shape};

                lue_hpx_assert(empty());
                assert_invariants();
            }


            /*!
                @brief      Reshape the array to @a shape

                If the underlying buffer is large enough to contain the elements of the new shape, then
                this buffer is not resized.
            */
            void reshape(Shape const& shape)
            {
                if (_shape != shape)
                {
                    _shape = shape;

                    if (_nr_elements != lue::nr_elements(shape))
                    {
                        _nr_elements = lue::nr_elements(shape);

                        if (_buffer_size < _nr_elements)
                        {
                            // Current buffer doesn't contain enough room for the elements
                            _buffer_size = _nr_elements;

                            if (_buffer_size == 0)
                            {
                                _elements.reset();
                            }
                            else
                            {

                                _elements = std::make_unique<Element[]>(_buffer_size);
                            }
                        }
                    }

                    _span = Span{_elements.get(), _shape};
                }

                assert_invariants();
            }


            /*!
                @brief      Return a pointer to the start of the underlying element buffer
            */
            auto data() const -> Element const*
            {
                return _elements.get();
            }


            /*!
                @overload
            */
            auto data() -> Element*
            {
                return _elements.get();
            }


            /*!
                @brief      Return the element at @a idxs in the array
                @warning    The number of indices must be equal to the rank
            */
            template<typename... Idxs>
            auto operator()(Idxs... idxs) const -> Element const&
            {
                static_assert(sizeof...(idxs) == rank);

                if constexpr (BuildOptions::validate_idxs)
                {
                    validate_idxs(_shape, idxs...);
                }

                return _span[idxs...];
            }


            /*!
                @overload
            */
            template<typename... Idxs>
            auto operator()(Idxs... idxs) -> Element&
            {
                static_assert(sizeof...(idxs) == rank);

                if constexpr (BuildOptions::validate_idxs)
                {
                    validate_idxs(_shape, idxs...);
                }

                return _span[idxs...];
            }


            /*!
                @brief      Return the element at @a idx in the array
                @warning    @a idx must be smaller than the number of elements
            */
            auto operator[](std::size_t idx) const -> Element const&
            {
                lue_hpx_assert(static_cast<Count>(idx) < _nr_elements);

                return this->data()[idx];
            }


            /*!
                @overload
            */
            auto operator[](std::size_t idx) -> Element&
            {
                lue_hpx_assert(static_cast<Count>(idx) < _nr_elements);

                return this->data()[idx];
            }


            /*!
                @brief      Return the array span
            */
            auto span() const -> Span const&
            {
                return _span;
            }


        private:

            void assert_invariants() const
            {
                lue_hpx_assert(lue::nr_elements(_shape) == _nr_elements);
                lue_hpx_assert(static_cast<Count>(_span.size()) == _nr_elements);
                lue_hpx_assert(_span.data_handle() == _elements.get());
                lue_hpx_assert(
                    (_nr_elements == 0 && data() == nullptr) || (_nr_elements > 0 && data() != nullptr));
                lue_hpx_assert(_nr_elements <= _buffer_size);
            }


            //! Shape of the array
            Shape _shape;

            //! Number of elements in the array
            Count _nr_elements;

            //! Number of elements in the underlying buffer
            Count _buffer_size;

            //! 1D buffer with array elements
            Elements _elements;

            //! Span for converting nD indices to linear indices
            Span _span;
    };


    namespace detail {

        template<typename E, Rank r>
        class ArrayTraits<lue::Array<E, r>>
        {

            public:

                using Element = E;

                constexpr static Rank rank = r;

                using Shape = typename lue::Array<E, r>::Shape;
        };

    }  // namespace detail


    /*!
        @brief      Return the shape of @a array
    */
    template<typename Element, Rank rank>
    inline auto shape(Array<Element, rank> const& array) -> auto const&
    {
        return array.shape();
    }


    /*!
        @brief      Return the number of elements in @a array
    */
    template<typename Element, Rank rank>
    inline auto nr_elements(Array<Element, rank> const& array)
    {
        return array.nr_elements();
    }

}  // namespace lue
