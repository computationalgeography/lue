#include "lue/framework/algorithm/value_policies/cos.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> cos(
            PartitionedArray<Element, rank> const& array)
        {
            return value_policies::cos(array);
        }

    }  // Anonymous namespace


    void bind_cos(
        pybind11::module& module)
    {
        module.def("cos", cos<float, 2>);
        module.def("cos", cos<double, 2>);
    }

}  // namespace lue::framework
