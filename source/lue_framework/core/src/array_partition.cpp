#include "lue/framework/core/component/array_partition.hpp"


namespace lue {
namespace client {

template<
    typename Index,
    typename Value,
    std::size_t rank>
ArrayPartition<Index, Value, rank>::ArrayPartition(
    hpx::id_type const& gid):

    Base{gid}

{
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
ArrayPartition<Index, Value, rank>::ArrayPartition(
    hpx::future<hpx::id_type>&& id):

    Base{std::move(id)}

{
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
ArrayPartition<Index, Value, rank>::ArrayPartition(
    hpx::id_type const& where,
    Definition const& definition):

    Base{hpx::new_<Server>(where, definition)}

{
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
ArrayPartition<Index, Value, rank>::ArrayPartition(
    hpx::id_type const& where,
    Definition const& definition,
    Value value):

    Base{hpx::new_<Server>(where, definition, value)}

{
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
std::shared_ptr<typename ArrayPartition<Index, Value, rank>::Server>
    ArrayPartition<Index, Value, rank>::component() const
{
    return hpx::get_ptr<Server>(this->get_id()).get();
}


template class ArrayPartition<std::uint64_t, bool, 2>;
template class ArrayPartition<std::uint64_t, std::int32_t, 2>;
template class ArrayPartition<std::uint64_t, double, 2>;

}  // namespace client
}  // namespace lue
