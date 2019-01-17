#pragma once
#include <hpx/config.hpp>
#include "lue/framework/core/array_partition_definition.hpp"
#include <hpx/parallel/algorithms/copy.hpp>
#include <hpx/parallel/algorithms/fill.hpp>
#include <boost/multi_array.hpp>


namespace lue {

template<
    typename Index,
    typename Value,
    std::size_t rank>
class ArrayPartitionData
{

public:

    using Definition = ArrayPartitionDefinition<Index, rank>;

    using Values = boost::multi_array<Value, rank>;

                   ArrayPartitionData  ();

                   ArrayPartitionData  (Definition const& definition);

                   ArrayPartitionData  (Definition const& definition,
                                        Value const& value);

                   ArrayPartitionData  (ArrayPartitionData const& other);

                   ArrayPartitionData  (ArrayPartitionData&& other)=delete;

                   ~ArrayPartitionData ()=default;

    ArrayPartitionData& operator=      (ArrayPartitionData const& other);

    ArrayPartitionData& operator=      (ArrayPartitionData&& other);

    bool           operator==          (ArrayPartitionData const& other) const;

    Definition const& definition       () const;

    Values const&  values              () const;

    Values&        values              ();

private:

    Definition     _definition;

    Values         _values;

};


template<
    typename Index,
    typename Value,
    std::size_t rank>
inline ArrayPartitionData<Index, Value, rank>::ArrayPartitionData():

    _definition{},
    _values{}

{
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
inline ArrayPartitionData<Index, Value, rank>::ArrayPartitionData(
    Definition const& definition):

    _definition{definition},
    _values{definition.shape()}

{
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
inline ArrayPartitionData<Index, Value, rank>::ArrayPartitionData(
    Definition const& definition,
    Value const& value):

    _definition{definition},
    _values{definition.shape()}

{
    hpx::parallel::fill_n(
        hpx::parallel::execution::par,
        _values.data(), _values.num_elements(), value);
}


/*!
    @brief      Copy construct and instance based on @a other
*/
template<
    typename Index,
    typename Value,
    std::size_t rank>
inline ArrayPartitionData<Index, Value, rank>::ArrayPartitionData(
    ArrayPartitionData const& other):

    _definition{other._definition},
    _values{other._definition.shape()}

{
    auto const src_begin = other._values.data();
    auto const src_end = src_begin + other._values.num_elements();
    auto dst_begin = _values.data();

    hpx::parallel::copy(
        hpx::parallel::execution::par, src_begin, src_end, dst_begin);
}


/*!
    @brief      Assign @a other to this instance
*/
template<
    typename Index,
    typename Value,
    std::size_t rank>
inline ArrayPartitionData<Index, Value, rank>&
        ArrayPartitionData<Index, Value, rank>::operator=(
    ArrayPartitionData const& other)
{
    _definition = other._definition;
    _values.resize(other._definition.shape());

    auto const src_begin = other._values.data();
    auto const src_end = src_begin + other._values.num_elements();
    auto dst_begin = _values.data();

    hpx::parallel::copy(
        hpx::parallel::execution::par, src_begin, src_end, dst_begin);

    return *this;
}


/*!
    @brief      Move assign @a other to this instance
*/
template<
    typename Index,
    typename Value,
    std::size_t rank>
inline ArrayPartitionData<Index, Value, rank>&
        ArrayPartitionData<Index, Value, rank>::operator=(
    ArrayPartitionData&& other)
{
    _definition = std::move(other._definition);

    // Copy values and empty the source array afterwards
    _values.resize(_definition.shape());

    auto const src_begin = other._values.data();
    auto const src_end = src_begin + other._values.num_elements();
    auto dst_begin = _values.data();

    hpx::parallel::copy(
        hpx::parallel::execution::par, src_begin, src_end, dst_begin);

    std::array<Index, rank> empty_shape;
    empty_shape.fill(0);
    other._values.resize(empty_shape);

    return *this;
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
inline bool ArrayPartitionData<Index, Value, rank>::operator==(
    ArrayPartitionData const& other) const
{
    return _definition == other._definition && _values == other._values;
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
inline typename ArrayPartitionData<Index, Value, rank>::Definition const&
    ArrayPartitionData<Index, Value, rank>::definition() const
{
    return _definition;
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
inline typename ArrayPartitionData<Index, Value, rank>::Values const&
    ArrayPartitionData<Index, Value, rank>::values() const
{
    return _values;
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
inline typename ArrayPartitionData<Index, Value, rank>::Values&
    ArrayPartitionData<Index, Value, rank>::values()
{
    return _values;
}

}  // namespace lue
