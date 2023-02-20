#include "lue/framework/algorithm/iterate_per_element.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};

        template<typename Element>
        PartitionedArray<Element, rank> iterate_per_element(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::iterate_per_element::DefaultPolicies<Element>;

            return iterate_per_element(Policies{}, array);
        }

    }  // Anonymous namespace


    void bind_iterate_per_element(pybind11::module& module)
    {
        module.def("iterate_per_element", iterate_per_element<std::uint8_t>);
        module.def("iterate_per_element", iterate_per_element<std::uint32_t>);
        module.def("iterate_per_element", iterate_per_element<std::uint64_t>);
        module.def("iterate_per_element", iterate_per_element<std::int32_t>);
        module.def("iterate_per_element", iterate_per_element<std::int64_t>);
        module.def("iterate_per_element", iterate_per_element<float>);
        module.def("iterate_per_element", iterate_per_element<double>);
    }

}  // namespace lue::framework
