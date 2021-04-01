#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"


namespace lue {
    namespace detail {

        template<
            typename InputElement,
            typename OutputElement_=std::uint8_t>
        class Valid
        {

            public:

                using OutputElement = OutputElement_;

                constexpr OutputElement operator()(
                    [[maybe_unused]] InputElement const& input_element) const noexcept
                {
                    return 1;
                }

        };

    }  // namespace detail


    namespace policy::valid {

        // If the input no-data policy returns true, the output no-data
        // policy must write a 0. In all other cases, 1 must be written.
        // In the result, each elements contains a 0 or 1. No element
        // contains a no-data value, or any other value.

        template<
            typename BooleanElement,
            typename ExpressionElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<ExpressionElement>,
            OutputElements<BooleanElement>,
            InputElements<ExpressionElement>>;


        template<
            typename BooleanElement,
            typename ExpressionElement>
        using DefaultValuePoliciesBase = Policies<
            AllValuesWithinDomain<ExpressionElement>,
            OutputsPolicies<OutputPolicies<MarkNoDataByValue<BooleanElement>>>,
            InputsPolicies<InputPolicies<DefaultInputNoDataPolicy<ExpressionElement>>>>;


        template<
            typename BooleanElement,
            typename ExpressionElement>
        class DefaultValuePolicies:
            public DefaultValuePoliciesBase<BooleanElement, ExpressionElement>
        {

            public:

                using PoliciesBase = DefaultValuePoliciesBase<BooleanElement, ExpressionElement>;
                using BooleanOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using ExpressionInputPolicies = InputPoliciesT<PoliciesBase, 0>;

                DefaultValuePolicies():

                    PoliciesBase{
                            DomainPolicyT<PoliciesBase>{},
                            BooleanOutputPolicies{0},
                            ExpressionInputPolicies{}
                        }

                {
                }

        };

    }  // namespace policy::valid


    template<
        typename BooleanElement,
        typename Policies,
        typename ExpressionElement,
        Rank rank>
    PartitionedArray<BooleanElement, rank> valid(
        Policies const& policies,
        PartitionedArray<ExpressionElement, rank> const& array)
    {
        return unary_local_operation(policies, array, detail::Valid<ExpressionElement>{});
    }


    template<
        typename BooleanElement,
        typename ExpressionElement,
        Rank rank>
    PartitionedArray<BooleanElement, rank> valid(
        PartitionedArray<ExpressionElement, rank> const& array)
    {
        using Policies = policy::valid::DefaultPolicies<BooleanElement, ExpressionElement>;

        return valid<BooleanElement>(Policies{}, array);
    }

}  // namespace lue
