#pragma once
#include "lue/framework/core/define.hpp"
#include "lue/framework/core/numa_domain.hpp"
#include "lue/framework/core/partition_allocator.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/framework/core/serialize/shared_buffer.hpp"
#include "lue/framework/core/span.hpp"
#include "lue/framework/core/type_traits.hpp"
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

    // using Allocator = PartitionAllocator<Element>;
    // using Allocator = std::allocator<Element>;
    using Allocator = NUMADomainAllocator<Element>;

    using Elements = SharedBuffer<Element, Allocator>;

public:

    using Span = DynamicSpan<Element, rank>;

    using Subspan = DynamicSubspan<Element, rank>;

    using Count = lue::Count;

    using Shape = lue::Shape<Count, rank>;

    using Slice = std::pair<std::ptrdiff_t, std::ptrdiff_t>;

    using Slices = std::array<Slice, rank>;

    using Iterator = typename Elements::iterator;

    using ConstIterator = typename Elements::const_iterator;

                   ArrayPartitionData  ();

                   ArrayPartitionData  (Shape const& shape,
                                        TargetIndex const target_idx);

                   ArrayPartitionData  (Shape const& shape,
                                        Element const& value,
                                        TargetIndex const target_idx);

    template<
        typename InputIterator>
                   ArrayPartitionData  (Shape const& shape,
                                        InputIterator begin,
                                        InputIterator end,
                                        TargetIndex const target_idx);

                   // ArrayPartitionData  (Shape const& shape,
                   //                      Elements const& elements);

                   ArrayPartitionData  (Shape const& shape,
                                        std::initializer_list<Element>
                                            elements,
                                        TargetIndex const target_idx);

                   ArrayPartitionData  (ArrayPartitionData const& other);

                   ArrayPartitionData  (ArrayPartitionData const& other,
                                        CopyMode const mode);

                   ArrayPartitionData  (ArrayPartitionData&& other);

                   ~ArrayPartitionData ()=default;

    ArrayPartitionData& operator=      (ArrayPartitionData const& other)=delete;

    ArrayPartitionData& operator=      (ArrayPartitionData&& other);

    bool           operator==          (ArrayPartitionData const& other) const;

    Shape const&   shape               () const;

    Count          nr_elements         () const;

    bool           empty               () const;

    void           reshape             (Shape const& shape);

    ConstIterator  begin               () const;

    Iterator       begin               ();

    ConstIterator  end                 () const;

    Iterator       end                 ();

    Element&       operator[]          (std::size_t idx);

    Element const& operator[]          (std::size_t idx) const;

    Span const&    span                () const;

    template<
        typename... Indxs>
    Element const& operator()(
        Indxs... idxs) const
    {
        return _span(idxs...);
    }

    template<
        typename... Indxs>
    Element& operator()(
        Indxs... idxs)
    {
        return _span(idxs...);
    }

    ArrayPartitionData slice           (Slices const& slices) const;

    TargetIndex target_idx() const
    {
        return _target_idx;
    }

private:

    Elements const& elements           () const;

    Elements&      elements            ();

    // All arguments passed to actions must support serialization,
    // even if they are never actually communicated between localities
    friend class hpx::serialization::access;

    template<typename Archive>
    void serialize(
        Archive& archive,
        unsigned int const /* version */)
    {
        archive & _shape & _elements;
    }

    //! The index of the target the elements are managed by
    TargetIndex    _target_idx;

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

    ArrayPartitionData{Shape{}, scattered_target_index()}

{
    assert(_elements.size() == lue::nr_elements(_shape));
}


/*!
    @brief      Construct an instance with shape @a shape
    @param      shape Shape of data array
*/
template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank>::ArrayPartitionData(
    Shape const& shape,
    TargetIndex const target_idx):

    _target_idx{target_idx},
    _shape{shape},
    _elements{
            lue::nr_elements(shape),
            Allocator{Targets(1, target(_target_idx))}
        },
    _span{_elements.data(), _shape}

