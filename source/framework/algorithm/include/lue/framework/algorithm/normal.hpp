#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/core/component.hpp"
#include <algorithm>
#include <random>


namespace lue {
    namespace detail::normal {

        template<typename Policies, typename InputPartition, typename OutputElement>
        PartitionT<InputPartition, OutputElement> normal_partition(
            Policies const& policies,
            InputPartition const& input_partition,
            OutputElement const mean,
            OutputElement const stddev)
        {
            using Offset = OffsetT<InputPartition>;
            using Shape = ShapeT<InputPartition>;
            using OutputPartition = PartitionT<InputPartition, OutputElement>;
            using OutputData = DataT<OutputPartition>;

            static_assert(std::is_floating_point_v<OutputElement>);

            lue_hpx_assert(input_partition.is_ready());

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                    [policies, input_partition, mean, stddev](Offset const& offset, Shape const& shape)
                    {
                        HPX_UNUSED(input_partition);

                        OutputData output_partition_data{shape};

                        auto const& dp = policies.domain_policy();
                        auto const& indp1 = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                        auto const& indp2 = std::get<1>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                        if (indp1.is_no_data(mean) || indp2.is_no_data(stddev))
                        {
                            Count const nr_elements{lue::nr_elements(output_partition_data)};

                            for (Index i = 0; i < nr_elements; ++i)
                            {
                                ondp.mark_no_data(output_partition_data, i);
                            }
                        }
                        else if (!dp.within_domain(mean, stddev))
                        {
                            Count const nr_elements{lue::nr_elements(output_partition_data)};

                            for (Index i = 0; i < nr_elements; ++i)
                            {
                                ondp.mark_no_data(output_partition_data, i);
                            }
                        }
                        else
                        {
                            // Will be used to obtain a seed for the random number engine
                            std::random_device random_device;

                            // Standard mersenne_twister_engine seeded with the random_device
                            std::mt19937 random_number_engine(random_device());

                            auto distribution = [mean, stddev]() {
                                return std::normal_distribution<OutputElement>{mean, stddev};
                            }();

                            std::generate(
                                output_partition_data.begin(),
                                output_partition_data.end(),

                                [&distribution, &random_number_engine]()
                                { return distribution(random_number_engine); }

                            );
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                    }

                    ),
                input_partition.offset(),
                input_partition.shape());
        }


