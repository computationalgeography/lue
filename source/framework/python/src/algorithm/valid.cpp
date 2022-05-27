#include "lue/framework/algorithm/value_policies/valid.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        using BooleanElement = std::uint8_t;


        template<
            typename Element,
            Rank rank>
        PartitionedArray<BooleanElement, rank> valid(
            PartitionedArray<Element, rank> const& array)
        {
            using BooleanElement = std::uint8_t;

            return value_policies::valid<BooleanElement>(array);
        }

    }  // Anonymous namespace


    void bind_valid(
        pybind11::module& module)
    {
        module.def("valid", valid<std::uint8_t, 2>);
        module.def("valid", valid<std::uint32_t, 2>);
        module.def("valid", valid<std::uint64_t, 2>);
        module.def("valid", valid<std::int32_t, 2>);
        module.def("valid", valid<std::int64_t, 2>);
        module.def("valid", valid<float, 2>);
        module.def("valid", valid<double, 2>);
    }

}  // namespace lue::framework
