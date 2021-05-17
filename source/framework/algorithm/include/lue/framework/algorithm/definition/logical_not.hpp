#pragma once
#include "lue/framework/algorithm/logical_not.hpp"
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/framework/algorithm/export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<
            typename InputElement>
        class LogicalNot
        {

            public:

                static_assert(std::is_integral_v<InputElement>);

                using OutputElement = std::uint8_t;


                constexpr OutputElement operator()(
                    InputElement const& input_element) const noexcept
                {
                    return !input_element;
                }

        };

    }  // namespace detail


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
