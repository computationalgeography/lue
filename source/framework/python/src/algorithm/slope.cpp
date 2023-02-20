#include "lue/framework/algorithm/value_policies/slope.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> slope(
            PartitionedArray<Element, rank> const& elevation, Element const& cell_size)
        {
            return value_policies::slope(elevation, cell_size);
        }

    }  // Anonymous namespace


    void bind_slope(pybind11::module& module)
    {
        // TODO How to document these?
        module.def("slope", slope<float, 2>, "dem"_a, "cell_size"_a);
        module.def("slope", slope<double, 2>, "dem"_a, "cell_size"_a);
    }

}  // namespace lue::framework
