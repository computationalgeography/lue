#include "lue/framework/core/component/server/array_partition.hpp"
#include <type_traits>


namespace lue {
namespace server {

// template<
//     typename Data>
// ArrayPartition<Data>::ArrayPartition(
//     Definition const& definition):
// 
//     Base{},
//     _data{definition}
// 
// {
// }
// 
// 
// template<
//     typename Data>
// ArrayPartition<Data>::ArrayPartition(
//     Definition const& definition,
//     Value value):
// 
//     Base{},
//     _data{definition, value}
// 
// {
//     static_assert(std::is_trivial_v<Value>);
// }
// 
// 
// template<
//     typename Data>
// Data const& ArrayPartition<Data>::data() const
// {
//     return _data;
// }
// 
// 
// template class ArrayPartition<std::uint64_t, bool, 2>;
// template class ArrayPartition<std::uint64_t, int32_t, 2>;
// template class ArrayPartition<std::uint64_t, double, 2>;

}  // namespace server
}  // namespace lue


// template<
//     typename Data>
// using PartitionServer =
//     hpx::components::component<lue::server::ArrayPartition<Data>>;


// using PartitionServer1 = PartitionServer<std::uint64_t, bool, 2>;
// using PartitionServer2 = PartitionServer<std::uint64_t, std::int32_t, 2>;
// using PartitionServer3 = PartitionServer<std::uint64_t, double, 2>;

// HPX_REGISTER_COMPONENT(PartitionServer1)
// HPX_REGISTER_COMPONENT(PartitionServer2)
// HPX_REGISTER_COMPONENT(PartitionServer3)
