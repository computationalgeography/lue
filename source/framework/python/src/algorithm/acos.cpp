#include "lue/framework/algorithm/value_policies/acos.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> acos(PartitionedArray<Element, rank> const& array)
        {
            return value_policies::acos(array);
        }

    }  // Anonymous namespace


    void bind_acos(pybind11::module& module)
    {
        module.def("acos", acos<float, 2>);
        module.def("acos", acos<double, 2>);
    }

}  // namespace lue::framework
