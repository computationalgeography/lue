#include "lue/framework/algorithm/minimum.hpp"
#include <pybind11/pybind11.h>


namespace lue {
namespace framework {
namespace {

template<
    typename Element,
    Rank rank>
hpx::shared_future<Element> minimum(
    PartitionedArray<Element, rank> const& array)
{
    using Policies = policy::minimum::DefaultValuePolicies<Element>;

    return minimum(Policies{}, array).share();
}

}  // Anonymous namespace


void bind_minimum(
    pybind11::module& module)
{
    // TODO How to document these?
    module.def("minimum", minimum<std::uint8_t, 1>);
    module.def("minimum", minimum<std::uint8_t, 2>);
    module.def("minimum", minimum<std::uint32_t, 1>);
    module.def("minimum", minimum<std::uint32_t, 2>);
    module.def("minimum", minimum<std::uint64_t, 1>);
    module.def("minimum", minimum<std::uint64_t, 2>);
    module.def("minimum", minimum<std::int32_t, 1>);
    module.def("minimum", minimum<std::int32_t, 2>);
    module.def("minimum", minimum<std::int64_t, 1>);
    module.def("minimum", minimum<std::int64_t, 2>);
    module.def("minimum", minimum<float, 1>);
    module.def("minimum", minimum<float, 2>);
    module.def("minimum", minimum<double, 1>);
    module.def("minimum", minimum<double, 2>);
}

}  // namespace framework
}  // namespace lue
