#include "lue/framework/algorithm/value_policies/atan.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> atan(
            PartitionedArray<Element, rank> const& array)
        {
            return value_policies::atan(array);
        }

    }  // Anonymous namespace


    void bind_atan(
        pybind11::module& module)
    {
        module.def("atan", atan<float, 2>);
        module.def("atan", atan<double, 2>);
    }

}  // namespace lue::framework
