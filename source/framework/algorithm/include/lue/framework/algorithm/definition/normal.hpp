#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/miscellaneous_operation_export.hpp"
#include "lue/framework/algorithm/normal.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"
#include <algorithm>
#include <random>


namespace lue {

    // -------------------------------------------------------------------------
    // normal(policies, array, mean, stddev)
    // -------------------------------------------------------------------------

    namespace detail::normal {

        template<typename Policies, typename InputPartition>
        auto normal_partition(
            Policies const& policies,
            InputPartition const& input_partition,
            policy::OutputElementT<Policies, 0> const mean,
            policy::OutputElementT<Policies, 0> const stddev)
            -> PartitionT<InputPartition, policy::OutputElementT<Policies, 0>>
        {
            using Offset = OffsetT<InputPartition>;
            using Shape = ShapeT<InputPartition>;
            using Element = policy::OutputElementT<Policies, 0>;
            using OutputPartition = PartitionT<InputPartition, Element>;
            using OutputData = DataT<OutputPartition>;

            lue_hpx_assert(input_partition.is_ready());

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                    [policies, input_partition, mean, stddev](Offset const& offset, Shape const& shape)
                    {
                        AnnotateFunction annotation{"normal: partition"};

                        HPX_UNUSED(input_partition);

                        OutputData output_partition_data{shape};

                        auto const& dp = policies.domain_policy();
                        // TODO Use indp1 to test for no-data-ness in input partition's data(?)
                        auto const& indp2 = std::get<1>(policies.inputs_policies()).input_no_data_policy();
                        auto const& indp3 = std::get<2>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                        if (indp2.is_no_data(mean) || indp3.is_no_data(stddev))
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

                            auto distribution = [mean, stddev]()
                            { return std::normal_distribution<Element>{mean, stddev}; }();

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


        template<typename Policies, typename InputPartition>
        struct NormalPartitionAction:
            hpx::actions::make_action<
                decltype(&normal_partition<Policies, InputPartition>),
                &normal_partition<Policies, InputPartition>,
                NormalPartitionAction<Policies, InputPartition>>::type
        {
        };

    }  // namespace detail::normal


    template<typename Policies, Rank rank>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array,
        Scalar<policy::InputElementT<Policies, 1>> const& mean,
        Scalar<policy::InputElementT<Policies, 1>> const& stddev)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        static_assert(std::is_same_v<policy::InputElementT<Policies, 1>, policy::InputElementT<Policies, 2>>);
        static_assert(
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 1>>);

        // Reimplement as ternary local operation?

        // The result array will have the same shape, partitioning and
        // location as the input array, but the elements might be of a different
        // type.

        using Element = policy::OutputElementT<Policies, 0>;
        using SomeElement = policy::InputElementT<Policies, 0>;

        using InputArray = PartitionedArray<SomeElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;
        using InputPartition = PartitionT<InputArray>;

        using OutputArray = PartitionedArray<Element, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;

        lue_hpx_assert(all_are_valid(input_array.partitions()));
        lue_hpx_assert(mean.future().valid());
        lue_hpx_assert(stddev.future().valid());

        detail::normal::NormalPartitionAction<Policies, InputPartition> action;

        Localities<rank> const& localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index partition_idx = 0; partition_idx < nr_partitions(input_array); ++partition_idx)
        {
            output_partitions[partition_idx] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[partition_idx], action, policies](
                    InputPartition const& input_partition,
                    hpx::shared_future<Element> const& mean,
                    hpx::shared_future<Element> const& stddev)
                { return action(locality_id, policies, input_partition, mean.get(), stddev.get()); },

