#include "lue/framework/algorithm/value_policies/tan.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> tan(
            PartitionedArray<Element, rank> const& array)
        {
            return value_policies::tan(array);
        }

    }  // Anonymous namespace


    void bind_tan(
        pybind11::module& module)
    {
        module.def("tan", tan<float, 2>);
        module.def("tan", tan<double, 2>);
    }

}  // namespace lue::framework
