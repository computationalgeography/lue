#include "lue/framework/algorithm/value_policies/round.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> round(PartitionedArray<Element, rank> const& array)
        {
            return value_policies::round(array);
        }

    }  // Anonymous namespace


    void bind_round(pybind11::module& module)
    {
        module.def("round", round<float, 2>);
        module.def("round", round<double, 2>);
    }

}  // namespace lue::framework
