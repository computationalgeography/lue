#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_=bool>
class LogicalInclusiveOr
{

public:

    using OutputElement = OutputElement_;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 || input_element2;
    }

};

}  // namespace detail


namespace policy {
namespace logical_inclusive_or {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace logical_inclusive_or
}  // namespace policy


LUE_BINARY_LOCAL_OPERATION_OVERLOADS(logical_inclusive_or, detail::LogicalInclusiveOr)

}  // namespace lue
