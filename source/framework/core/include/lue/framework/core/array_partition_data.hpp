#pragma once
#include "lue/framework/core/debug.hpp"
#include "lue/framework/core/define.hpp"
#include "lue/framework/core/erase.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/framework/core/serialize/shared_buffer.hpp"
#include "lue/framework/core/span.hpp"
#include "lue/framework/core/type_traits.hpp"
#include "lue/configure.hpp"
#include "lue/define.hpp"
#include <initializer_list>


namespace lue {

/*!
    @brief      Class for keeping track of array partition data values
    @warning    Copies of ArrayPartitionData instances share the
                underlying array with elements

    There is a specialization for array scalars (arrays with rank ==
    0). This allows scalars to be handled the same as arrays.
*/
template<
    typename Element,
    Rank rank>
class ArrayPartitionData
{

private:

    using Elements = SharedBuffer<Element>;

public:

    using Span = DynamicSpan<Element, rank>;

    using Subspan = DynamicSubspan<Element, rank>;

    using Count = lue::Count;

    using Shape = lue::Shape<Count, rank>;

    using Slice = std::pair<std::ptrdiff_t, std::ptrdiff_t>;

    using Slices = std::array<Slice, rank>;

    using Iterator = typename Elements::Iterator;

    using ConstIterator = typename Elements::ConstIterator;

                   ArrayPartitionData  ();

    explicit       ArrayPartitionData  (Shape const& shape);

                   ArrayPartitionData  (Shape const& shape,
                                        Element const& value);

    template<
        typename InputIterator>
                   ArrayPartitionData  (Shape const& shape,
                                        InputIterator begin,
                                        InputIterator end);

                   ArrayPartitionData  (Shape const& shape,
                                        std::initializer_list<Element>
                                            elements);

                   ArrayPartitionData  (ArrayPartitionData const&)=default;

                   ArrayPartitionData  (ArrayPartitionData&& other);

                   ~ArrayPartitionData ()=default;

    ArrayPartitionData& operator=      (ArrayPartitionData const&)=default;

    ArrayPartitionData& operator=      (ArrayPartitionData&& other);

    bool           operator==          (ArrayPartitionData const& other) const;

    Shape const&   shape               () const;

    Count          nr_elements         () const;

    bool           empty               () const;

    void           reshape             (Shape const& shape);

    void           erase               (Rank const dimension_idx,
                                        Index const hyperslab_begin_idx,
                                        Index const hyperslab_end_idx);

    ConstIterator  begin               () const;

    Iterator       begin               ();

    ConstIterator  end                 () const;

    Iterator       end                 ();

    Element&       operator[]          (Index idx);

    Element const& operator[]          (Index idx) const;

    Span const&    span                () const;

    template<
        typename... Idxs>
    Element const& operator()(
        Idxs... idxs) const
    {
        if constexpr(BuildOptions::validate_idxs)
        {
            validate_idxs(_shape, idxs...);
        }

        return _span(idxs...);
    }

    template<
        typename... Idxs>
    Element& operator()(
        Idxs... idxs)
    {
        if constexpr(BuildOptions::validate_idxs)
        {
            validate_idxs(_shape, idxs...);
        }

        return _span(idxs...);
    }

    ArrayPartitionData slice           (Slices const& slices) const;

private:

    Elements const& elements           () const;

    Elements&      elements            ();

    void           assert_invariants   () const;

    friend class hpx::serialization::access;

    void serialize(
        hpx::serialization::input_archive& archive,
        unsigned int const /* version */)
    {
        archive & _shape & _elements;
        _span = Span{_elements.data(), _shape};

        assert_invariants();
    }

    void serialize(
        hpx::serialization::output_archive& archive,
        unsigned int const /* version */) const
    {
        assert_invariants();

        archive & _shape & _elements;
    }

    Shape          _shape;

    Elements       _elements;

    // Span for converting nD indices to linear indices
    Span           _span;

};


/*!
    @brief      Default-construct an instance

    The instance will be an empty partition.
*/
template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank>::ArrayPartitionData():

    ArrayPartitionData{Shape{}}

{
    assert_invariants();
}


/*!
    @brief      Construct an instance with shape @a shape
    @param      shape Shape of data array

    The elements will be default-initialized.
*/
template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank>::ArrayPartitionData(
    Shape const& shape):

    _shape{shape},
    _elements{lue::nr_elements(shape)},
    _span{_elements.data(), _shape}

{
    assert_invariants();
}


/*!
    @brief      Construct an instance with a certain shape and fill-value
    @param      shape Shape of data array to create
    @param      value Value to fill array with
*/
template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank>::ArrayPartitionData(
    Shape const& shape,
    Element const& value):

