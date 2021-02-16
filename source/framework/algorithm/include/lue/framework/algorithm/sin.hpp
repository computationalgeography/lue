#pragma once
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include <cmath>


namespace lue {
namespace detail {

template<
    typename InputElement>
class Sin
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    using OutputElement = InputElement;

    OutputElement operator()(
        InputElement const& input_element) const noexcept
    {
        return std::sin(input_element);
    }

};

}  // namespace detail


namespace policy::sin {

    template<
        typename Element>
    using DefaultPolicies = policy::DefaultPolicies<
        AllValuesWithinDomain<Element>,
        OutputElements<Element>,
        InputElements<Element>>;

    template<
        typename Element>
    using DefaultValuePolicies = policy::DefaultValuePolicies<
        AllValuesWithinDomain<Element>,
        OutputElements<Element>,
        InputElements<Element>>;

}  // namespace policy::sin


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> sin(
    PartitionedArray<Element, rank> const& array)
{
    using Functor = detail::Sin<Element>;
    using Policies = policy::sin::DefaultPolicies<Element>;

    return unary_local_operation(Policies{}, array, Functor{});
}

}  // namespace lue
