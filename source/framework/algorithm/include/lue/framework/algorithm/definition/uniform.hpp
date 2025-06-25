#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/miscellaneous_operation_export.hpp"
#include "lue/framework/algorithm/uniform.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"
#include <algorithm>
#include <random>


// https://stackoverflow.com/questions/31460733/why-arent-stduniform-int-distributionuint8-t-and-stduniform-int-distri

namespace lue {

    // -------------------------------------------------------------------------
    // uniform(policies, array, min_value, max_value) -> array
    // -------------------------------------------------------------------------

    namespace detail::uniform {

        template<typename Policies, typename InputPartition>
        auto uniform_partition(
            Policies const& policies,
            InputPartition const& input_partition,
            policy::OutputElementT<Policies, 0> const min_value,
            policy::OutputElementT<Policies, 0> const max_value)
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

                    [policies, input_partition, min_value, max_value](
                        Offset const& offset, Shape const& shape)
                    {
                        AnnotateFunction annotation{"uniform: partition"};

                        HPX_UNUSED(input_partition);

                        OutputData output_partition_data{shape};

                        // TODO Use indp1 to test for no-data-ness in input partition's data(?)
                        auto const& indp2 = std::get<1>(policies.inputs_policies()).input_no_data_policy();
                        auto const& indp3 = std::get<2>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                        if (indp2.is_no_data(min_value) || indp3.is_no_data(max_value))
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
                                if constexpr (std::is_floating_point_v<Element>)
                                {
                                    return std::uniform_real_distribution<Element>{min_value, max_value};
                                }
                                else if constexpr (std::is_integral_v<Element>)
                                {
                                    return std::uniform_int_distribution<Element>{min_value, max_value};
                                }
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


        template<typename Policies, typename InputPartition>
        struct UniformPartitionAction:
            hpx::actions::make_action<
                decltype(&uniform_partition<Policies, InputPartition>),
                &uniform_partition<Policies, InputPartition>,
                UniformPartitionAction<Policies, InputPartition>>::type
        {
        };

    }  // namespace detail::uniform


    /*!
        @brief      Return a partitioned array filled with uniformly distributed random values

        In case the output element is integral, the values are drawn from the closed interval `[min_value,
        max_value]`. In case the output element is a floating point, the values are drawn from the half
       open interval `[min_value, max_value)`.
    */
    template<typename Policies, Rank rank>
        requires(!std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
                 !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array,
        Scalar<policy::InputElementT<Policies, 1>> const& min_value,
        Scalar<policy::InputElementT<Policies, 1>> const& max_value)
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
        lue_hpx_assert(min_value.future().valid());
        lue_hpx_assert(max_value.future().valid());

        detail::uniform::UniformPartitionAction<Policies, InputPartition> action;

        Localities<rank> const& localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index partition_idx = 0; partition_idx < nr_partitions(input_array); ++partition_idx)
        {
            output_partitions[partition_idx] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[partition_idx], action, policies](
                    InputPartition const& input_partition,
                    hpx::shared_future<Element> const& min_value,
                    hpx::shared_future<Element> const& max_value)
                { return action(locality_id, policies, input_partition, min_value.get(), max_value.get()); },