    ArrayPartitionData{shape}

{
    std::fill_n(_elements.begin(), _elements.size(), value);

    assert_invariants();
}


/*!
    @brief      Construct an instance with a certain shape and initial elements
    @param      shape Shape of data array to create
    @param      begin Iterator to begin of range of elements to move
    @param      last Iterator to end of range of elements to move
    @warning    The range of elements passed in is *moved* into the instance
*/
template<
    typename Element,
    Rank rank>
template<
    typename InputIterator>
ArrayPartitionData<Element, rank>::ArrayPartitionData(
    Shape const& shape,
    InputIterator begin,
    InputIterator end):

    ArrayPartitionData{shape}

{
    lue_assert(std::distance(begin, end) == _elements.size());

    std::move(begin, end, _elements.begin());

    assert_invariants();
}


template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank>::ArrayPartitionData(
    Shape const& shape,
    std::initializer_list<Element> elements):

    ArrayPartitionData{shape}

{
    std::copy(elements.begin(), elements.end(), _elements.begin());

    assert_invariants();
}


/*!
    @brief      Move-construct and instance based on @a other
*/
template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank>::ArrayPartitionData(
    ArrayPartitionData&& other):

    _shape{std::move(other._shape)},
    _elements{std::move(other._elements)},
    _span{_elements.data(), _shape}

{
    other._shape.fill(0);
    other._elements.clear();
    other._span = Span{other._elements.data(), other._shape};

    other.assert_invariants();
    lue_assert(other.empty());
    assert_invariants();
}


/*!
    @brief      Move assign @a other to this instance
*/
template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank>& ArrayPartitionData<Element, rank>::operator=(
    ArrayPartitionData&& other)
{
    if(this != &other)
    {
        _shape = std::move(other._shape);
        other._shape.fill(0);

        _elements = std::move(other._elements);
        other._elements.clear();

        _span = Span{_elements.data(), _shape};
        other._span = Span{other._elements.data(), other._shape};

        other.assert_invariants();
        lue_assert(other.empty());
    }

    assert_invariants();

    return *this;
}


template<
    typename Element,
    Rank rank>
bool ArrayPartitionData<Element, rank>::operator==(
    ArrayPartitionData const& other) const
{
    return _shape == other._shape && elements() == other.elements();
}


template<
    typename Element,
    Rank rank>
typename ArrayPartitionData<Element, rank>::Shape const&
    ArrayPartitionData<Element, rank>::shape() const
{
    return _shape;
}


template<
    typename Element,
    Rank rank>
typename ArrayPartitionData<Element, rank>::Count
    ArrayPartitionData<Element, rank>::nr_elements() const
{
    return lue::nr_elements(_shape);
}


template<
    typename Element,
    Rank rank>
bool ArrayPartitionData<Element, rank>::empty() const
{
    return nr_elements() == 0;
}


template<
    typename Element,
    Rank rank>
void ArrayPartitionData<Element, rank>::reshape(
    Shape const& shape)
{
    // Reshaping the elements while multiple ArrayPartitionData instances
    // refer to it is dangereous because of the _span member. The other
    // instance(s) do not have there _span member updated.
    // If this fires, move the span and shape into the
    // SharedBuffer(?). Still there might be an issue with multiple threads
    // using the data at the same time, while it is reshaped...
    lue_assert(_elements.use_count() == 1);

    if(_shape != shape)
    {
        _elements.resize(lue::nr_elements(shape));
        _shape = shape;
        _span = Span{_elements.data(), _shape};
    }

    assert_invariants();
}


/*!
    @brief      .
    @param      .
    @return     .
    @exception  .
    @warning    The underlying memory buffer is not resized. Array
                elements are moved within the buffer.
*/
template<
    typename Element,
    Rank rank>
void ArrayPartitionData<Element, rank>::erase(
    Rank const dimension_idx,
    Index const hyperslab_begin_idx,
    Index const hyperslab_end_idx)
{
    _shape = lue::erase(_elements, _shape, dimension_idx, hyperslab_begin_idx, hyperslab_end_idx);
    _span = Span{_elements.data(), _shape};

    assert_invariants();
}


template<
    typename Element,
    Rank rank>
typename ArrayPartitionData<Element, rank>::Elements const&
    ArrayPartitionData<Element, rank>::elements() const
{
    return _elements;
}


template<
    typename Element,
    Rank rank>
typename ArrayPartitionData<Element, rank>::Elements&
    ArrayPartitionData<Element, rank>::elements()
{
    return _elements;
}


template<
    typename Element,
    Rank rank>
typename ArrayPartitionData<Element, rank>::ConstIterator
    ArrayPartitionData<Element, rank>::begin() const
{
    return elements().begin();
}


template<
    typename Element,
    Rank rank>
typename ArrayPartitionData<Element, rank>::Iterator
    ArrayPartitionData<Element, rank>::begin()
{
    return elements().begin();
}


template<
    typename Element,
    Rank rank>
typename ArrayPartitionData<Element, rank>::ConstIterator
    ArrayPartitionData<Element, rank>::end() const
{
    return elements().end();
}


template<
    typename Element,
    Rank rank>
typename ArrayPartitionData<Element, rank>::Iterator
    ArrayPartitionData<Element, rank>::end()
{
    return elements().end();
}


template<
    typename Element,
    Rank rank>
Element& ArrayPartitionData<Element, rank>::operator[](
    Index const idx)
{
    lue_assert(idx < _elements.size());

    return _elements[idx];
}


template<
    typename Element,
    Rank rank>
Element const& ArrayPartitionData<Element, rank>::operator[](
    Index const idx) const
{
    lue_assert(idx < _elements.size());

    return _elements[idx];
}


template<
    typename Element,
    Rank rank>
typename ArrayPartitionData<Element, rank>::Span const&
    ArrayPartitionData<Element, rank>::span() const
{
    return _span;
}


template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank> ArrayPartitionData<Element, rank>::slice(
    Slices const& slices) const
{
    static_assert(rank > 0 && rank < 3);

    // TODO Maybe also allow a slice into a shared buffer. This would prevent
    //      the copy of the sliced elements into the new instance.

    if constexpr (rank == 1) {
        auto const [nr_elements] = _shape;
        LUE_UNUSED(nr_elements);

        auto const& slice = slices[0];
        auto const begin = std::get<0>(slice);
        auto const end = std::get<1>(slice);
        lue_assert(end >= begin);
        lue_assert(end <= nr_elements);
        auto const nr_elements_slice = end - begin;

        ArrayPartitionData sliced_data{Shape{{nr_elements_slice}}};

        if(!sliced_data.empty())
        {
            auto const source_begin = &(operator()(begin));
            auto destination = &(sliced_data(0));

            std::copy(source_begin, source_begin + nr_elements_slice, destination);
        }

        return sliced_data;
    }
    else {
        auto const [nr_elements0, nr_elements1] = _shape;

        auto const& slice0 = slices[0];
        auto const& slice1 = slices[1];

        auto const begin0 = std::get<0>(slice0);
        auto const end0 = std::get<1>(slice0);
        lue_assert(end0 >= begin0);
        lue_assert(end0 <= nr_elements0);
        auto const nr_elements0_slice = end0 - begin0;

        auto const begin1 = std::get<0>(slice1);
        auto const end1 = std::get<1>(slice1);
        lue_assert(end1 >= begin1);
        lue_assert(end1 <= nr_elements1);
        auto const nr_elements1_slice = end1 - begin1;

        ArrayPartitionData sliced_data{Shape{{nr_elements0_slice, nr_elements1_slice}}};

        if(!sliced_data.empty())
        {
            using Idx = std::tuple_element<0, Slice>::type;

            for(Idx r = 0; r < nr_elements0_slice; ++r)
            {
                auto const source_begin = &(operator()(begin0 + r, begin1));
                auto destination = &(sliced_data(r, 0));

                std::copy(source_begin, source_begin + nr_elements1_slice, destination);
            }
        }

        return sliced_data;
    }
}


template<
    typename Element,
    Rank rank>
void ArrayPartitionData<Element, rank>::assert_invariants() const
{
    lue_assert(lue::nr_elements(_shape) == _elements.size());
    lue_assert(_span.size() == _elements.size());
    lue_assert(_span.data() == _elements.data());
}


template<
    typename Element>
class ArrayPartitionData<Element, 0>
{

private:

