#include "lue/framework/algorithm/value_policies/sin.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> sin(PartitionedArray<Element, rank> const& array)
        {
            return value_policies::sin(array);
        }

    }  // Anonymous namespace


    void bind_sin(pybind11::module& module)
    {
        module.def("sin", sin<float, 2>);
        module.def("sin", sin<double, 2>);
    }

}  // namespace lue::framework
