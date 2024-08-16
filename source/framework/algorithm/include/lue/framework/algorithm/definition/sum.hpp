#pragma once
#include "lue/framework/algorithm/global_operation_export.hpp"
#include "lue/framework/algorithm/sum.hpp"
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_>
        class Sum
        {

            public:

                using OutputElement = OutputElement_;

                constexpr OutputElement operator()() const noexcept
                {
                    // The result is zero if there are no values to aggregate
                    return 0;
                }

                constexpr OutputElement operator()(InputElement const input_element) const noexcept
                {
                    return input_element;
                }

                constexpr OutputElement operator()(
                    InputElement const aggregate_value, InputElement const input_element) const noexcept
                {
                    return aggregate_value + input_element;
                }

                constexpr OutputElement partition(OutputElement const input_element) const noexcept
                {
                    return input_element;
                }

                constexpr OutputElement partition(
                    OutputElement const aggregated_value, OutputElement const input_element) const noexcept
                {
                    return aggregated_value + input_element;
                }
        };

    }  // namespace detail


    template<typename Policies, Rank rank>
    auto sum(
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)
        -> Scalar<policy::OutputElementT<Policies, 0>>
    {
        using Functor = detail::Sum<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>;

        return unary_aggregate_operation(policies, array, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_SUM(Policies, rank)                                                                  \
                                                                                                             \
    template LUE_GLOBAL_OPERATION_EXPORT auto sum<ArgumentType<void(Policies)>, rank>(                       \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&)                                   \
        ->Scalar<policy::OutputElementT<Policies, 0>>;
