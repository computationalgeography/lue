#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/decreasing_order.hpp"
#include "lue/framework/algorithm/detail/promise.hpp"
#include "lue/framework/algorithm/highest_n.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/framework/algorithm/walk.hpp"
#include "lue/macro.hpp"


namespace lue {

    /*!
        @brief      Functor for marking cells as true
    */
    template<typename Policies, Rank rank>
    class MarkAsTrue
    {

        public:

            //! Linear index
            using OutputElement = policy::OutputElementT<Policies>;


            template<typename PartitionedArray>
            MarkAsTrue(Policies const& policies, PartitionedArray const& clone, Count const max_nr_cells):

                _max_nr_cells{max_nr_cells},
                _tmp_result{},
                _promises{clone.partitions().shape()}

            {
                auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();
                OutputElement no_data_value;

                ondp.mark_no_data(no_data_value);

                _tmp_result =
                    create_partitioned_array(clone, InstantiateFilled<OutputElement, rank>{no_data_value});

                // TODO Wait for it to finish creating partitions?
            }


            void visit_cells(Index const partition_idx, std::vector<Index> const& cell_idxs)
            {
                // partition_idx: Linear idx of partition to visit. We can assume this partition
                //     is in our current locality.
                // cell_idxs: Linear idxs of cells to visit. Do whatever we need to do. In our case:
                //     assign a True value to the cells in the result partition.


                // TODO
            }


            PartitionedArray<OutputElement, rank> result()
            {
                using PartitionedArray = PartitionedArray<OutputElement, rank>;
                using Partitions = PartitionsT<PartitionedArray>;
                using Partition = PartitionT<PartitionedArray>;

                Count const nr_partitions{nr_elements(_promises.shape())};

                Partitions partitions{_promises.shape()};

                for (Index p = 0; p < nr_partitions; ++p)
                {
                    // Connect the array with promises with the array of futures
                    partitions[p] = Partition{_promises[p].get_future()};
                }

                return PartitionedArray{_tmp_result.shape(), _tmp_result.localities(), std::move(partitions)};
            }


            void finish_partition(Index const idx)
            {
                // This function must be called once, for all partitions
                lue_hpx_assert(idx < nr_elements(_promises.shape()));
                lue_hpx_assert(_tmp_result.partitions()[idx].valid());
                lue_hpx_assert(_tmp_result.partitions()[idx].is_ready());

                _promises[idx].set_value(_tmp_result.partitions()[idx].get_id());
            }


        private:

            //! Maximum number of cells to mark
            Count _max_nr_cells;

            //! Array that eventually will contain the result
            PartitionedArray<OutputElement, rank> _tmp_result;

            //! For each result partition a promise for the ID
            detail::IDPromiseArray<rank> _promises;
    };


    template<typename Policies, Rank rank>
    PartitionedArray<policy::OutputElementT<Policies>, rank> highest_n(
        [[maybe_unused]] /* TODO */ Policies const& policies,
        SerialRoute<rank> const& route,
        Count const nr_cells)
    {
        using InputElement = policy::InputElementT<Policies>;
        using OutputElement = policy::OutputElementT<Policies>;

        using WalkPolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<InputElement>,
            policy::OutputElements<OutputElement>,
            policy::InputElements<InputElement>>;

        auto functor = MarkAsTrue<WalkPolicies, rank>{WalkPolicies{}, route, nr_cells};
        auto result = functor.result();

        walk(route, std::move(functor));

        return result;
    }


    template<typename Policies, typename ZoneElement, typename InputElement, Rank rank>
    PartitionedArray<policy::OutputElementT<Policies>, rank> highest_n(
        [[maybe_unused]] /* TODO */ Policies const& policies,
        PartitionedArray<ZoneElement, rank> const& region,
        PartitionedArray<InputElement, rank> const& array,
        Count const nr_cells)
    {
        // TODO Make this work
        // using DecreasingOrderPolicies = policy::Policies<
        //     policy::AllValuesWithinDomain<ZoneElement, InputElement>,
        //     policy::OutputsPolicies<>,
        //     policy::InputsPolicies<policy::InputPoliciesT<Policies, 0>, policy::InputPoliciesT<Policies,
        //     1>>>;

        using DecreasingOrderPolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<ZoneElement, InputElement>,
            policy::OutputElements<>,
            policy::InputElements<ZoneElement, InputElement>>;

        // TODO Make this depend on the policy and its state passed in
        using OutputElement = policy::OutputElementT<Policies>;

        using WalkPolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<InputElement>,
            policy::OutputElements<OutputElement>,
            policy::InputElements<InputElement>>;

        return highest_n(
            WalkPolicies{}, decreasing_order(DecreasingOrderPolicies{}, region, array, nr_cells), nr_cells);
    }


    template<typename Policies, typename InputElement, Rank rank>
    PartitionedArray<policy::OutputElementT<Policies>, rank> highest_n(
        [[maybe_unused]] /* TODO */ Policies const& policies,
        PartitionedArray<InputElement, rank> const& array,
        Count const nr_cells)
    {
        // TODO Make this work
        // using DecreasingOrderPolicies = policy::Policies<
        //     policy::AllValuesWithinDomain<InputElement>,
        //     policy::OutputsPolicies<>,
        //     policy::InputsPolicies<policy::InputPoliciesT<Policies, 0, InputElement>>>;

        using DecreasingOrderPolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<InputElement>,
            policy::OutputElements<>,
            policy::InputElements<InputElement>>;

        // TODO Make this depend on the policy and its state passed in
        using OutputElement = policy::OutputElementT<Policies>;

        using WalkPolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<InputElement>,
            policy::OutputElements<OutputElement>,
            policy::InputElements<InputElement>>;

        return highest_n(
            WalkPolicies{}, decreasing_order(DecreasingOrderPolicies{}, array, nr_cells), nr_cells);
    }

}  // namespace lue


#define LUE_INSTANTIATE_HIGHEST_N(Policies, InputElement)                                                    \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies>, 2>              \
    highest_n<ArgumentType<void(Policies)>, InputElement, 2>(                                                \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<InputElement, 2> const&,                                                            \
        Count const nr_cells);

#define LUE_INSTANTIATE_HIGHEST_N_ZONE(Policies, ZoneElement, InputElement)                                  \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies>, 2>              \
    highest_n<ArgumentType<void(Policies)>, ZoneElement, InputElement, 2>(                                   \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ZoneElement, 2> const&,                                                             \
        PartitionedArray<InputElement, 2> const&,                                                            \
        Count const nr_cells);
