#include "lue/framework/algorithm/value_policies/cell_index.hpp"
#include "lue/framework.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_cell_index(pybind11::module& module)
    {
        module.def(
            "cell_index",
            [](PartitionedArray<BooleanElement, 2> const& condition, Index const dimension_idx)
            { value_policies::cell_index<IndexElement>(condition, dimension_idx); });
    }

}  // namespace lue::framework
