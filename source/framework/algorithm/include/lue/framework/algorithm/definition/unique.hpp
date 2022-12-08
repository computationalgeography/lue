#pragma once
#include "lue/framework/algorithm/global_operation_export.hpp"
#include "lue/framework/algorithm/unique.hpp"
#include "lue/macro.hpp"
#include <set>


namespace lue {
    namespace detail {

        template<
            typename Policies,
            typename Partition>
        hpx::future<std::set<ElementT<Partition>>> unique_partition_ready(
            Policies const& policies,
            Partition const& partition)
        {
            using Data = DataT<Partition>;
            using Element = ElementT<Partition>;

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                        [partition, policies](
                            Data const& input_partition_data)
                        {
                            HPX_UNUSED(partition);

                            // Copy values from input array into a set
                            std::set<Element> unique_values;

                            auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();

                            Count const nr_elements{lue::nr_elements(input_partition_data)};

                            for(Index i = 0; i < nr_elements; ++i)
                            {
                                if(!indp.is_no_data(input_partition_data, i))
                                {
                                    unique_values.insert(input_partition_data[i]);
                                }
                            }

                            Count const nr_unique_values = static_cast<Count>(unique_values.size());
                            lue_hpx_assert(nr_unique_values <= input_partition_data.nr_elements());

                            return unique_values;
                        }

                    ),
                    partition.data()
                );
        }


        template<
            typename Policies,
            typename Partition>
        hpx::future<std::set<ElementT<Partition>>> unique_partition(
            Policies const& policies,
            Partition const& partition)
        {
            return hpx::dataflow(
                    hpx::launch::async,

                    [policies](Partition const& partition)
                    {
                        return unique_partition_ready(policies, partition);
                    },

                    partition
                );
        }


        template<
            typename Policies,
            typename Partition>
        struct UniquePartitionAction:
            hpx::actions::make_action<
                decltype(&detail::unique_partition<Policies, Partition>),
                &unique_partition<Policies, Partition>,
                UniquePartitionAction<Policies, Partition>>::type
        {};

    }  // namespace detail


    template<
        typename Policies,
        typename Element,
        Rank rank>
    hpx::future<std::set<Element>> unique(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array)
    {
        static_assert(std::is_integral_v<Element>);

        using Array = PartitionedArray<Element, rank>;
        using Partitions = PartitionsT<Array>;
        using Partition = PartitionT<Array>;

        detail::UniquePartitionAction<Policies, Partition> action;

        Localities<rank> const& localities{array.localities()};
        Partitions const& input_partitions{array.partitions()};
        std::vector<hpx::future<std::set<Element>>> output_partitions{
            static_cast<std::size_t>(nr_partitions(array))};

        for(Index p = 0; p < nr_partitions(array); ++p)
        {
            output_partitions[p] = hpx::async(action, localities[p], policies, input_partitions[p]);
        }

        return hpx::when_all(output_partitions.begin(), output_partitions.end()).then(
                hpx::unwrapping(

                        [](auto&& partitions)
                        {
                            std::set<Element> unique_values{};

                            for(auto& unique_values_per_partition_f: partitions)
                            {
                                auto const& unique_values_per_partition{unique_values_per_partition_f.get()};

                                unique_values.insert(
                                    unique_values_per_partition.begin(), unique_values_per_partition.end());
                            }

                            return unique_values;
                        }

                    )
            );
    }

}  // namespace lue


#define LUE_INSTANTIATE_UNIQUE(                         \
    Policies, Element)                                  \
                                                        \
    template LUE_GLOBAL_OPERATION_EXPORT                \
    hpx::future<std::set<Element>> unique<              \
            ArgumentType<void(Policies)>, Element, 2>(  \
        ArgumentType<void(Policies)> const&,            \
        PartitionedArray<Element, 2> const&);
