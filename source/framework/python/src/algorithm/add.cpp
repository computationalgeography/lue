#include "lue/framework/algorithm/add.hpp"
#include <pybind11/pybind11.h>


namespace lue {
namespace framework {
namespace {

template<
    typename Element,
    Rank rank,
    typename Input1,
    typename Input2>
PartitionedArray<Element, rank> addx(
    Input1 const& lhs,
    Input2 const& rhs)
{
    using Policies = policy::add::DefaultValuePolicies<Element, Element>;

    return add(Policies{}, lhs, rhs);
}

}  // Anonymous namespace


#define LUE_BINARY_ADD_OVERLOADS(type, rank) \
    module.def("add", addx<type, rank, PartitionedArray<type, rank>, PartitionedArray<type, rank>>); \
    module.def("add", addx<type, rank, PartitionedArray<type, rank>, type>); \
    module.def("add", addx<type, rank, PartitionedArray<type, rank>, hpx::shared_future<type>>); \
    module.def("add", addx<type, rank, type, PartitionedArray<type, rank>>); \
    module.def("add", addx<type, rank, hpx::shared_future<type>, PartitionedArray<type, rank>>);


void bind_add(
    pybind11::module& module)
{
    // TODO How to document these?
    LUE_BINARY_ADD_OVERLOADS(std::uint8_t, 2)
    LUE_BINARY_ADD_OVERLOADS(std::uint32_t, 2)
    LUE_BINARY_ADD_OVERLOADS(std::uint64_t, 2)
    LUE_BINARY_ADD_OVERLOADS(std::int32_t, 2)
    LUE_BINARY_ADD_OVERLOADS(std::int64_t, 2)
    LUE_BINARY_ADD_OVERLOADS(float, 2)
    LUE_BINARY_ADD_OVERLOADS(double, 2)
}

}  // namespace framework
}  // namespace lue
