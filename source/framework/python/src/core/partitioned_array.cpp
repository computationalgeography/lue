#include "lue/framework/core/component/partitioned_array.hpp"
#include "type_traits.hpp"
#include <pybind11/pybind11.h>


namespace lue {
namespace framework {
namespace {

template<
    typename Element,
    Rank rank>
void bind_partitioned_array(
    pybind11::module& module)
{
    pybind11::class_<lue::PartitionedArray<Element, rank>>(
        module,
        fmt::format("PartitionedArray{}", as_string<Element>()).c_str(),
        "PartitionedArray docstring...");
}

}  // Anonymous namespace


void bind_partitioned_array(
    pybind11::module& module)
{
    bind_partitioned_array<std::uint8_t, 2>(module);
    bind_partitioned_array<std::uint32_t, 2>(module);
    bind_partitioned_array<std::uint64_t, 2>(module);
    bind_partitioned_array<std::int8_t, 2>(module);
    bind_partitioned_array<std::int32_t, 2>(module);
    bind_partitioned_array<std::int64_t, 2>(module);
    bind_partitioned_array<float, 2>(module);
    bind_partitioned_array<double, 2>(module);
}

}  // namespace framework
}  // namespace lue
