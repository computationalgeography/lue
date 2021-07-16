#pragma once
#include "lue/framework/algorithm/unary_aggregate_operation.hpp"
#include "lue/framework/algorithm/policy.hpp"


namespace lue {
    namespace detail {

        // Return true if all of the input elements are true
        template<
            typename InputElement>
        class All
        {

            public:

                static_assert(std::is_convertible_v<InputElement, bool>);

                using OutputElement = InputElement;

                constexpr OutputElement operator()() const noexcept
                {
                    // The result is false if there are not values to aggregate
                    return false;
                }

                constexpr OutputElement operator()(
                    InputElement const input_element) const noexcept
                {
                    // The result is true if the input element is true
                    return input_element;
                }

                constexpr OutputElement operator()(
                    InputElement const aggregate_value,
                    InputElement const input_element) const noexcept
                {
                    // The result is true if the value aggregated until now is true
                    // and the input element is true
                    return aggregate_value && input_element;
                }

                constexpr OutputElement partition(
                    OutputElement const input_element) const noexcept
                {
                    // The result is true if the input element is true
                    return input_element;
                }

                constexpr OutputElement partition(
                    OutputElement const aggregated_value,
                    OutputElement const input_element) const noexcept
                {
                    // The result is true if the value aggregated until now is true
                    // and the input element is true
                    return aggregated_value && input_element;
                }

        };

    }  // namespace detail


    template<
        typename Policies,
        typename Element,
        Rank rank>
    hpx::future<Element> all(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array)
    {
        using Functor = detail::All<Element>;

        return unary_aggregate_operation(policies, array, Functor{});
    }


    template<
        typename Policies,
        typename Element,
        Rank rank>
    hpx::future<Element> all(
        hpx::id_type const locality_id,
        Policies const& policies,
        ArrayPartition<Element, rank> const& partition)
    {
        using Functor = detail::All<Element>;

        return unary_aggregate_operation(locality_id, policies, partition, Functor{});
    }

}  // namespace lue
