#pragma once
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class IteratePerElement
{

public:

    static_assert(std::is_integral_v<InputElement>);

    using OutputElement = InputElement;

    constexpr OutputElement operator()(
        InputElement const& input_element) const noexcept
    {
        // The use of volatile prevends the optimizing compiler
        // to remove this iteration
        volatile InputElement nr_iterations = input_element;
        lue_hpx_assert(nr_iterations >= InputElement{0});

        while(nr_iterations > InputElement{0}) {
            --nr_iterations;
        }

        return input_element;
    }

};

}  // namespace detail


namespace policy::iterate_per_element {

    template<
        typename Element>
    using DefaultPolicies = policy::DefaultPolicies<
        AllValuesWithinDomain<Element>,
        OutputElements<Element>,
        InputElements<Element>>;

}  // namespace policy::iterate_per_element


/*!
    @brief      Per cell in a partitioned array, iterate a number of
                times before copying the cell to the result

    This algorithm has the same effect as the copy algorithm, but spends
    some more time. It is useful when simulating a configurable amount
    of load per cell.
*/
template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> iterate_per_element(
    PartitionedArray<Element, rank> const& input_array)
{
    using Functor = detail::IteratePerElement<Element>;
    using Policies = policy::iterate_per_element::DefaultPolicies<Element>;

    return unary_local_operation(Policies{}, input_array, Functor{});
}

}  // namespace lue
