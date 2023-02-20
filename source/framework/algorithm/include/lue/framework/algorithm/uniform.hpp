#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include <algorithm>
#include <random>


namespace lue {
    namespace detail {
        namespace uniform {

            template<typename Policies, typename InputPartition, typename OutputElement>
            PartitionT<InputPartition, OutputElement> uniform_partition(
                Policies const& policies,
                InputPartition const& input_partition,
                OutputElement const min_value,
                OutputElement const max_value)
            {
                using Offset = OffsetT<InputPartition>;
                using Shape = ShapeT<InputPartition>;
                using OutputPartition = PartitionT<InputPartition, OutputElement>;
                using OutputData = DataT<OutputPartition>;

                static_assert(std::is_floating_point_v<OutputElement> || std::is_integral_v<OutputElement>);

                lue_hpx_assert(input_partition.is_ready());

                return hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(

                        [policies, input_partition, min_value, max_value](
                            Offset const& offset, Shape const& shape)
                        {
                            AnnotateFunction annotation{"uniform_partition"};

                            HPX_UNUSED(input_partition);

                            OutputData output_partition_data{shape};

                            auto const& indp1 =
                                std::get<0>(policies.inputs_policies()).input_no_data_policy();
                            auto const& indp2 =
                                std::get<1>(policies.inputs_policies()).input_no_data_policy();
                            auto const& ondp =
                                std::get<0>(policies.outputs_policies()).output_no_data_policy();

                            if (indp1.is_no_data(min_value) || indp2.is_no_data(max_value))
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

                                auto distribution = [min_value, max_value]()
                                {
                                    if constexpr (std::is_floating_point_v<OutputElement>)
                                    {
                                        return std::uniform_real_distribution<OutputElement>{
                                            min_value, max_value};
                                    }
                                    else if constexpr (std::is_integral_v<OutputElement>)
                                    {
                                        // https://stackoverflow.com/questions/31460733/why-arent-stduniform-int-distributionuint8-t-and-stduniform-int-distri
                                        static_assert(!std::is_same_v<OutputElement, std::int8_t>);
                                        static_assert(!std::is_same_v<OutputElement, std::uint8_t>);
                                        return std::uniform_int_distribution<OutputElement>{
                                            min_value, max_value};
                                    }
                                }();

                                std::generate(
                                    output_partition_data.begin(),
                                    output_partition_data.end(),

                                    [&distribution, &random_number_engine]()
                                    { return distribution(random_number_engine); }

                                );
                            }

                            return OutputPartition{
                                hpx::find_here(), offset, std::move(output_partition_data)};
                        }

                        ),
                    input_partition.offset(),
                    input_partition.shape());
            }


            template<typename Policies, typename InputPartition, typename OutputElement>
            struct UniformPartitionAction:
                hpx::actions::make_action<
                    decltype(&uniform_partition<Policies, InputPartition, OutputElement>),
                    &uniform_partition<Policies, InputPartition, OutputElement>,
                    UniformPartitionAction<Policies, InputPartition, OutputElement>>::type
            {
            };

        }  // namespace uniform
    }      // namespace detail


    /// template<
    ///     typename Policies,
    ///     typename T>
    /// using UniformAction = typename detail::uniform::OverloadPicker<Policies, T>::Action;


    template<typename Policies, typename InputElement, typename OutputElement, Rank rank>
    PartitionedArray<OutputElement, rank> uniform(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        hpx::shared_future<OutputElement> const& min_value,
        hpx::shared_future<OutputElement> const& max_value)
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

        static_assert(std::is_floating_point_v<OutputElement> || std::is_integral_v<OutputElement>);

        lue_hpx_assert(all_are_valid(input_array.partitions()));
        lue_hpx_assert(min_value.valid());
        lue_hpx_assert(max_value.valid());

        detail::uniform::UniformPartitionAction<Policies, InputPartition, OutputElement> action;

        Localities<rank> const& localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index p = 0; p < nr_partitions(input_array); ++p)
        {
            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[p], action, policies](
                    InputPartition const& input_partition,
                    hpx::shared_future<OutputElement> const& min_value,
                    hpx::shared_future<OutputElement> const& max_value)
                {
                    AnnotateFunction annotation{"uniform"};
                    return action(locality_id, policies, input_partition, min_value.get(), max_value.get());
                },

                input_partitions[p],
                min_value,
                max_value);
        }

        return OutputArray{shape(input_array), localities, std::move(output_partitions)};
    }


    template<typename Policies, typename InputElement, typename OutputElement, Rank rank>
    PartitionedArray<OutputElement, rank> uniform(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        OutputElement const min_value,
        OutputElement const max_value)
    {
        return uniform(
            policies,
            input_array,
            hpx::make_ready_future<OutputElement>(min_value).share(),
            hpx::make_ready_future<OutputElement>(max_value).share());
    }


    // -------------------------------------------------------------------------


    namespace detail {

        template<typename Policies, typename Element, Rank rank>
        ArrayPartition<Element, rank> uniform_partition(
            [[maybe_unused]] Policies const& policies,
            OffsetT<ArrayPartition<Element, rank>> const& offset,
            ShapeT<ArrayPartition<Element, rank>> const& partition_shape,
            Element const min_value,
            Element const max_value)
        {
            using Partition = ArrayPartition<Element, rank>;
            using PartitionData = DataT<Partition>;

            PartitionData partition_data{partition_shape};

            // Will be used to obtain a seed for the random number engine
            std::random_device random_device;

            // Standard mersenne_twister_engine seeded with the random_device
            std::mt19937 random_number_engine(random_device());

            auto distribution = [min_value, max_value]()
            {
                if constexpr (std::is_floating_point_v<Element>)
                {
                    // [min, max)
                    return std::uniform_real_distribution<Element>{min_value, max_value};
                }
                else if constexpr (std::is_integral_v<Element>)
                {
                    // [min, max]
                    return std::uniform_int_distribution<Element>{min_value, max_value};
                }
            }();

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
                decltype(&uniform_partition<Policies, Element, rank>),
                &uniform_partition<Policies, Element, rank>,
                UniformPartitionAction2<Policies, Element, rank>>::type
        {
        };

    }  // namespace detail


    template<typename Element, Rank rank>
    class InstantiateUniform
    {

        public:

            using OutputElement = Element;
            using Partition = lue::ArrayPartition<OutputElement, rank>;
            using Offset = lue::OffsetT<Partition>;
            using Shape = lue::ShapeT<Partition>;


            static constexpr bool instantiate_per_locality{false};


            InstantiateUniform(Element const min_value, Element const max_value):

                _min_value{min_value},
                _max_value{max_value}

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

                return hpx::async(
                    Action{}, locality_id, policies, offset, partition_shape, _min_value, _max_value);
            }


        private:

            Element const _min_value;

            Element const _max_value;
    };


    template<typename Element, Rank rank>
    class FunctorTraits<InstantiateUniform<Element, rank>>
    {

        public:

            static constexpr bool const is_functor{true};
    };


    template<typename Element, typename Policies, typename Shape>
    PartitionedArray<Element, rank<Shape>> uniform(
        Policies const& policies,
        Shape const& array_shape,
        Shape const& partition_shape,
        Element const min_value,
        Element const max_value)
    {
        using Functor = InstantiateUniform<Element, rank<Shape>>;

        return create_partitioned_array(
            policies, array_shape, partition_shape, Functor{min_value, max_value});
    }

}  // namespace lue
