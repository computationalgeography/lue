#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"


namespace lue {
    namespace detail {

        template<
            typename InputElement>
        class LogicalNot
        {

        public:

            using OutputElement = std::uint8_t;

            constexpr OutputElement operator()(
                InputElement const& input_element) const noexcept
            {
                return !input_element;
            }

        };

    }  // namespace detail


    namespace policy::logical_not {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::logical_not


    template<
        typename InputElement,
        Rank rank>
    PartitionedArray<std::uint8_t, rank> logical_not(
        PartitionedArray<InputElement, rank> const& array)
    {
        using Functor = detail::LogicalNot<InputElement>;
        using OutputElement = OutputElementT<Functor>;
        using Policies = policy::logical_not::DefaultPolicies<OutputElement, InputElement>;

        return unary_local_operation(Policies{}, array, Functor{});
    }

}  // namespace lue
