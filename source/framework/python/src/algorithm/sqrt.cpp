#include "lue/framework/algorithm/sqrt.hpp"
#include <pybind11/pybind11.h>


namespace lue {
namespace framework {
namespace {

template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> sqrt(
    PartitionedArray<Element, rank> const& array)
{
    using Policies = policy::sqrt::DefaultValuePolicies<Element>;

    return sqrt(Policies{}, array);
}

}  // Anonymous namespace


void bind_sqrt(
    pybind11::module& module)
{
    // TODO How to document these?
    module.def("sqrt", sqrt<float, 2>);
    module.def("sqrt", sqrt<double, 2>);
}

}  // namespace framework
}  // namespace lue