                input_partitions[partition_idx],
                mean.future(),
                stddev.future());
        }

        return OutputArray{shape(input_array), localities, std::move(output_partitions)};
    }


    // -------------------------------------------------------------------------
    // normal(policies, mean, stddev)
    // -------------------------------------------------------------------------

    template<typename Policies>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        Scalar<policy::InputElementT<Policies, 0>> const& mean,
        Scalar<policy::InputElementT<Policies, 1>> const& stddev)
        -> Scalar<policy::OutputElementT<Policies, 0>>
    {
        static_assert(std::is_same_v<policy::InputElementT<Policies, 0>, policy::InputElementT<Policies, 1>>);
        static_assert(
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 0>>);

        using Element = policy::OutputElementT<Policies, 0>;

        lue_hpx_assert(mean.future().valid());
        lue_hpx_assert(stddev.future().valid());

        return hpx::dataflow(
            hpx::launch::async,
            hpx::unwrapping(
                [policies](Element const& mean, Element const& stddev) -> Element
                {
                    auto const& indp1 = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                    auto const& indp2 = std::get<1>(policies.inputs_policies()).input_no_data_policy();
                    auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                    Element output_element{};

                    if (indp1.is_no_data(mean) || indp2.is_no_data(stddev))
                    {
                        ondp.mark_no_data(output_element);
                    }
                    else
                    {
                        // Will be used to obtain a seed for the random number engine
                        std::random_device random_device;

                        // Standard mersenne_twister_engine seeded with the random_device
                        std::mt19937 random_number_engine(random_device());

                        auto distribution = [mean, stddev]()
                        { return std::normal_distribution<Element>{mean, stddev}; }();

                        output_element = distribution(random_number_engine);
                    }

                    return output_element;
                }),
            mean.future(),
            stddev.future());
    }


    // -------------------------------------------------------------------------
    // normal(policies, array_shape, mean, stddev)
    // -------------------------------------------------------------------------

    namespace detail {

        template<typename Policies, Rank rank>
        auto normal_partition(
            [[maybe_unused]] Policies const& policies,
            Offset<Index, rank> const& offset,
            Shape<Count, rank> const& partition_shape,
            policy::InputElementT<Policies, 0> const mean,
            policy::InputElementT<Policies, 1> const stddev)
            -> ArrayPartition<policy::OutputElementT<Policies, 0>, rank>
        {
            AnnotateFunction annotation{"normal: partition"};

            using Element = policy::OutputElementT<Policies, 0>;
            using Partition = ArrayPartition<Element, rank>;
            using PartitionData = DataT<Partition>;

            PartitionData partition_data{partition_shape};

            // Will be used to obtain a seed for the random number engine
            std::random_device random_device;

            // Standard mersenne_twister_engine seeded with the random_device
            std::mt19937 random_number_engine(random_device());

            auto distribution = std::normal_distribution<Element>{mean, stddev};

            std::generate(
                partition_data.begin(),
                partition_data.end(),

                [&distribution, &random_number_engine]() { return distribution(random_number_engine); }

            );

            return Partition{hpx::find_here(), offset, std::move(partition_data)};
        }


        template<typename Policies, Rank rank>
        struct NormalPartitionAction2:
            hpx::actions::make_action<
                decltype(&normal_partition<Policies, rank>),
                &normal_partition<Policies, rank>,
                NormalPartitionAction2<Policies, rank>>::type
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


            InstantiateNormal(
                hpx::shared_future<Element> const& mean, hpx::shared_future<Element> const& stddev):

                _mean{mean},
                _stddev{stddev}

            {
            }


            template<typename Policies>
            auto instantiate(
                hpx::id_type const locality_id,
                [[maybe_unused]] Policies const& policies,
                [[maybe_unused]] Shape const& array_shape,
                [[maybe_unused]] Shape const& shape_in_partitions,
                [[maybe_unused]] lue::Index const partition_idx_along_curve,
                [[maybe_unused]] lue::Index const partition_idx,
                Offset const& offset,
                Shape const& partition_shape) -> Partition
            {
                using Action = detail::NormalPartitionAction2<Policies, rank>;

                // return hpx::async(Action{}, locality_id, policies, offset, partition_shape, _mean,
                // _stddev);

                return hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(
                        [locality_id, policies, offset, partition_shape](
                            Element const mean, Element const stddev) -> Partition {
                            return hpx::async(
                                Action{}, locality_id, policies, offset, partition_shape, mean, stddev);
                        }),
                    _mean,
                    _stddev);
            }


        private:

            hpx::shared_future<Element> _mean;

            hpx::shared_future<Element> _stddev;
    };


    template<typename Policies, typename Shape>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        Shape const& array_shape,
        Shape const& partition_shape,
        Scalar<policy::InputElementT<Policies, 0>> const& mean,
        Scalar<policy::InputElementT<Policies, 1>> const& stddev)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>
    {
        static_assert(std::is_same_v<policy::InputElementT<Policies, 0>, policy::InputElementT<Policies, 1>>);
        static_assert(
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 0>>);

        using Functor = InstantiateNormal<policy::OutputElementT<Policies, 0>, rank<Shape>>;

        return create_partitioned_array(
            policies, array_shape, partition_shape, Functor{mean.future(), stddev.future()});
    }


    template<typename Policies, typename Shape>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        Shape const& array_shape,
        Scalar<policy::InputElementT<Policies, 0>> const& mean,
        Scalar<policy::InputElementT<Policies, 1>> const& stddev)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>
    {
        static_assert(std::is_same_v<policy::InputElementT<Policies, 0>, policy::InputElementT<Policies, 1>>);
        static_assert(
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 0>>);

        using Functor = InstantiateNormal<policy::OutputElementT<Policies, 0>, rank<Shape>>;

        return create_partitioned_array(policies, array_shape, Functor{mean.future(), stddev.future()});
    }

}  // namespace lue


