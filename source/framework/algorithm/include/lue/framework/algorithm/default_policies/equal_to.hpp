#pragma once
#include "lue/framework/algorithm/equal_to.hpp"


namespace lue {
    namespace policy::equal_to {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::equal_to


    namespace default_policies {

        // partition == scalar_f
        template<typename InputElement, typename OutputElement = std::uint8_t, Rank rank>
        ArrayPartition<OutputElement, rank> equal_to(
            hpx::id_type const locality_id,
            ArrayPartition<InputElement, rank> const& partition,
            hpx::shared_future<InputElement> const& scalar)
        {
            using Policies = policy::equal_to::DefaultPolicies<OutputElement, InputElement>;

            return binary_local_operation(
                locality_id, Policies{}, partition, scalar, detail::EqualTo<InputElement, OutputElement>{});
        }


        // partition == scalar
        template<typename InputElement, typename OutputElement = std::uint8_t, Rank rank>
        ArrayPartition<OutputElement, rank> equal_to(
            hpx::id_type const locality_id,
            ArrayPartition<InputElement, rank> const& partition,
            InputElement const& scalar)
        {
            return equal_to<InputElement, OutputElement, rank>(
                locality_id, partition, hpx::make_ready_future<InputElement>(scalar).share());
        }


        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(equal_to, detail::EqualTo, policy::equal_to::DefaultPolicies)
        LUE_BINARY_COMPARISON_OPERATOR(==, equal_to)

    }  // namespace default_policies
}  // namespace lue
