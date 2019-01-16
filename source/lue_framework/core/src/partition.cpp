#include "lue/framework/core/component/partition.hpp"


namespace lue {
namespace client {

template<
    typename Index,
    typename Value,
    std::size_t rank>
Partition<Index, Value, rank>::Partition(
    hpx::id_type const& gid):

    Base{gid}

{
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
Partition<Index, Value, rank>::Partition(
    hpx::future<hpx::id_type>&& id):

    Base{std::move(id)}

{
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
Partition<Index, Value, rank>::Partition(
    hpx::id_type const& where,
    Definition const& definition):

    Base{hpx::new_<Server>(where, definition)}

{
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
Partition<Index, Value, rank>::Partition(
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
std::shared_ptr<typename Partition<Index, Value, rank>::Server>
    Partition<Index, Value, rank>::component() const
{
    return hpx::get_ptr<Server>(this->get_id()).get();
}


template class Partition<std::uint64_t, bool, 2>;
template class Partition<std::uint64_t, std::int32_t, 2>;
template class Partition<std::uint64_t, double, 2>;

}  // namespace client
}  // namespace lue
