#include "lue/framework/algorithm/value_policies/maximum.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        hpx::shared_future<Element> maximum(
            PartitionedArray<Element, rank> const& array)
        {
            return value_policies::maximum(array).share();
        }

    }  // Anonymous namespace


    void bind_maximum(
        pybind11::module& module)
    {
        // TODO How to document these?
        module.def("maximum", maximum<std::uint8_t, 2>);
        module.def("maximum", maximum<std::uint32_t, 2>);
        module.def("maximum", maximum<std::uint64_t, 2>);
        module.def("maximum", maximum<std::int32_t, 2>);
        module.def("maximum", maximum<std::int64_t, 2>);
        module.def("maximum", maximum<float, 2>);
        module.def("maximum", maximum<double, 2>);
    }

}  // namespace lue::framework
