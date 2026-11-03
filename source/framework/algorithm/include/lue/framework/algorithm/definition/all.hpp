#pragma once
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/global_operation_export.hpp"
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        // Return true if all of the input elements are true
        template<typename InputElement>
        class All
        {

            public:

                static_assert(std::is_convertible_v<InputElement, bool>);

                using OutputElement = InputElement;

                constexpr auto operator()() const noexcept -> OutputElement
                {
                    // The result is false if there are not values to aggregate
                    return false;
                }

                constexpr auto operator()(InputElement const input_element) const noexcept -> OutputElement
                {
                    // The result is true if the input element is true
                    return input_element;
                }

                constexpr auto operator()(
                    InputElement const aggregate_value, InputElement const input_element) const noexcept
                    -> OutputElement
                {
                    // The result is true if the value aggregated until now is true
                    // and the input element is true
                    return aggregate_value && input_element;
                }

                constexpr auto partition(OutputElement const input_element) const noexcept -> OutputElement
                {
                    // The result is true if the input element is true
                    return input_element;
                }

                constexpr auto partition(
                    OutputElement const aggregated_value, OutputElement const input_element) const noexcept
                    -> OutputElement
                {
                    // The result is true if the value aggregated until now is true
                    // and the input element is true
                    return aggregated_value && input_element;
                }
        };

    }  // namespace detail


    template<typename Policies, Rank rank>
    auto all(
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)
        -> Scalar<policy::OutputElementT<Policies, 0>>
    {
        static_assert(
            std::is_same_v<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>);

        using Functor = detail::All<policy::InputElementT<Policies, 0>>;

        return unary_aggregate_operation(policies, array, Functor{});
    }


    template<typename Policies, Rank rank>
    auto all(
        hpx::id_type const locality_id,
        Policies const& policies,
        ArrayPartition<policy::InputElementT<Policies, 0>, rank> const& partition)
        -> Scalar<policy::OutputElementT<Policies, 0>>
    {
        static_assert(
            std::is_same_v<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>);

        using Functor = detail::All<policy::InputElementT<Policies, 0>>;

        return unary_aggregate_operation(locality_id, policies, partition, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ALL(Policies, rank)                                                                  \
                                                                                                             \
    template LUE_GLOBAL_OPERATION_EXPORT auto all<ArgumentType<void(Policies)>, rank>(                       \
        hpx::id_type const locality_id,                                                                      \
        ArgumentType<void(Policies)> const&,                                                                 \
        ArrayPartition<policy::InputElementT<Policies, 0>, rank> const&)                                     \
        -> Scalar<policy::OutputElementT<Policies, 0>>;                                                      \
                                                                                                             \
    template LUE_GLOBAL_OPERATION_EXPORT auto all<ArgumentType<void(Policies)>, rank>(                       \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&)                                   \
        -> Scalar<policy::OutputElementT<Policies, 0>>;
