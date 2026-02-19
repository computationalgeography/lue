#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    template<typename Policies, Rank rank>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array,
        Scalar<policy::InputElementT<Policies, 1>> const& mean,
        Scalar<policy::InputElementT<Policies, 1>> const& stddev)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;


    template<typename Policies, Rank rank>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array,
        policy::InputElementT<Policies, 1> const mean,
        policy::InputElementT<Policies, 1> const stddev)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        return normal<Policies, rank>(
            policies,
            input_array,
            Scalar<policy::InputElementT<Policies, 1>>{mean},
            Scalar<policy::InputElementT<Policies, 2>>{stddev});
    }


    template<typename Policies>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        Scalar<policy::InputElementT<Policies, 0>> const& mean,
        Scalar<policy::InputElementT<Policies, 1>> const& stddev)
        -> Scalar<policy::OutputElementT<Policies, 0>>;


    template<typename Policies>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        policy::InputElementT<Policies, 0> const mean,
        policy::InputElementT<Policies, 1> const stddev) -> Scalar<policy::OutputElementT<Policies, 0>>
    {
        return normal(
            policies,
            Scalar<policy::InputElementT<Policies, 0>>{mean},
            Scalar<policy::InputElementT<Policies, 1>>{stddev});
    }


    template<typename Policies, typename Shape>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        Shape const& array_shape,
        Shape const& partition_shape,
        Scalar<policy::InputElementT<Policies, 0>> const& mean,
        Scalar<policy::InputElementT<Policies, 1>> const& stddev)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>;


    template<typename Policies, typename Shape>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        Shape const& array_shape,
        Shape const& partition_shape,
        policy::InputElementT<Policies, 0> const mean,
        policy::InputElementT<Policies, 1> const stddev)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>
    {
        return normal(
            policies,
            array_shape,
            partition_shape,
            Scalar<policy::InputElementT<Policies, 0>>{mean},
            Scalar<policy::InputElementT<Policies, 1>>{stddev});
    }


    template<typename Policies, typename Shape>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        Shape const& array_shape,
        Scalar<policy::InputElementT<Policies, 0>> const& mean,
        Scalar<policy::InputElementT<Policies, 1>> const& stddev)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>;


    template<typename Policies, typename Shape>
        requires(std::is_floating_point_v<policy::OutputElementT<Policies, 0>>)
    auto normal(
        Policies const& policies,
        Shape const& array_shape,
        policy::InputElementT<Policies, 0> const mean,
        policy::InputElementT<Policies, 1> const stddev)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank<Shape>>
    {
        return normal(
            policies,
            array_shape,
            Scalar<policy::InputElementT<Policies, 0>>{mean},
            Scalar<policy::InputElementT<Policies, 1>>{stddev});
    }

}  // namespace lue