{
    assert(_elements.size() == lue::nr_elements(_shape));
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
    Element const& value,
    TargetIndex const target_idx):

    ArrayPartitionData{shape, target_idx}

{
    std::fill_n(_elements.begin(), _elements.size(), value);

    assert(_elements.size() == lue::nr_elements(_shape));
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
    InputIterator end,
    TargetIndex const target_idx):

    ArrayPartitionData{shape, target_idx}

{
    std::move(begin, end, _elements.begin());

    assert(_elements.size() == lue::nr_elements(_shape));
}


template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank>::ArrayPartitionData(
    Shape const& shape,
    std::initializer_list<Element> elements,
    TargetIndex const target_idx):

    ArrayPartitionData{shape, target_idx}

{
    assert(static_cast<Count>(elements.size()) == lue::nr_elements(_shape));

    std::copy(elements.begin(), elements.end(), _elements.begin());

    assert(static_cast<Count>(_elements.size()) == lue::nr_elements(_shape));
}


template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank>::ArrayPartitionData(
    ArrayPartitionData const& other):

    ArrayPartitionData{other, CopyMode::share}

{
    // This constructor is used by ArrayPartitionData::set_data

    assert(_elements.data() == other._elements.data());
    assert(_elements.size() == lue::nr_elements(_shape));
}


/*!
    @brief      Copy-construct an instance based on @a other
*/
template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank>::ArrayPartitionData(
    ArrayPartitionData const& other,
    CopyMode const mode):

    _target_idx{other._target_idx},
    _shape{other._shape},
    _elements{other._elements, mode},
    _span{_elements.data(), _shape}

{
    assert(
        (mode == CopyMode::copy && (
            (_elements.data() != other._elements.data()) ||
            (_elements.data() == nullptr))) ||
        (mode == CopyMode::share && (
            _elements.data() == other._elements.data()))
    );
    assert(_elements.size() == lue::nr_elements(_shape));
}


/*!
    @brief      Move-construct and instance based on @a other
*/
template<
    typename Element,
    Rank rank>
ArrayPartitionData<Element, rank>::ArrayPartitionData(
    ArrayPartitionData&& other):

    _target_idx{other._target_idx},
    _shape{std::move(other._shape)},
    _elements{std::move(other._elements)},
    _span{_elements.data(), _shape}

{
    // An instance is already created. Moving-constructing a new instance
    // based on the existing instance just grabs the internals. No need to
    // be smart about NUMA domains here. Keep the elements where they are.

    assert(
        (_elements.data() != other._elements.data()) ||
        (_elements.data() == nullptr));
    assert(_elements.size() == lue::nr_elements(_shape));
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
    // WRT NUMA domain: Our elements are already located on a specific
    // domain. This domain is possibly different from the elements of the
    // source instance. Moving-assigning the existing instance just
    // grabs the internals. No need to be smart about NUMA domains
    // here. Keep the elements where they already are.

    _shape = std::move(other._shape);
    _elements = std::move(other._elements);
    _span = Span{_elements.data(), _shape};

    assert(
        (_elements.data() != other._elements.data()) ||
        (_elements.data() == nullptr));
    assert(_elements.size() == lue::nr_elements(_shape));

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
    assert(_elements.size() == lue::nr_elements(_shape));

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
    if(_shape != shape) {
        _elements.resize(lue::nr_elements(shape));
        _shape = shape;
        _span = Span{_elements.data(), _shape};
    }
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
    std::size_t const idx)
{
    return _elements[idx];
}


template<
    typename Element,
    Rank rank>
