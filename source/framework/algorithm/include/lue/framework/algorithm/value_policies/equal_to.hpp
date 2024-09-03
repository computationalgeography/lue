#pragma once
#include "lue/framework/algorithm/equal_to.hpp"


namespace lue {
    namespace policy::equal_to {

        template<typename OutputElement, typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::equal_to


    namespace value_policies {

        // partition == scalar_f
        template<typename InputElement, typename OutputElement = std::uint8_t, Rank rank>
        auto equal_to(
            hpx::id_type const locality_id,
            ArrayPartition<InputElement, rank> const& partition,
            hpx::shared_future<InputElement> const& scalar) -> ArrayPartition<OutputElement, rank>
        {
            using Policies = policy::equal_to::DefaultValuePolicies<OutputElement, InputElement>;

            return binary_local_operation(
                locality_id, Policies{}, partition, scalar, detail::EqualTo<InputElement, OutputElement>{});
        }


        // partition == scalar
        template<typename InputElement, typename OutputElement = std::uint8_t, Rank rank>
        auto equal_to(
            hpx::id_type const locality_id,
            ArrayPartition<InputElement, rank> const& partition,
            InputElement const& scalar) -> ArrayPartition<OutputElement, rank>
        {
            return equal_to<InputElement, OutputElement, rank>(
                locality_id, partition, hpx::make_ready_future<InputElement>(scalar).share());
        }


        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            equal_to, policy::equal_to::DefaultValuePolicies)
        LUE_BINARY_COMPARISON_OPERATOR(==, equal_to)

    }  // namespace value_policies
}  // namespace lue
