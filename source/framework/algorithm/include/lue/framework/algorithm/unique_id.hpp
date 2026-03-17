#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    /*!
        @brief      Return a new array with for each true element in @a array a unique positive ID
        @tparam     IDElement An integral type
        @tparam     ConditionElement An integral type

        Elements in @a array that contain a value which evaluates to false will be assigned a
        zero in the returned array.

        Only in case all elements in @a array contain a valid value according to the @a policies
        passed in, and they contain a value which evaluates to true, will the sequence of IDs
        in the array returned be a continuous range. In all other cases they will not be (IDs
        will be missing from the range).

        The IDs will be assigned to elements in the resulting array in a non-deterministic
        way. Calling this function multiple times will likely result in different distributions
        of IDs over the array. This depends on the order in which work gets scheduled, which
        depends on too many things to describe here.
    */
    template<typename IDElement, typename Policies, typename ConditionElement, Rank rank>
    auto unique_id(Policies const& policies, PartitionedArray<ConditionElement, rank> const& array)
        -> PartitionedArray<IDElement, rank>;

}  // namespace lue
