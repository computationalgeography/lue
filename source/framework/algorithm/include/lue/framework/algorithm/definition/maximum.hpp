#pragma once
#include "lue/framework/algorithm/global_operation_export.hpp"
#include "lue/framework/algorithm/maximum.hpp"
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_>
        class Maximum
        {

            public:

                using OutputElement = OutputElement_;

                constexpr OutputElement operator()() const noexcept
                {
                    // The result is small if there are no values to aggregate
                    return std::numeric_limits<OutputElement>::min();
                }

                constexpr OutputElement operator()(InputElement const input_element) const noexcept
                {
                    return input_element;
                }

                constexpr OutputElement operator()(
                    InputElement const aggregated_value, InputElement const input_element) const noexcept
                {
                    return std::max(aggregated_value, input_element);
                }

                constexpr OutputElement partition(OutputElement const input_element) const noexcept
                {
                    return input_element;
                }

                constexpr OutputElement partition(
                    OutputElement const aggregated_value, OutputElement const input_element) const noexcept
                {
                    return std::max(aggregated_value, input_element);
                }
        };

    }  // namespace detail


    /*!
      @brief      Return the maximum element value in the array passed in
      @ingroup    global_operation
    */
    template<typename Policies, Rank rank>
    auto maximum(
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)
        -> Scalar<policy::OutputElementT<Policies, 0>>
    {
        using Functor =
            detail::Maximum<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>;

        return unary_aggregate_operation(policies, array, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_MAXIMUM(Policies, rank)                                                              \
                                                                                                             \
    template LUE_GLOBAL_OPERATION_EXPORT auto maximum<ArgumentType<void(Policies)>, rank>(                   \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&)                                   \
        ->Scalar<policy::OutputElementT<Policies, 0>>;