                input_partitions[partition_idx],
                min_value.future(),
                max_value.future());
        }

        return OutputArray{shape(input_array), localities, std::move(output_partitions)};
    }

    // -------------------------------------------------------------------------
    // uniform(policies, min_value, max_value) -> scalar
    // -------------------------------------------------------------------------

    /*!
        @brief      Return a scalar containing a uniformly distributed random value

        In case the output element is integral, the value is drawn from the closed interval `[min_value,
        max_value]`. In case the output element is a floating point, the value is drawn from the half
       open interval `[min_value, max_value)`.
    */
    template<typename Policies>
        requires(!std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
                 !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        Scalar<policy::InputElementT<Policies, 0>> const& min_value,
        Scalar<policy::InputElementT<Policies, 1>> const& max_value)
        -> Scalar<policy::OutputElementT<Policies, 0>>
    {
        static_assert(std::is_same_v<policy::InputElementT<Policies, 0>, policy::InputElementT<Policies, 1>>);
        static_assert(
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 0>>);

        using Element = policy::OutputElementT<Policies, 0>;

        lue_hpx_assert(min_value.future().valid());
        lue_hpx_assert(max_value.future().valid());

        return hpx::dataflow(
            hpx::launch::async,
            hpx::unwrapping(
                [policies](Element const& min_value, Element const& max_value) -> Element
                {
                    auto const& indp1 = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                    auto const& indp2 = std::get<1>(policies.inputs_policies()).input_no_data_policy();
                    auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                    Element output_element{};

                    if (indp1.is_no_data(min_value) || indp2.is_no_data(max_value))
                    {
                        ondp.mark_no_data(output_element);
                    }
                    else
                    {
                        // Will be used to obtain a seed for the random number engine
                        std::random_device random_device;

                        // Standard mersenne_twister_engine seeded with the random_device
                        std::mt19937 random_number_engine(random_device());

                        auto distribution = [min_value, max_value]()
                        {
                            if constexpr (std::is_floating_point_v<Element>)
                            {
                                return std::uniform_real_distribution<Element>{min_value, max_value};
                            }
                            else if constexpr (std::is_integral_v<Element>)
                            {
                                return std::uniform_int_distribution<Element>{min_value, max_value};
                            }
                        }();

                        output_element = distribution(random_number_engine);
                    }

                    return output_element;
                }),
            min_value.future(),
            max_value.future());
    }


    // -------------------------------------------------------------------------
    // uniform(policies, array_shape, min_value, max_value) -> array
    // -------------------------------------------------------------------------

    namespace detail {

        template<typename Policies, Rank rank>
        auto uniform_partition(
            [[maybe_unused]] Policies const& policies,
            OffsetT<ArrayPartition<policy::OutputElementT<Policies, 0>, rank>> const& offset,
            ShapeT<ArrayPartition<policy::OutputElementT<Policies, 0>, rank>> const& partition_shape,
            policy::InputElementT<Policies, 0> const min_value,
            policy::InputElementT<Policies, 1> const max_value)
            -> ArrayPartition<policy::OutputElementT<Policies, 0>, rank>
        {
            AnnotateFunction annotation{"uniform: partition"};

            using Element = policy::OutputElementT<Policies, 0>;
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


        template<typename Policies, Rank rank>
        struct UniformPartitionAction2:
            hpx::actions::make_action<
                decltype(&uniform_partition<Policies, rank>),
                &uniform_partition<Policies, rank>,
                UniformPartitionAction2<Policies, rank>>::type
        {
        };

    }  // namespace detail


    template<typename Element, Rank rank>
    class InstantiateUniform
    {

        public:

            using OutputElement = Element;
            using Partition = lue::ArrayPartition<Element, rank>;
            using Offset = lue::OffsetT<Partition>;
            using Shape = lue::ShapeT<Partition>;


            static constexpr bool instantiate_per_locality{false};


            InstantiateUniform(
                hpx::shared_future<Element> const& min_value, hpx::shared_future<Element> const& max_value):

                _min_value{min_value},
                _max_value{max_value}

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
                using Action = detail::UniformPartitionAction2<Policies, rank>;

                return hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(
                        [locality_id, policies, offset, partition_shape](
                            Element const min_value, Element const max_value) -> Partition {
                            return hpx::async(
                                Action{},
                                locality_id,
                                policies,
                                offset,
                                partition_shape,
                                min_value,
                                max_value);
                        }),
                    _min_value,
                    _max_value);
            }


        private:

            hpx::shared_future<Element> _min_value;

            hpx::shared_future<Element> _max_value;
    };


    /*!
        @brief      Return a partitioned array filled with uniformly distributed random values

        In case the output element is integral, the values are drawn from the closed interval `[min_value,
        max_value]`. In case the output element is a floating point, the values are drawn from the half
       open interval `[min_value, max_value)`.
    */
    template<typename Policies, typename Shape>
        requires(!std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
                 !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        Shape const& array_shape,
        Shape const& partition_shape,
        Scalar<policy::InputElementT<Policies, 0>> const& min_value,
        Scalar<policy::InputElementT<Policies, 1>> const& max_value)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>
    {
        static_assert(std::is_same_v<policy::InputElementT<Policies, 0>, policy::InputElementT<Policies, 1>>);
        static_assert(
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 0>>);

        using Functor = InstantiateUniform<policy::OutputElementT<Policies, 0>, rank<Shape>>;

        return create_partitioned_array(
            policies, array_shape, partition_shape, Functor{min_value.future(), max_value.future()});
    }


    /*!
        @overload
    */
    template<typename Policies, typename Shape>
        requires(!std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
                 !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        Shape const& array_shape,
        Scalar<policy::InputElementT<Policies, 0>> const& min_value,
        Scalar<policy::InputElementT<Policies, 1>> const& max_value)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>
    {
        static_assert(std::is_same_v<policy::InputElementT<Policies, 0>, policy::InputElementT<Policies, 1>>);
        static_assert(
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 0>>);

        using Functor = InstantiateUniform<policy::OutputElementT<Policies, 0>, rank<Shape>>;

        return create_partitioned_array(
            policies, array_shape, Functor{min_value.future(), max_value.future()});
    }

}  // namespace lue


