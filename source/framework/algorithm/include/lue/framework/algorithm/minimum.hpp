#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_>
        class Minimum
        {

            public:

                using OutputElement = OutputElement_;

                constexpr OutputElement operator()() const noexcept
                {
                    // The result is large if there are no values to aggregate
                    return std::numeric_limits<OutputElement>::max();
                }

                constexpr OutputElement operator()(InputElement const input_element) const noexcept
                {
                    return input_element;
                }

                constexpr OutputElement operator()(
                    InputElement const aggregated_value, InputElement const input_element) const noexcept
                {
                    return std::min(aggregated_value, input_element);
                }

                constexpr OutputElement partition(OutputElement const input_element) const noexcept
                {
                    return input_element;
                }

                constexpr OutputElement partition(
                    OutputElement const aggregated_value, OutputElement const input_element) const noexcept
                {
                    return std::min(aggregated_value, input_element);
                }
        };

    }  // namespace detail


    template<typename Policies, typename Element, Rank rank>
    hpx::future<Element> minimum(Policies const& policies, PartitionedArray<Element, rank> const& array)
    {
        using Functor = detail::Minimum<Element, Element>;

        return unary_aggregate_operation(policies, array, Functor{});
    }

}  // namespace lue
