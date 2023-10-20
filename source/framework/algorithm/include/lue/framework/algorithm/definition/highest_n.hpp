#pragma once
#include "lue/framework/algorithm/clone_array.hpp"
#include "lue/framework/algorithm/decreasing_order.hpp"
#include "lue/framework/algorithm/highest_n.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/framework/algorithm/walk.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<typename OutputElement>
        class HighestN
        {

            public:

                HighestN(Count const nr_cells):

                    _nr_cells{nr_cells}

                {
                }

            private:

                Count _nr_cells;
        };

    }  // namespace detail


    template<typename OutputElement, typename Policies, Rank rank>
    PartitionedArray<OutputElement, rank> highest_n(
        Policies const& policies, SerialRoute<rank> const& route, Count const nr_cells)
    {
        using Functor = detail::HighestN<OutputElement>;

        PartitionedArray<OutputElement, rank> result{clone_array<OutputElement>(route)};

        walk(policies, route, Functor{nr_cells}, result);

        return result;
    }


    template<
        typename OutputElement,
        typename Policies,
        typename ZoneElement,
        typename InputElement,
        Rank rank>
    PartitionedArray<OutputElement, rank> highest_n(
        Policies const& policies,
        PartitionedArray<ZoneElement, rank> const& region,
        PartitionedArray<InputElement, rank> const& array,
        Count const nr_cells)
    {
        return highest_n<OutputElement>(policies, decreasing_order(region, array, nr_cells), nr_cells);
    }


    template<typename OutputElement, typename Policies, typename InputElement, Rank rank>
    PartitionedArray<OutputElement, rank> highest_n(
        Policies const& policies, PartitionedArray<InputElement, rank> const& array, Count const nr_cells)
    {
        return highest_n<OutputElement>(policies, decreasing_order(array, nr_cells), nr_cells);
    }

}  // namespace lue


#define LUE_INSTANTIATE_HIGHEST_N(Policies, OutputElement, InputElement)                                     \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<OutputElement, 2>                                 \
    highest_n<OutputElement, ArgumentType<void(Policies)>, InputElement, 2>(                                 \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<InputElement, 2> const&,                                                            \
        Count const nr_cells);

#define LUE_INSTANTIATE_HIGHEST_N_ZONE(Policies, OutputElement, ZoneElement, InputElement)                   \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<OutputElement, 2>                                 \
    highest_n<OutputElement, ArgumentType<void(Policies)>, ZoneElement, InputElement, 2>(                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ZoneElement, 2> const&,                                                             \
        PartitionedArray<InputElement, 2> const&,                                                            \
        Count const nr_cells);