        template<typename Policies, typename InputPartition, typename OutputElement>
        struct NormalPartitionAction:
            hpx::actions::make_action<
                decltype(&normal_partition<Policies, InputPartition, OutputElement>),
                &normal_partition<Policies, InputPartition, OutputElement>,
                NormalPartitionAction<Policies, InputPartition, OutputElement>>::type
        {
        };

    }  // namespace detail::normal


    template<typename Policies, typename InputElement, typename OutputElement, Rank rank>
    PartitionedArray<OutputElement, rank> normal(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        hpx::shared_future<OutputElement> const& mean,
        hpx::shared_future<OutputElement> const& stddev)
    {
        // Reimplement as ternary local operation?

        // The result array will have the same shape, partitioning and
        // location as the input array, but the elements might be of a different
        // type.

        using InputArray = PartitionedArray<InputElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;
        using InputPartition = PartitionT<InputArray>;

        using OutputArray = PartitionedArray<OutputElement, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;

        static_assert(std::is_floating_point_v<OutputElement>);

        lue_hpx_assert(all_are_valid(input_array.partitions()));
        lue_hpx_assert(mean.valid());
        lue_hpx_assert(stddev.valid());

        detail::normal::NormalPartitionAction<Policies, InputPartition, OutputElement> action;

        Localities<rank> const& localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index p = 0; p < nr_partitions(input_array); ++p)
        {
            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[p], action, policies](
                    InputPartition const& input_partition,
                    hpx::shared_future<OutputElement> const& mean,
                    hpx::shared_future<OutputElement> const& stddev)
                { return action(locality_id, policies, input_partition, mean.get(), stddev.get()); },

                input_partitions[p],
                mean,
                stddev);
        }

        return OutputArray{shape(input_array), localities, std::move(output_partitions)};
    }


    template<typename Policies, typename InputElement, typename OutputElement, Rank rank>
    PartitionedArray<OutputElement, rank> normal(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        OutputElement const mean,
        OutputElement const stddev)
    {
        return normal(
            policies,
            input_array,
            hpx::make_ready_future<OutputElement>(mean).share(),
            hpx::make_ready_future<OutputElement>(stddev).share());
    }


    // -------------------------------------------------------------------------


    namespace detail {

        template<typename Policies, typename Element, Rank rank>
        ArrayPartition<Element, rank> normal_partition(
            [[maybe_unused]] Policies const& policies,
            OffsetT<ArrayPartition<Element, rank>> const& offset,
            ShapeT<ArrayPartition<Element, rank>> const& partition_shape,
            Element const mean,
            Element const stddev)
        {
            using Partition = ArrayPartition<Element, rank>;
            using PartitionData = DataT<Partition>;

            PartitionData partition_data{partition_shape};

            // Will be used to obtain a seed for the random number engine
            std::random_device random_device;

            // Standard mersenne_twister_engine seeded with the random_device
            std::mt19937 random_number_engine(random_device());

            static_assert(std::is_floating_point_v<Element>);

            auto distribution = std::normal_distribution<Element>{mean, stddev};

            std::generate(
                partition_data.begin(),
                partition_data.end(),

                [&distribution, &random_number_engine]() { return distribution(random_number_engine); }

            );

            return Partition{hpx::find_here(), offset, std::move(partition_data)};
        }


        template<typename Policies, typename Element, Rank rank>
        struct UniformPartitionAction2:
            hpx::actions::make_action<
                decltype(&normal_partition<Policies, Element, rank>),
                &normal_partition<Policies, Element, rank>,
                UniformPartitionAction2<Policies, Element, rank>>::type
        {
        };

    }  // namespace detail


    template<typename Element, Rank rank>
    class InstantiateNormal
    {

        public:

            using OutputElement = Element;
            using Partition = lue::ArrayPartition<OutputElement, rank>;
            using Offset = lue::OffsetT<Partition>;
            using Shape = lue::ShapeT<Partition>;


            static constexpr bool instantiate_per_locality{false};


            InstantiateNormal(Element const mean, Element const stddev):

                _mean{mean},
                _stddev{stddev}

            {
            }


            template<typename Policies>
            Partition instantiate(
                hpx::id_type const locality_id,
                [[maybe_unused]] Policies const& policies,
                [[maybe_unused]] Shape const& array_shape,
                [[maybe_unused]] lue::Index const partition_idx,
                Offset const& offset,
                Shape const& partition_shape)
            {
                using Action = detail::UniformPartitionAction2<Policies, Element, rank>;

                return hpx::async(Action{}, locality_id, policies, offset, partition_shape, _mean, _stddev);
            }


        private:

            Element const _mean;

            Element const _stddev;
    };


    template<typename Element, Rank rank>
    class FunctorTraits<InstantiateNormal<Element, rank>>
    {

        public:

            static constexpr bool const is_functor{true};
    };


    template<typename Element, typename Policies, typename Shape>
    PartitionedArray<Element, rank<Shape>> normal(
        Policies const& policies,
        Shape const& array_shape,
        Shape const& partition_shape,
        Element const mean,
        Element const stddev)
    {
        using Functor = InstantiateNormal<Element, rank<Shape>>;

        return create_partitioned_array(policies, array_shape, partition_shape, Functor{mean, stddev});
    }


    template<typename Element, typename Policies, typename Shape>
    PartitionedArray<Element, rank<Shape>> normal(
        Policies const& policies, Shape const& array_shape, Element const mean, Element const stddev)
    {
        using Functor = InstantiateNormal<Element, rank<Shape>>;

        return create_partitioned_array(policies, array_shape, Functor{mean, stddev});
    }

}  // namespace lue
