#pragma once
#include "lue/framework/algorithm/uniform.hpp"


namespace lue {
    namespace policy::uniform {

        // The min/max values are the input arguments whose values are
        // relevant. These have the same element type as the output element.
        template<Arithmetic Element, Arithmetic... SomeElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<SomeElement..., Element, Element>,
            OutputElements<Element>,
            InputElements<SomeElement..., Element, Element>>;

    }  // namespace policy::uniform


    namespace value_policies {

        template<Arithmetic Element, Arithmetic SomeElement, Rank rank>
            requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>)
        auto uniform(
            PartitionedArray<SomeElement, rank> const& input_array,
            Scalar<Element> const& min_value,
            Scalar<Element> const& max_value) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::uniform::DefaultValuePolicies<Element, SomeElement>;

            return lue::uniform(Policies{}, input_array, min_value, max_value);
        }


        template<Arithmetic Element, Arithmetic SomeElement, Rank rank>
            requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>)
        auto uniform(
            PartitionedArray<SomeElement, rank> const& input_array,
            Element const min_value,
            Element const max_value) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::uniform::DefaultValuePolicies<Element, SomeElement>;

            return lue::uniform(Policies{}, input_array, min_value, max_value);
        }


        template<Arithmetic Element>
            requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>)
        auto uniform(Scalar<Element> const& min_value, Scalar<Element> const& max_value) -> Scalar<Element>
        {
            using Policies = policy::uniform::DefaultValuePolicies<Element>;

            return lue::uniform(Policies{}, min_value, max_value);
        }


        template<Arithmetic Element>
            requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>)
        auto uniform(Element const min_value, Element const max_value) -> Scalar<Element>
        {
            using Policies = policy::uniform::DefaultValuePolicies<Element>;

            return lue::uniform(Policies{}, min_value, max_value);
        }


        template<Arithmetic Element, typename Count, Rank rank>
            requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>)
        auto uniform(
            Shape<Count, rank> const& array_shape,
            Shape<Count, rank> const& partition_shape,
            Scalar<Element> const& min_value,
            Scalar<Element> const& max_value) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::uniform::DefaultValuePolicies<Element>;

            return lue::uniform(Policies{}, array_shape, partition_shape, min_value, max_value);
        }


        template<Arithmetic Element, typename Count, Rank rank>
            requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>)
        auto uniform(
            Shape<Count, rank> const& array_shape,
            Shape<Count, rank> const& partition_shape,
            Element const min_value,
            Element const max_value) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::uniform::DefaultValuePolicies<Element>;

            return lue::uniform(Policies{}, array_shape, partition_shape, min_value, max_value);
        }


        template<Arithmetic Element, typename Count, Rank rank>
            requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>)
        auto uniform(
            Shape<Count, rank> const& array_shape,
            Scalar<Element> const& min_value,
            Scalar<Element> const& max_value) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::uniform::DefaultValuePolicies<Element>;

            return lue::uniform(Policies{}, array_shape, min_value, max_value);
        }


        template<Arithmetic Element, typename Count, Rank rank>
            requires(!std::is_same_v<Element, std::uint8_t> && !std::is_same_v<Element, std::int8_t>)
        auto uniform(Shape<Count, rank> const& array_shape, Element const min_value, Element const max_value)
            -> PartitionedArray<Element, rank>
        {
            using Policies = policy::uniform::DefaultValuePolicies<Element>;

            return lue::uniform(Policies{}, array_shape, min_value, max_value);
        }

    }  // namespace value_policies
}  // namespace lue
