#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_=InputElement>
class Subtract
{

public:

    using OutputElement = OutputElement_;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 - input_element2;
    }

};

}  // namespace detail


namespace policy {
namespace subtract {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace subtract
}  // namespace policy


LUE_BINARY_LOCAL_OPERATION_OVERLOADS(subtract, detail::Subtract)

}  // namespace lue