    using Elements = std::array<Element, 1>;

public:

    using Count = lue::Count;

    using Shape = lue::Shape<Count, 0>;

    using Slice = std::pair<std::ptrdiff_t, std::ptrdiff_t>;

    using Slices = std::array<Slice, 0>;

    using Iterator = typename Elements::iterator;

    using ConstIterator = typename Elements::const_iterator;

                   ArrayPartitionData  ();

    explicit       ArrayPartitionData  (Element const& value);

    explicit       ArrayPartitionData  (Shape const& shape);

                   ArrayPartitionData  (Shape const& shape,
                                        Element const& value);

                   ArrayPartitionData  (ArrayPartitionData const&)=default;

                   ArrayPartitionData  (ArrayPartitionData&&)=default;

                   ~ArrayPartitionData ()=default;

    ArrayPartitionData& operator=      (ArrayPartitionData const&)=default;

    ArrayPartitionData& operator=      (ArrayPartitionData&&)=default;

    bool           operator==          (ArrayPartitionData const& other) const;

    Shape const&   shape               () const;

    Count          nr_elements         () const;

    bool           empty               () const;

    void           reshape             (Shape const& shape);

    ConstIterator  begin               () const;

    Iterator       begin               ();

    ConstIterator  end                 () const;

    Iterator       end                 ();

