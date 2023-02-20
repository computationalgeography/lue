#include "lue/framework/algorithm/value_policies/asin.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> asin(PartitionedArray<Element, rank> const& array)
        {
            return value_policies::asin(array);
        }

    }  // Anonymous namespace


    void bind_asin(pybind11::module& module)
    {
        module.def("asin", asin<float, 2>);
        module.def("asin", asin<double, 2>);
    }

}  // namespace lue::framework
