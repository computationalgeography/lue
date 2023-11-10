#include "lue/framework/algorithm/value_policies/floor.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> floor(PartitionedArray<Element, rank> const& array)
        {
            return value_policies::floor(array);
        }

    }  // Anonymous namespace


    void bind_floor(pybind11::module& module)
    {
        module.def("floor", floor<float, 2>);
        module.def("floor", floor<double, 2>);
    }

}  // namespace lue::framework
