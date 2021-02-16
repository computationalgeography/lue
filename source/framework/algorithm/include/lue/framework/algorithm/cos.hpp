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
class Cos
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    using OutputElement = InputElement;

    OutputElement operator()(
        InputElement const& input_element) const noexcept
    {
        return std::cos(input_element);
    }

};

}  // namespace detail


namespace policy::cos {

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

}  // namespace policy::cos


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> cos(
    PartitionedArray<Element, rank> const& array)
{
    using Functor = detail::Cos<Element>;
    using Policies = policy::cos::DefaultPolicies<Element>;

    return unary_local_operation(Policies{}, array, Functor{});
}

}  // namespace lue
