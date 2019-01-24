#include "lue/framework/core/component/array_partition.hpp"


namespace lue {
namespace client {

template<
    typename Data>
ArrayPartition<Data>::ArrayPartition(
    hpx::id_type const& id):

    Base{id}

{
}


template<
    typename Data>
ArrayPartition<Data>::ArrayPartition(
    hpx::future<hpx::id_type>&& id):

    Base{std::move(id)}

{
}


template<
    typename Data>
ArrayPartition<Data>::ArrayPartition(
    hpx::id_type const& where,
    Definition const& definition):

    Base{hpx::new_<Server>(where, definition)}

{
}


template<
    typename Data>
ArrayPartition<Data>::ArrayPartition(
    hpx::id_type const& where,
    Definition const& definition,
    Value value):

    Base{hpx::new_<Server>(where, definition, value)}

{
}


template<
    typename Data>
std::shared_ptr<typename ArrayPartition<Data>::Server>
    ArrayPartition<Data>::component() const
{
    return hpx::get_ptr<Server>(this->get_id()).get();
}


// template class ArrayPartition<std::uint64_t, bool, 2>;
// template class ArrayPartition<std::uint64_t, std::int32_t, 2>;
// template class ArrayPartition<std::uint64_t, double, 2>;

}  // namespace client
}  // namespace lue
