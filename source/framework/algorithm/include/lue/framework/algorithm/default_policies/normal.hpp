#pragma once
#include "lue/framework/algorithm/normal.hpp"


namespace lue {
    namespace policy::normal {

        // The mean/stddev values are the input arguments whose values are
        // relevant. These have the same element type as the output element.
        template<std::floating_point Element, Arithmetic... SomeElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<SomeElement..., Element, Element>,
            OutputElements<Element>,
            InputElements<SomeElement..., Element, Element>>;

    }  // namespace policy::normal


    namespace default_policies {

        template<typename SomeElement, typename Element, Rank rank>
        auto normal(
            PartitionedArray<SomeElement, rank> const& input_array,
            hpx::shared_future<Element> const& mean,
            hpx::shared_future<Element> const& stddev) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::normal::DefaultPolicies<Element, SomeElement>;

            return lue::normal(Policies{}, input_array, mean, stddev);
        }


        template<typename SomeElement, typename Element, Rank rank>
        auto normal(
            PartitionedArray<SomeElement, rank> const& input_array, Element const mean, Element const stddev)
            -> PartitionedArray<Element, rank>
        {
            return normal(
                input_array,
                hpx::make_ready_future<Element>(mean).share(),
                hpx::make_ready_future<Element>(stddev).share());
        }


        template<typename Element>
        auto normal(hpx::shared_future<Element> const& mean, hpx::shared_future<Element> const& stddev)
            -> Scalar<Element>
        {
            using Policies = policy::normal::DefaultPolicies<Element>;

            return lue::normal(Policies{}, mean, stddev);
        }


        template<typename Element>
        auto normal(Element const mean, Element const stddev) -> Scalar<Element>
        {
            return normal(
                hpx::make_ready_future<Element>(mean).share(),
                hpx::make_ready_future<Element>(stddev).share());
        }


        template<typename Element, typename Count, Rank rank>
        auto normal(
            Shape<Count, rank> const& array_shape,
            Shape<Count, rank> const& partition_shape,
            Element const mean,
            Element const stddev) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::normal::DefaultPolicies<Element>;

            return lue::normal<Element>(Policies{}, array_shape, partition_shape, mean, stddev);
        }

    }  // namespace default_policies
}  // namespace lue
