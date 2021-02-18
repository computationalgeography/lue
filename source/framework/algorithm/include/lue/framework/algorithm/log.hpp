#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<
            typename InputElement>
        class Log
        {

            public:

                static_assert(std::is_floating_point_v<InputElement>);

                using OutputElement = InputElement;

                OutputElement operator()(
                    InputElement const& input_element) const noexcept
                {
                    return std::log(input_element);
                }

        };

    }  // namespace detail


    namespace policy::log {

        template<
            typename Element>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(
                    Element const element) noexcept
                {
                    return element >= 0;
                }

        };


        template<
            typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            DomainPolicy<Element>,
            OutputElements<Element>,
            InputElements<Element>>;


        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::log


    template<
        typename Policies,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> log(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array)
    {
        return unary_local_operation(policies, array, detail::Log<Element>{});
    }


    template<
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> log(
        PartitionedArray<Element, rank> const& array)
    {
        using Policies = policy::log::DefaultPolicies<Element>;

        return log(Policies{}, array);
    }

}  // namespace lue
