#include "lue/framework/algorithm/value_policies/sqrt.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> sqrt(
            PartitionedArray<Element, rank> const& array)
        {
            return value_policies::sqrt(array);
        }

    }  // Anonymous namespace


    void bind_sqrt(
        pybind11::module& module)
    {
        // TODO How to document these?
        module.def("sqrt", sqrt<float, 2>);
        module.def("sqrt", sqrt<double, 2>);
    }

}  // namespace lue::framework
