#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
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

    /// static_assert(std::numeric_limits<InputElement>::has_quiet_NaN);

    using OutputElement = OutputElement_;

    OutputElement operator()(
        InputElement const input_element,
        InputElement const exponent) const noexcept
    {
        return std::pow(input_element, exponent);
    }

};

}  // namespace detail


namespace policy {
namespace pow {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace pow
}  // namespace policy


LUE_BINARY_LOCAL_OPERATION_OVERLOADS(pow, detail::Pow)

}  // namespace lue
