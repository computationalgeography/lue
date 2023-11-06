#include "lue/framework/algorithm/value_policies/log10.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> log10(PartitionedArray<Element, rank> const& array)
        {
            return value_policies::log10(array);
        }

    }  // Anonymous namespace


    void bind_log10(pybind11::module& module)
    {
        module.def("log10", log10<float, 2>);
        module.def("log10", log10<double, 2>);
    }

}  // namespace lue::framework
