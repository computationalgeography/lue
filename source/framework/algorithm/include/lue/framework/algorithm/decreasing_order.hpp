#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/framework/serial_route.hpp"


namespace lue {

    /*!
        @brief      Determine the route along the cells in an array, visiting them in decreasing
                    order, from highest to lowest @a value
        @param      max_nr_cells Maximum length of the route to determine. In case zero is
                    passed in, a single empty route will be created.

        In case there are no valid values passed in, then no route is created.
    */
    template<typename Policies, Rank rank>
    auto decreasing_order(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& value,
        Count max_nr_cells = std::numeric_limits<Count>::max())
        -> SerialRoute<policy::OutputElementT<Policies, 0>, rank>;


    /*!
        @brief      Per @a zone, determine the route along the cells in an array, visiting them
                    in decreasing order, from highest to lowest @a value
        @param      max_nr_cells Maximum length of the route to determine. In case zero is
                    passed in, for each zone an empty route will be created.

        In case for a zone no valid values passed in, then no route is created for that zone.
    */
    template<typename Policies, Rank rank>
    auto decreasing_order(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& zone,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& value,
        Count max_nr_cells = std::numeric_limits<Count>::max())
        -> SerialRoute<policy::OutputElementT<Policies, 0>, rank>;

}  // namespace lue
