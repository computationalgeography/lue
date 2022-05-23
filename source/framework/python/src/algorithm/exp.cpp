#include "lue/framework/algorithm/value_policies/exp.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> exp(
            PartitionedArray<Element, rank> const& array)
        {
            return value_policies::exp(array);
        }

    }  // Anonymous namespace


    void bind_exp(
        pybind11::module& module)
    {
        module.def("exp", exp<float, 2>);
        module.def("exp", exp<double, 2>);
    }

}  // namespace lue::framework
