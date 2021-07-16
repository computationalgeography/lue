#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> uniform(
            PartitionedArray<Element, rank> const& array,
            Element const min,
            Element const max)
        {
            return value_policies::uniform(array, min, max);
        }

    }  // Anonymous namespace


    void bind_uniform(
        pybind11::module& module)
    {
        // TODO How to document these?
        module.def("uniform", uniform<std::uint8_t, 2>);
        module.def("uniform", uniform<std::uint32_t, 2>);
        module.def("uniform", uniform<std::uint64_t, 2>);
        module.def("uniform", uniform<std::int32_t, 2>);
        module.def("uniform", uniform<std::int64_t, 2>);
        module.def("uniform", uniform<float, 2>);
        module.def("uniform", uniform<double, 2>);
    }

}  // namespace lue::framework
