#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    /*!
        @brief      Return a partitioned array filled with uniformly distributed random values

        In case the output element is integral, the values are drawn from the closed interval `[min_value,
        max_value]`. In case the output element is a floating point, the values are drawn from the half
        open interval `[min_value, max_value)`.
    */
    template<typename Policies, Rank rank>
        requires(
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array,
        Scalar<policy::InputElementT<Policies, 1>> const& min_value,
        Scalar<policy::InputElementT<Policies, 1>> const& max_value)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;


    /*!
        @overload
    */
    template<typename Policies, Rank rank>
        requires(
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array,
        policy::InputElementT<Policies, 1> const min_value,
        policy::InputElementT<Policies, 1> const max_value)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        return uniform<Policies, rank>(
            policies,
            input_array,
            Scalar<policy::InputElementT<Policies, 1>>{min_value},
            Scalar<policy::InputElementT<Policies, 2>>{max_value});
    }


    /*!
        @brief      Return a scalar containing a uniformly distributed random value

        In case the output element is integral, the value is drawn from the closed interval `[min_value,
        max_value]`. In case the output element is a floating point, the value is drawn from the half
        open interval `[min_value, max_value)`.
    */
    template<typename Policies>
        requires(
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        Scalar<policy::InputElementT<Policies, 0>> const& min_value,
        Scalar<policy::InputElementT<Policies, 1>> const& max_value)
        -> Scalar<policy::OutputElementT<Policies, 0>>;


    /*!
        @overload
    */
    template<typename Policies>
        requires(
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        policy::InputElementT<Policies, 0> const min_value,
        policy::InputElementT<Policies, 1> const max_value) -> Scalar<policy::OutputElementT<Policies, 0>>
    {
        return uniform(
            policies,
            Scalar<policy::InputElementT<Policies, 0>>{min_value},
            Scalar<policy::InputElementT<Policies, 1>>{max_value});
    }


    /*!
        @brief      Return a partitioned array filled with uniformly distributed random values

        In case the output element is integral, the values are drawn from the closed interval `[min_value,
        max_value]`. In case the output element is a floating point, the values are drawn from the half
        open interval `[min_value, max_value)`.
    */
    template<typename Policies, typename Shape>
        requires(
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        Shape const& array_shape,
        Shape const& partition_shape,
        Scalar<policy::InputElementT<Policies, 0>> const& min_value,
        Scalar<policy::InputElementT<Policies, 1>> const& max_value)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>;


    /*!
        @overload
    */
    template<typename Policies, typename Shape>
        requires(
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        Shape const& array_shape,
        Shape const& partition_shape,
        policy::InputElementT<Policies, 0> const min_value,
        policy::InputElementT<Policies, 1> const max_value)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>
    {
        return uniform(
            policies,
            array_shape,
            partition_shape,
            Scalar<policy::InputElementT<Policies, 0>>{min_value},
            Scalar<policy::InputElementT<Policies, 1>>{max_value});
    }


    /*!
        @overload
    */
    template<typename Policies, typename Shape>
        requires(
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        Shape const& array_shape,
        Scalar<policy::InputElementT<Policies, 0>> const& min_value,
        Scalar<policy::InputElementT<Policies, 1>> const& max_value)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>;


    /*!
        @overload
    */
    template<typename Policies, typename Shape>
        requires(
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::uint8_t> &&
            !std::is_same_v<policy::OutputElementT<Policies, 0>, std::int8_t>)
    auto uniform(
        Policies const& policies,
        Shape const& array_shape,
        policy::InputElementT<Policies, 0> const min_value,
        policy::InputElementT<Policies, 1> const max_value)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>
    {
        return uniform(
            policies,
            array_shape,
            Scalar<policy::InputElementT<Policies, 0>>{min_value},
            Scalar<policy::InputElementT<Policies, 1>>{max_value});
    }

}  // namespace lue
