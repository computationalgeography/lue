#pragma once
#include "lue/framework/algorithm/uniform.hpp"
#include "lue/concept.hpp"


namespace lue {
    namespace policy::uniform {

        // The min/max values are the input arguments whose values are
        // relevant. These have the same element type as the output element.
        template<Arithmetic Element, Arithmetic... SomeElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<SomeElement..., Element, Element>,
            OutputElements<Element>,
            InputElements<SomeElement..., Element, Element>>;

    }  // namespace policy::uniform


    namespace default_policies {

        template<Arithmetic SomeElement, Arithmetic Element, Rank rank>
        requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>) auto uniform(
            PartitionedArray<SomeElement, rank> const& input_array,
            hpx::shared_future<Element> const& min_value,
            hpx::shared_future<Element> const& max_value) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::uniform::DefaultPolicies<Element, SomeElement>;

            return uniform(Policies{}, input_array, min_value, max_value);
        }


        template<Arithmetic SomeElement, Arithmetic Element, Rank rank>
        requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>) auto uniform(
            PartitionedArray<SomeElement, rank> const& input_array,
            Element const min_value,
            Element const max_value) -> PartitionedArray<Element, rank>
        {
            return uniform(
                input_array,
                hpx::make_ready_future<Element>(min_value).share(),
                hpx::make_ready_future<Element>(max_value).share());
        }


        template<Arithmetic Element, typename Count, Rank rank>
        requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>) auto uniform(
            Shape<Count, rank> const& array_shape,
            Shape<Count, rank> const& partition_shape,
            Element const min_value,
            Element const max_value) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::uniform::DefaultPolicies<Element>;

            return lue::uniform(Policies{}, array_shape, partition_shape, min_value, max_value);
        }


        template<Arithmetic Element, typename Count, Rank rank>
        requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>) auto uniform(
            Shape<Count, rank> const& array_shape, Element const min_value, Element const max_value)
            -> PartitionedArray<Element, rank>
        {
            using Policies = policy::uniform::DefaultPolicies<Element>;

            return lue::uniform(Policies{}, array_shape, min_value, max_value);
        }

    }  // namespace default_policies
}  // namespace lue
