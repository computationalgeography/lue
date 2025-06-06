#pragma once
#include "lue/framework/core/debug.hpp"
#include "lue/framework/core/define.hpp"
#include "lue/framework/core/erase.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/framework/core/span.hpp"
#include "lue/framework/core/type_traits.hpp"
#include "lue/assert.hpp"
#include "lue/configure.hpp"
#include <boost/container/vector.hpp>
#include <vector>


namespace lue {

    namespace detail {

        template<typename Element>
        struct ElementTraits
        {
                // Use std::vector by default
                using Vector = std::vector<Element>;
        };


        template<>
        struct ElementTraits<bool>
        {
                // Use Boost vector in case of bool. Using this type in all cases started throwing "multiply
                // defined symbols" errors with MSVC.
                using Vector = boost::container::vector<bool>;
        };


        template<typename Element>
        using ElementVectorT = ElementTraits<Element>::Vector;

    }  // namespace detail


    template<typename Element, Rank rank>
    class Array
    {

        public:

            using Span = DynamicSpan<Element, rank>;

            using Subspan = DynamicSubspan<Element, rank>;

            using Slice = std::pair<lue::Index, lue::Index>;

            using Slices = std::array<Slice, rank>;

            using Shape = lue::Shape<lue::Index, rank>;

            using Elements = detail::ElementVectorT<Element>;

            using Iterator = typename Elements::iterator;

            using ConstIterator = typename Elements::const_iterator;


            Array():

                _shape{},
                _elements{},
                _span{}

            {
                // The shape is filled with indeterminate values! Explicitly
                // set things up for the default constructed instance.
                _shape.fill(0);
                _span = Span{_elements.data(), _shape};

                assert_invariants();
            }


            explicit Array(Shape const& shape):

                _shape{shape},
                _elements(lue::nr_elements(shape)),
                _span{_elements.data(), _shape}

            {
                assert_invariants();
            }


            Array(Shape const& shape, Element const value):

                _shape{shape},
                _elements(lue::nr_elements(shape), value),
                _span{_elements.data(), _shape}

            {
                assert_invariants();
            }


            Array(Shape const& shape, std::initializer_list<Element> values):

                _shape{shape},
                _elements(lue::nr_elements(shape)),
                _span{_elements.data(), _shape}

            {
                lue_hpx_assert(std::distance(values.begin(), values.end()) == lue::nr_elements(_shape));

                std::move(values.begin(), values.end(), _elements.begin());

                assert_invariants();
            }


            template<typename InputIterator>
            Array(Shape const& shape, InputIterator begin, InputIterator end):

                _shape{shape},
                _elements(lue::nr_elements(shape)),
                _span{_elements.data(), _shape}

            {
                std::move(begin, end, _elements.begin());

                assert_invariants();
            }


            Array(Array const& other):

                _shape{other._shape},
                _elements{other._elements},
                _span{_elements.data(), _shape}

            {
                assert_invariants();
            }


            Array(Array&& other):

                _shape{std::move(other._shape)},
                _elements{std::move(other._elements)},
                _span{_elements.data(), _shape}

            {
                other._shape.fill(0);
                other._elements.clear();
                other._span = Span{other._elements.data(), other._shape};

                other.assert_invariants();
                lue_hpx_assert(other.empty());
                assert_invariants();
            }


            ~Array() = default;


            Array& operator=(Array const& other)
            {
                if (this != &other)
                {
                    _shape = other._shape;
                    _elements = other._elements;
                    _span = Span{_elements.data(), _shape};
                }

                assert_invariants();

                return *this;
            }


            Array& operator=(Array&& other)
            {
                if (this != &other)
                {
                    _shape = std::move(other._shape);
                    other._shape.fill(0);

                    _elements = std::move(other._elements);
                    other._elements.clear();

                    _span = Span{_elements.data(), _shape};
                    other._span = Span{other._elements.data(), other._shape};
                }

                other.assert_invariants();
                lue_hpx_assert(other.empty());
                assert_invariants();

                return *this;
            }


            bool empty() const
            {
                return _elements.empty();
            }


            Shape const& shape() const
            {
                return _shape;
            }


            lue::Index extent(Rank const dimension_idx) const
            {
                return _shape[dimension_idx];
            }


            Iterator begin()
            {
                return _elements.begin();
            }


            Iterator end()
            {
                return _elements.end();
            }


            ConstIterator begin() const
            {
                return _elements.begin();
            }


            ConstIterator end() const
            {
                return _elements.end();
            }


            void reshape(Shape const& shape)
            {
                if (_shape != shape)
                {
                    _elements.resize(lue::nr_elements(shape));
                    _shape = shape;
                    _span = Span{_elements.data(), _shape};
                }

                assert_invariants();
            }


            void erase(
                Rank const dimension_idx, Index const hyperslab_begin_idx, Index const hyperslab_end_idx)
            {
                _shape = lue::erase(_elements, _shape, dimension_idx, hyperslab_begin_idx, hyperslab_end_idx);
                _span = Span{_elements.data(), _shape};

                assert_invariants();
            }


            Element const* data() const
            {
                return _elements.data();
            }


            Element* data()
            {
                return _elements.data();
            }


            template<typename... Idxs>
            Element const& operator()(Idxs... idxs) const
            {
                if constexpr (BuildOptions::validate_idxs)
                {
                    validate_idxs(_shape, idxs...);
                }

                return _span(idxs...);
            }


            template<typename... Idxs>
            Element& operator()(Idxs... idxs)
            {
                if constexpr (BuildOptions::validate_idxs)
                {
                    validate_idxs(_shape, idxs...);
                }

                return _span(idxs...);
            }


            Element const& operator[](std::size_t idx) const
            {
                return this->data()[idx];
            }


            Element& operator[](std::size_t idx)
            {
                return this->data()[idx];
            }


            Span const& span() const
            {
                return _span;
            }


        private:

            void assert_invariants() const
            {
                // lue_hpx_assert(
                //     static_cast<Size>(_elements.size()) == lue::nr_elements(_shape));
                lue_hpx_assert(lue::nr_elements(_shape) == static_cast<Size>(_elements.size()));
                // lue_hpx_assert(_span.size() == _elements.size());
                // lue_hpx_assert(_span.data_handle() == _elements.data());
                lue_hpx_assert(static_cast<Count>(_span.size()) == static_cast<Count>(_elements.size()));
                lue_hpx_assert(_span.data_handle() == _elements.data());
            }


            // Shape of array
            Shape _shape;

            // 1D buffer with array elements
            Elements _elements;

            // Span for converting nD indices to linear indices
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


    template<typename Element, Rank rank>
    inline auto const& shape(Array<Element, rank> const& array)
    {
        return array.shape();
    }


    template<typename Element, Rank rank>
    inline auto nr_elements(Array<Element, rank> const& array)
    {
        return nr_elements(shape(array));
    }

}  // namespace lue
