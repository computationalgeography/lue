#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"


namespace lue::detail {

    // Return true if none of the input elements is true. All of them evaluate
    // to false.
    template<typename InputElement>
    class None
    {

        public:

            static_assert(std::is_convertible_v<InputElement, bool>);

            using OutputElement = InputElement;

            constexpr OutputElement operator()() const noexcept
            {
                // The result is false if there are no values to aggregate
                return false;
            }

            constexpr OutputElement operator()(InputElement const input_element) const noexcept
            {
                // The result is true if the input element is not true
                return !input_element;
            }

            constexpr OutputElement operator()(
                InputElement const aggregated_value, InputElement const input_element) const noexcept
            {
                // The result is true if the value aggregated until now is true
                // and the input element is not true
                return aggregated_value && !input_element;
            }

            constexpr OutputElement partition(OutputElement const input_element) const noexcept
            {
                // The result is true if the input element is true
                return input_element;
            }

            constexpr OutputElement partition(
                OutputElement const aggregated_value, OutputElement const input_element) const noexcept
            {
                // The result is true if the value aggregated until now is true
                // and the input element is true
                return aggregated_value && input_element;
            }
    };

}  // namespace lue::detail
