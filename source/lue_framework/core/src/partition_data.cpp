#include <hpx/config.hpp>
#include "lue/framework/core/partition_data.hpp"
#include <hpx/parallel/algorithms/fill.hpp>


namespace lue {

template<
    typename Index,
    typename Value,
    std::size_t rank>
PartitionData<Index, Value, rank>::PartitionData():

    _definition{},
    _values{}

{
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
PartitionData<Index, Value, rank>::PartitionData(
    Definition const& definition):

    _definition{definition},
    _values(definition.shape())

{
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
PartitionData<Index, Value, rank>::PartitionData(
    Definition const& definition,
    Value const& value):

    _definition{definition},
    _values(definition.shape())

{
    hpx::parallel::fill_n(
        hpx::parallel::execution::par,
        _values.data(), _values.num_elements(), value);
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
inline bool PartitionData<Index, Value, rank>::operator==(
    PartitionData const& other) const
{
    return _definition == other._definition && _values == other._values;
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
typename PartitionData<Index, Value, rank>::Definition const&
    PartitionData<Index, Value, rank>::definition() const
{
    return _definition;
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
typename PartitionData<Index, Value, rank>::Values const&
    PartitionData<Index, Value, rank>::values() const
{
    return _values;
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
typename PartitionData<Index, Value, rank>::Values&
    PartitionData<Index, Value, rank>::values()
{
    return _values;
}


// template<
//     typename Value,
//     std::size_t rank>
// Value PartitionData<Index, Value, rank>::value(
//     std::size_t const row_idx,
//     std::size_t const col_idx) const
// {
//     assert(rank == 2);
// 
//     assert(row_idx >= _definition.begin_idx(0));
//     assert(row_idx < _definition.end_idx(0));
//     assert(col_idx >= _definition.begin_idx(1));
//     assert(col_idx < _definition.end_idx(1));
// 
//     std::size_t const r = row_idx - _definition.begin_idx(0);
//     std::size_t const c = col_idx - _definition.begin_idx(1);
//     // std::size_t const nr_cols = _definition.size(1);
//     // std::size_t const idx = r * nr_cols + c;
// 
//     // assert(idx < _values.size());
// 
//     return _values[r][c];
// }
// 
// 
// // TODO Use enable_if to only enable this overload in case rank==2
// //      Otherwise compilation fails for rank!=2
// template<
//     typename Value,
//     std::size_t rank>
// void PartitionData<Index, Value, rank>::set_value(
//     std::size_t const row_idx,
//     std::size_t const col_idx,
//     Value const& value)
// {
//     assert(rank == 2);
// 
//     assert(row_idx >= _definition.begin_idx(0));
//     assert(row_idx < _definition.end_idx(0));
//     assert(col_idx >= _definition.begin_idx(1));
//     assert(col_idx < _definition.end_idx(1));
// 
//     std::size_t const r = row_idx - _definition.begin_idx(0);
//     std::size_t const c = col_idx - _definition.begin_idx(1);
//     // std::size_t const nr_cols = _definition.size(1);
//     // std::size_t const idx = r * nr_cols + c;
// 
//     // assert(idx < _values.size());
// 
//     _values[r][c] = value;
// }


template class PartitionData<std::uint64_t, bool, 2>;
template class PartitionData<std::uint64_t, std::int32_t, 2>;
template class PartitionData<std::uint64_t, double, 2>;

}  // namespace lue