Element const& ArrayPartitionData<Element, rank>::operator[](
    std::size_t const idx) const
{
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
        assert(end >= begin);
        assert(end <= nr_elements);
        auto const nr_elements_slice = end - begin;

        // Place slice on same NUMA domain as we are
        ArrayPartitionData sliced_data{
            Shape{{nr_elements_slice}}, _target_idx};

        auto const source_begin = &(operator()(begin));
        auto destination = &(sliced_data(0));

        std::copy(
            source_begin, source_begin + nr_elements_slice, destination);

        return sliced_data;
    }
    else {
#ifndef NDEBUG
        auto const [nr_elements0, nr_elements1] = _shape;
#endif

        auto const& slice0 = slices[0];
        auto const& slice1 = slices[1];

        auto const begin0 = std::get<0>(slice0);
        auto const end0 = std::get<1>(slice0);
        assert(end0 >= begin0);
        assert(end0 <= nr_elements0);
        auto const nr_elements0_slice = end0 - begin0;

        auto const begin1 = std::get<0>(slice1);
        auto const end1 = std::get<1>(slice1);
        assert(end1 >= begin1);
        assert(end1 <= nr_elements1);
        auto const nr_elements1_slice = end1 - begin1;

        // Place slice on same NUMA domain as we are
        ArrayPartitionData sliced_data{
            Shape{{nr_elements0_slice, nr_elements1_slice}},
            _target_idx};

        using Idx = std::tuple_element<0, Slice>::type;

        for(Idx r = 0; r < nr_elements0_slice; ++r) {
            auto const source_begin = &(operator()(begin0 + r, begin1));
            auto destination = &(sliced_data(r, 0));

            std::copy(
                source_begin, source_begin + nr_elements1_slice, destination);
        }

        return sliced_data;
    }
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

                   ArrayPartitionData  (Element const& value,
                                        TargetIndex const target_idx);

                   ArrayPartitionData  (Shape const& shape,
                                        Element const& value,
                                        TargetIndex const target_idx);

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

    Element&       operator[]          (std::size_t idx);

    Element const& operator[]          (std::size_t idx) const;

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
    assert(_elements.size() == 1);
}


template<
    typename Element>
ArrayPartitionData<Element, 0>::ArrayPartitionData(
    Element const& value,
    TargetIndex const /* target_idx */):

    _shape{},
    _elements{}

{
    _elements[0] = value;

    assert(_elements.size() == 1);
}


template<
    typename Element>
ArrayPartitionData<Element, 0>::ArrayPartitionData(
    Shape const& /* shape */,
    Element const& value,
    TargetIndex const target_idx):

    ArrayPartitionData{value, target_idx}

{
    assert(_elements.size() == 1);
}


template<
    typename Element>
bool ArrayPartitionData<Element, 0>::operator==(
    ArrayPartitionData const& other) const
{
    assert(_shape == other._shape);

    return _elements[0] == other._elements[0];
}


template<
    typename Element>
typename ArrayPartitionData<Element, 0>::Shape const&
    ArrayPartitionData<Element, 0>::shape() const
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
typename ArrayPartitionData<Element, 0>::Count
    ArrayPartitionData<Element, 0>::nr_elements() const
{
    assert(lue::nr_elements(_shape) == 0);
    assert(_elements.size() == 1);

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
typename ArrayPartitionData<Element, 0>::ConstIterator
    ArrayPartitionData<Element, 0>::begin() const
{
    return _elements.begin();
}


template<
    typename Element>
typename ArrayPartitionData<Element, 0>::Iterator
    ArrayPartitionData<Element, 0>::begin()
{
    return _elements.begin();
}


template<
    typename Element>
typename ArrayPartitionData<Element, 0>::ConstIterator
    ArrayPartitionData<Element, 0>::end() const
{
    return _elements.end();
}


template<
    typename Element>
typename ArrayPartitionData<Element, 0>::Iterator
    ArrayPartitionData<Element, 0>::end()
{
    return _elements.end();
}


template<
    typename Element>
Element& ArrayPartitionData<Element, 0>::operator[](
    [[maybe_unused]] std::size_t const idx)
{
    assert(idx == 0);

    return _elements[0];
}


template<
    typename Element>
Element const& ArrayPartitionData<Element, 0>::operator[](
    [[maybe_unused]] std::size_t const idx) const
{
    assert(idx == 0);

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

}  // namespace lue
