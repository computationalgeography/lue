#include "lue/framework/algorithm/value_policies/log.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> log(PartitionedArray<Element, rank> const& array)
        {
            return value_policies::log(array);
        }

    }  // Anonymous namespace


    void bind_log(pybind11::module& module)
    {
        module.def("log", log<float, 2>);
        module.def("log", log<double, 2>);
    }

}  // namespace lue::framework
