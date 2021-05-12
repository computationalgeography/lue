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
        class LogicalAnd
        {

            public:

                static_assert(std::is_integral_v<InputElement>);

                using OutputElement = OutputElement_;


                constexpr OutputElement operator()(
                    InputElement const& input_element1,
                    InputElement const& input_element2) const noexcept
                {
                    return input_element1 && input_element2;
                }

        };

    }  // namespace detail


    namespace policy::logical_and {

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

    }  // namespace policy::logical_and


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS(logical_and, detail::LogicalAnd)
    LUE_BINARY_LOGICAL_OPERATOR(&&, logical_and)

}  // namespace lue
