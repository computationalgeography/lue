#pragma once
#include "lue/framework/algorithm/uniform.hpp"


namespace lue {
    namespace policy::uniform {

        // The min/max values are the input arguments whose values are
        // relevant. These have the same element type as the output element.
        template<
            typename OutputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<OutputElement, OutputElement>,
            OutputElements<OutputElement>,
            InputElements<OutputElement, OutputElement>>;

    }  // namespace policy::uniform


    namespace value_policies {

        template<
            typename InputElement,
            typename OutputElement,
            Rank rank>
        PartitionedArray<OutputElement, rank> uniform(
            PartitionedArray<InputElement, rank> const& input_array,
            hpx::shared_future<OutputElement> const& min_value,
            hpx::shared_future<OutputElement> const& max_value)
        {
            using Policies = policy::uniform::DefaultValuePolicies<OutputElement>;

            return uniform(Policies{}, input_array, min_value, max_value);
        }


        template<
            typename InputElement,
            typename OutputElement,
            Rank rank>
        PartitionedArray<OutputElement, rank> uniform(
            PartitionedArray<InputElement, rank> const& input_array,
            OutputElement const min_value,
            OutputElement const max_value)
        {
            return uniform(
                input_array,
                hpx::make_ready_future<OutputElement>(min_value).share(),
                hpx::make_ready_future<OutputElement>(max_value).share());
        }


        template<
            typename Element,
            typename Count,
            Rank rank>
        PartitionedArray<Element, rank> uniform(
            Shape<Count, rank> const& array_shape,
            Shape<Count, rank> const& partition_shape,
            Element const min_value,
            Element const max_value)
        {
            using Policies = policy::uniform::DefaultValuePolicies<Element>;

            return lue::uniform<Element>(
                Policies{}, array_shape, partition_shape, min_value, max_value);
        }

    }  // namespace value_policies

}  // namespace lue
