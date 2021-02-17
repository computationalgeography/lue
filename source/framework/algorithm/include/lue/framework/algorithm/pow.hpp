#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include <cmath>
#include <limits>


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_=InputElement>
class Pow
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    using OutputElement = OutputElement_;

    OutputElement operator()(
        InputElement const input_element,
        InputElement const exponent) const noexcept
    {
        return std::pow(input_element, exponent);
    }

};

}  // namespace detail


namespace policy::pow {

    template<
        typename OutputElement,
        typename InputElement>
    using DefaultPolicies = policy::DefaultPolicies<
        AllValuesWithinDomain<InputElement, InputElement>,
        OutputElements<OutputElement>,
        InputElements<InputElement, InputElement>>;

    template<
        typename OutputElement,
        typename InputElement>
    using DefaultValuePolicies = policy::DefaultValuePolicies<
        AllValuesWithinDomain<InputElement, InputElement>,
        OutputElements<OutputElement>,
        InputElements<InputElement, InputElement>>;

}  // namespace policy::pow


LUE_BINARY_LOCAL_OPERATION_OVERLOADS(pow, detail::Pow)

}  // namespace lue
