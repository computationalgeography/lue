#include "lue/framework/algorithm/value_policies/atan2.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> atan2(
            PartitionedArray<Element, rank> const& array1,
            PartitionedArray<Element, rank> const& array2)
        {
            return value_policies::atan2(array1, array2);
        }

    }  // Anonymous namespace


    void bind_atan2(
        pybind11::module& module)
    {
        module.def("atan2", atan2<float, 2>);
        module.def("atan2", atan2<double, 2>);
    }

}  // namespace lue::framework
