#include "lue/framework/algorithm/value_policies/maximum.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        auto maximum(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
        {
            return value_policies::maximum(array);
        }

    }  // Anonymous namespace


    void bind_maximum(pybind11::module& module)
    {
        module.def("maximum", maximum<std::uint8_t, 2>);
        module.def("maximum", maximum<std::uint32_t, 2>);
        module.def("maximum", maximum<std::uint64_t, 2>);
        module.def("maximum", maximum<std::int32_t, 2>);
        module.def("maximum", maximum<std::int64_t, 2>);
        module.def("maximum", maximum<float, 2>);
        module.def("maximum", maximum<double, 2>);
    }

}  // namespace lue::framework
