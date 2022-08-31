#include "lue/framework/algorithm/value_policies/slope.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> slope(
            PartitionedArray<Element, rank> const& elevation,
            Element const& cell_size)
        {
            return value_policies::slope(elevation, cell_size);
        }

    }  // Anonymous namespace


    void bind_slope(
        pybind11::module& module)
    {
        // TODO How to document these?
        module.def("slope", slope<float, 2>);
        module.def("slope", slope<double, 2>);
    }

}  // namespace lue::framework
