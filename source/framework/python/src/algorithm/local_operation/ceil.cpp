#include "lue/framework/algorithm/value_policies/ceil.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> ceil(PartitionedArray<Element, rank> const& array)
        {
            return value_policies::ceil(array);
        }

    }  // Anonymous namespace


    void bind_ceil(pybind11::module& module)
    {
        module.def("ceil", ceil<float, 2>);
        module.def("ceil", ceil<double, 2>);
    }

}  // namespace lue::framework
