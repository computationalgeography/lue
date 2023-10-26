#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
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


    // TODO This function is not perse highest_n, but first_n. The route is given, based on
    // decreasing values, but that is invisible here, when we only have the route. This function
    // can be used in cases in which the route is created using some other rule as well (increasing
    // values, ...).
    template<typename OutputElement, typename Policies, Rank rank>
    PartitionedArray<OutputElement, rank> highest_n(
        Policies const& policies, SerialRoute<rank> const& route, Count const nr_cells)
    {
        using CreatePartitionFunctor = InstantiateFilled<OutputElement, rank>;

        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();
        OutputElement no_data_value;
        ondp.mark_no_data(no_data_value);

        PartitionedArray<OutputElement, rank> result{
            create_partitioned_array(route, CreatePartitionFunctor{no_data_value})};

        using Functor = detail::HighestN<OutputElement>;

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
        using DecreasingOrderPolicies = policy::Policies<
            policy::DomainPolicyT<Policies>,
            policy::OutputElements<>,
            policy::InputsPolicies<policy::InputPoliciesT<Policies, 0>>>;

        return highest_n<OutputElement>(
            policies, decreasing_order(DecreasingOrderPolicies{}, region, array, nr_cells), nr_cells);
    }


    template<typename OutputElement, typename Policies, typename InputElement, Rank rank>
    PartitionedArray<OutputElement, rank> highest_n(
        Policies const& policies, PartitionedArray<InputElement, rank> const& array, Count const nr_cells)
    {
        using DecreasingOrderPolicies = policy::Policies<
            policy::DomainPolicyT<Policies>,
            policy::OutputElements<>,
            policy::InputsPolicies<policy::InputPoliciesT<Policies, 0>>>;

        return highest_n<OutputElement>(policies, decreasing_order(policies, array, nr_cells), nr_cells);
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