#define LUE_INSTANTIATE_NORMAL(Policies, Shape)                                                              \
                                                                                                             \
    template LUE_MISCELLANEOUS_OPERATION_EXPORT Scalar<policy::OutputElementT<Policies, 0>>                  \
    normal<ArgumentType<void(Policies)>>(                                                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        Scalar<policy::InputElementT<Policies, 0>> const& mean,                                              \
        Scalar<policy::InputElementT<Policies, 1>> const& stddev);                                           \
                                                                                                             \
    template LUE_MISCELLANEOUS_OPERATION_EXPORT                                                              \
        PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>                                   \
        normal<ArgumentType<void(Policies)>, Shape>(                                                         \
            ArgumentType<void(Policies)> const&,                                                             \
            Shape const& array_shape,                                                                        \
            Shape const& partition_shape,                                                                    \
            Scalar<policy::InputElementT<Policies, 0>> const& mean,                                          \
            Scalar<policy::InputElementT<Policies, 1>> const& stddev);                                       \
                                                                                                             \
    template LUE_MISCELLANEOUS_OPERATION_EXPORT                                                              \
        PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>                                   \
        normal<ArgumentType<void(Policies)>, Shape>(                                                         \
            ArgumentType<void(Policies)> const&,                                                             \
            Shape const& array_shape,                                                                        \
            Scalar<policy::InputElementT<Policies, 0>> const& mean,                                          \
            Scalar<policy::InputElementT<Policies, 1>> const& stddev);


#define LUE_INSTANTIATE_NORMAL_TEMPLATE(Policies, Shape)                                                     \
                                                                                                             \
    template LUE_MISCELLANEOUS_OPERATION_EXPORT                                                              \
        PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>                                   \
        normal<ArgumentType<void(Policies)>, rank<Shape>>(                                                   \
            ArgumentType<void(Policies)> const&,                                                             \
            PartitionedArray<policy::InputElementT<Policies, 0>, rank<Shape>> const& input_array,            \
            Scalar<policy::InputElementT<Policies, 1>> const& mean,                                          \
            Scalar<policy::InputElementT<Policies, 1>> const& stddev);
