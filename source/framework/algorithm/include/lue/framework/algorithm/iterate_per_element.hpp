#pragma once
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement>
        class IteratePerElement
        {

            public:

                static_assert(std::is_arithmetic_v<InputElement>);

                using OutputElement = InputElement;


                constexpr OutputElement operator()(InputElement const& input_element) const noexcept
                {
                    // The use of volatile prevends the optimizing compiler
                    // to remove this iteration
                    volatile InputElement nr_iterations{std::max<InputElement>(input_element, 0)};

                    while (nr_iterations > InputElement{0})
                    {
                        nr_iterations -= 1;
                    }

                    return input_element;
                }
        };

    }  // namespace detail


    namespace policy::iterate_per_element {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::iterate_per_element


    template<typename Policies, typename Element, Rank rank>
    PartitionedArray<Element, rank> iterate_per_element(
        Policies const& policies, PartitionedArray<Element, rank> const& input_array)
    {
        using Functor = detail::IteratePerElement<Element>;

        return unary_local_operation(policies, input_array, Functor{});
    }


    /*!
        @brief      Per cell in a partitioned array, iterate a number of
                    times before copying the cell to the result

        This algorithm has the same effect as the copy algorithm, but spends
        some more time. It is useful when simulating a configurable amount
        of load per cell.
    */
    template<typename Element, Rank rank>
    PartitionedArray<Element, rank> iterate_per_element(PartitionedArray<Element, rank> const& input_array)
    {
        using Policies = policy::iterate_per_element::DefaultPolicies<Element>;

        return iterate_per_element(Policies{}, input_array);
    }

}  // namespace lue
