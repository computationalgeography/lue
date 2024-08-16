#include "lue/framework/algorithm/value_policies/minimum.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        auto minimum(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
        {
            return value_policies::minimum(array);
        }

    }  // Anonymous namespace


    void bind_minimum(pybind11::module& module)
    {
        module.def("minimum", minimum<std::uint8_t, 2>);
        module.def("minimum", minimum<std::uint32_t, 2>);
        module.def("minimum", minimum<std::uint64_t, 2>);
        module.def("minimum", minimum<std::int32_t, 2>);
        module.def("minimum", minimum<std::int64_t, 2>);
        module.def("minimum", minimum<float, 2>);
        module.def("minimum", minimum<double, 2>);
    }

}  // namespace lue::framework