#define LUE_INSTANTIATE_UNIFORM(Policies, Shape)                                                             \
                                                                                                             \
    template LUE_MISCELLANEOUS_OPERATION_EXPORT Scalar<policy::OutputElementT<Policies, 0>>                  \
    uniform<ArgumentType<void(Policies)>>(                                                                   \
        ArgumentType<void(Policies)> const&,                                                                 \
        Scalar<policy::InputElementT<Policies, 0>> const& min_value,                                         \
        Scalar<policy::InputElementT<Policies, 1>> const& max_value);                                        \
                                                                                                             \
    template LUE_MISCELLANEOUS_OPERATION_EXPORT                                                              \
        PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>                                   \
        uniform<ArgumentType<void(Policies)>, Shape>(                                                        \
            ArgumentType<void(Policies)> const&,                                                             \
            Shape const& array_shape,                                                                        \
            Shape const& partition_shape,                                                                    \
            Scalar<policy::InputElementT<Policies, 0>> const& min_value,                                     \
            Scalar<policy::InputElementT<Policies, 1>> const& max_value);                                    \
                                                                                                             \
    template LUE_MISCELLANEOUS_OPERATION_EXPORT                                                              \
        PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>                                   \
        uniform<ArgumentType<void(Policies)>, Shape>(                                                        \
            ArgumentType<void(Policies)> const&,                                                             \
            Shape const& array_shape,                                                                        \
            Scalar<policy::InputElementT<Policies, 0>> const& min_value,                                     \
            Scalar<policy::InputElementT<Policies, 1>> const& max_value);


#define LUE_INSTANTIATE_UNIFORM_TEMPLATE(Policies, Shape)                                                    \
                                                                                                             \
    template LUE_MISCELLANEOUS_OPERATION_EXPORT                                                              \
        PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>                                   \
        uniform<ArgumentType<void(Policies)>, rank<Shape>>(                                                  \
            ArgumentType<void(Policies)> const&,                                                             \
            PartitionedArray<policy::InputElementT<Policies, 0>, rank<Shape>> const& input_array,            \
            Scalar<policy::InputElementT<Policies, 1>> const& min_value,                                     \
            Scalar<policy::InputElementT<Policies, 1>> const& max_value);
