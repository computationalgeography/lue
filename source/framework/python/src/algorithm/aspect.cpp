#include "lue/framework/algorithm/value_policies/aspect.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> aspect(
            PartitionedArray<Element, rank> const& elevation)
        {
            return value_policies::aspect(elevation);
        }

    }  // Anonymous namespace


    void bind_aspect(
        pybind11::module& module)
    {
        // TODO How to document these?
        module.def("aspect", aspect<float, 2>);
        module.def("aspect", aspect<double, 2>);
    }

}  // namespace lue::framework
