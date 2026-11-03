#pragma once
#include "lue/framework/algorithm/global_operation_export.hpp"
#include "lue/framework/algorithm/none.hpp"
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

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

    }  // namespace detail


    template<typename Policies, Rank rank>
    auto none(
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)
        -> Scalar<policy::OutputElementT<Policies, 0>>
    {
        static_assert(
            std::is_same_v<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>);

        using Functor = detail::None<policy::InputElementT<Policies, 0>>;

        return unary_aggregate_operation(policies, array, Functor{});
    }


    template<typename Policies, Rank rank>
    auto none(
        hpx::id_type const locality_id,
        Policies const& policies,
        ArrayPartition<policy::InputElementT<Policies, 0>, rank> const& partition)
        -> Scalar<policy::OutputElementT<Policies, 0>>
    {
        static_assert(
            std::is_same_v<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>);

        using Functor = detail::None<policy::InputElementT<Policies, 0>>;

        return unary_aggregate_operation(locality_id, policies, partition, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_NONE(Policies, rank)                                                                 \
                                                                                                             \
    template LUE_GLOBAL_OPERATION_EXPORT auto none<ArgumentType<void(Policies)>, rank>(                      \
        hpx::id_type const locality_id,                                                                      \
        ArgumentType<void(Policies)> const&,                                                                 \
        ArrayPartition<policy::InputElementT<Policies, 0>, rank> const&)                                     \
        -> Scalar<policy::OutputElementT<Policies, 0>>;                                                      \
                                                                                                             \
    template LUE_GLOBAL_OPERATION_EXPORT auto none<ArgumentType<void(Policies)>, rank>(                      \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&)                                   \
        -> Scalar<policy::OutputElementT<Policies, 0>>;
