#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_=InputElement>
class Divide
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    static_assert(std::numeric_limits<InputElement>::has_quiet_NaN);

    using OutputElement = OutputElement_;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        // TODO(KDJ) Move this test to a domain policy used by the binary
        //     local operation
        return input_element2 == 0
            ? std::numeric_limits<OutputElement>::quiet_NaN()
            : input_element1 / input_element2
            ;
    }

};

}  // namespace detail


namespace policy {
namespace divide {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace divide
}  // namespace policy


LUE_BINARY_LOCAL_OPERATION_OVERLOADS(divide, detail::Divide)

}  // namespace lue
