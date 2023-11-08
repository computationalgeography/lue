#pragma once
#include "lue/framework/algorithm/normal.hpp"


namespace lue {
    namespace policy::normal {

        template<typename Element>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(
                    [[maybe_unused]] Element const mean, Element const stddev) noexcept
                {
                    return stddev > 0;
                }
        };


        // The mean/stddev values are the input arguments whose values are
        // relevant. These have the same element type as the output element.
        template<typename OutputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<OutputElement>,
            OutputElements<OutputElement>,
            InputElements<OutputElement, OutputElement>>;

    }  // namespace policy::normal


    namespace value_policies {

        template<typename InputElement, typename OutputElement, Rank rank>
        PartitionedArray<OutputElement, rank> normal(
            PartitionedArray<InputElement, rank> const& input_array,
            hpx::shared_future<OutputElement> const& mean,
            hpx::shared_future<OutputElement> const& stddev)
        {
            using Policies = policy::normal::DefaultValuePolicies<OutputElement>;

            return normal(Policies{}, input_array, mean, stddev);
        }


        template<typename InputElement, typename OutputElement, Rank rank>
        PartitionedArray<OutputElement, rank> normal(
            PartitionedArray<InputElement, rank> const& input_array,
            OutputElement const mean,
            OutputElement const stddev)
        {
            return normal(
                input_array,
                hpx::make_ready_future<OutputElement>(mean).share(),
                hpx::make_ready_future<OutputElement>(stddev).share());
        }


        template<typename Element, typename Count, Rank rank>
        PartitionedArray<Element, rank> normal(
            Shape<Count, rank> const& array_shape,
            Shape<Count, rank> const& partition_shape,
            Element const mean,
            Element const stddev)
        {
            using Policies = policy::normal::DefaultValuePolicies<Element>;

            return lue::normal<Element>(Policies{}, array_shape, partition_shape, mean, stddev);
        }

    }  // namespace value_policies

}  // namespace lue