    Element&       operator[]          (Index idx);

    Element const& operator[]          (Index idx) const;

private:

    // All arguments passed to actions must support serialization,
    // even if they are never actually communicated between localities
    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(Archive& /* archive */, unsigned int const /* version */)
    {
        // For now, we do nothing. Revisit later.
        HPX_ASSERT(false);
    }

    Shape          _shape;

    Elements       _elements;

};


template<
    typename Element>
ArrayPartitionData<Element, 0>::ArrayPartitionData():

    _shape{},
    _elements{}

{
    lue_assert(_elements.size() == 1);
}


template<
    typename Element>
ArrayPartitionData<Element, 0>::ArrayPartitionData(
    Element const& value):

    ArrayPartitionData{}

{
    _elements[0] = value;
}


template<
    typename Element>
ArrayPartitionData<Element, 0>::ArrayPartitionData(
    [[maybe_unused]] Shape const& shape):

    ArrayPartitionData{}

{
    lue_assert(shape.empty());
}


template<
    typename Element>
ArrayPartitionData<Element, 0>::ArrayPartitionData(
    [[maybe_unused]] Shape const& shape,
    Element const& value):

    ArrayPartitionData{value}

{
    lue_assert(shape.empty());
}


template<
    typename Element>
bool ArrayPartitionData<Element, 0>::operator==(
    ArrayPartitionData const& other) const
{
    lue_assert(_shape == other._shape);

    return _elements[0] == other._elements[0];
}


template<
    typename Element>
typename ArrayPartitionData<Element, 0>::Shape const& ArrayPartitionData<Element, 0>::shape() const
{
    return _shape;
}


template<
    typename Element>
void ArrayPartitionData<Element, 0>::reshape(
    Shape const& /* shape */)
{
    // Cannot resize a scalar array
    HPX_ASSERT(false);
}


template<
    typename Element>
typename ArrayPartitionData<Element, 0>::Count ArrayPartitionData<Element, 0>::nr_elements() const
{
    lue_assert(lue::nr_elements(_shape) == 0);
    lue_assert(_elements.size() == 1);

    return 1;
}


template<
    typename Element>
bool ArrayPartitionData<Element, 0>::empty() const
{
    return false;
}


template<
    typename Element>
typename ArrayPartitionData<Element, 0>::ConstIterator ArrayPartitionData<Element, 0>::begin() const
{
    return _elements.begin();
}


template<
    typename Element>
typename ArrayPartitionData<Element, 0>::Iterator ArrayPartitionData<Element, 0>::begin()
{
    return _elements.begin();
}


template<
    typename Element>
typename ArrayPartitionData<Element, 0>::ConstIterator ArrayPartitionData<Element, 0>::end() const
{
    return _elements.end();
}


template<
    typename Element>
typename ArrayPartitionData<Element, 0>::Iterator ArrayPartitionData<Element, 0>::end()
{
    return _elements.end();
}


template<
    typename Element>
Element& ArrayPartitionData<Element, 0>::operator[](
    [[maybe_unused]] Index const idx)
{
    lue_assert(idx == 0);

    return _elements[0];
}


template<
    typename Element>
Element const& ArrayPartitionData<Element, 0>::operator[](
    [[maybe_unused]] Index const idx) const
{
    lue_assert(idx == 0);

    return _elements[0];
}


namespace detail {

template<
    typename E,
    Rank r>
class ArrayTraits<ArrayPartitionData<E, r>>
{

public:

    using Element = E;

    constexpr static Rank rank = r;

    using Shape = typename ArrayPartitionData<E, r>::Shape;

    using Slice = typename ArrayPartitionData<E, r>::Slice;

    using Slices = typename ArrayPartitionData<E, r>::Slices;

};

}  // namespace detail


template<
    typename Element,
    Rank rank>
inline auto const& shape(
    ArrayPartitionData<Element, rank> const& data)
{
    return data.shape();
}


template<
    typename Element,
    Rank rank>
inline auto nr_elements(
    ArrayPartitionData<Element, rank> const& data)
{
    return lue::nr_elements(shape(data));
}


template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank> deep_copy(
    ArrayPartitionData<Element, rank> const& data)
{
    ArrayPartitionData<Element, rank> copy{data.shape()};

    std::copy(data.begin(), data.end(), copy.begin());

    return copy;
}

}  // namespace lue
