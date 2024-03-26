#include "lue/framework/algorithm/value_policies/cell_index.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        using ConditionElement = std::uint8_t;
        using IndexElement = std::uint64_t;

        template<Rank rank>
        auto cell_index(PartitionedArray<ConditionElement, rank> const& condition, Index const dimension_idx)
            -> PartitionedArray<IndexElement, rank>
        {
            return value_policies::cell_index<IndexElement>(condition, dimension_idx);
        }

    }  // Anonymous namespace


    void bind_cell_index(pybind11::module& module)
    {
        module.def("cell_index", cell_index<2>);
    }

}  // namespace lue::framework
