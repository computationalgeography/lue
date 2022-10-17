#include "lue/framework/algorithm/value_policies/logical_inclusive_or.hpp"
#include <pybind11/pybind11.h>


#define LUE_BIND_LOGICAL_INCLUSIVE_OR(type, rank) \
    module.def("logical_inclusive_or", logical_inclusive_or<type, rank>);


namespace lue::framework {

    template<
        typename Element,
        Rank rank>
    PartitionedArray<std::uint8_t, rank> logical_inclusive_or(
        PartitionedArray<Element, rank> const& argument1,
        PartitionedArray<Element, rank> const& argument2)
    {
        return value_policies::logical_inclusive_or(argument1, argument2);
    }


    void bind_logical_inclusive_or(
        pybind11::module& module)
    {
        LUE_BIND_LOGICAL_INCLUSIVE_OR(std::uint8_t, 2)
        LUE_BIND_LOGICAL_INCLUSIVE_OR(std::int32_t, 2)
        LUE_BIND_LOGICAL_INCLUSIVE_OR(std::uint32_t, 2)
        LUE_BIND_LOGICAL_INCLUSIVE_OR(std::int64_t, 2)
        LUE_BIND_LOGICAL_INCLUSIVE_OR(std::uint64_t, 2)
    }

}  // namespace lue::framework


#undef LUE_BIND_LOGICAL_INCLUSIVE_OR
