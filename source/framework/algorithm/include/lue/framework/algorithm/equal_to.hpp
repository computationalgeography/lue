#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"


namespace lue {
    namespace detail {

        template<
            typename InputElement,
            typename OutputElement_=std::uint8_t>
        class EqualTo
        {

            public:

                using OutputElement = OutputElement_;

                constexpr OutputElement operator()(
                    InputElement const& input_element1,
                    InputElement const& input_element2) const noexcept
                {
                    return input_element1 == input_element2;
                }

        };

    }  // namespace detail


    namespace policy::equal_to {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::equal_to


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS(equal_to, detail::EqualTo)


    // partition == scalar_f
    template<
        typename InputElement,
        typename OutputElement=std::uint8_t,
        Rank rank>
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
    template<
        typename InputElement,
        typename OutputElement=std::uint8_t,
        Rank rank>
    ArrayPartition<OutputElement, rank> equal_to(
        hpx::id_type const locality_id,
        ArrayPartition<InputElement, rank> const& partition,
        InputElement const& scalar)
    {
        return equal_to<InputElement, OutputElement, rank>(
            locality_id, partition, hpx::make_ready_future<InputElement>(scalar).share());
    }


    LUE_BINARY_COMPARISON_OPERATOR(==, equal_to)

}  // namespace lue
