#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Divide
        {

            public:

                static_assert(std::is_floating_point_v<InputElement>);

                using OutputElement = OutputElement_;


                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                {
                    return input_element1 / input_element2;
                }
        };

    }  // namespace detail


    namespace policy::divide {

        template<typename Element>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(
                    [[maybe_unused]] Element const numerator, Element const denominator) noexcept
                {
                    return denominator != 0;
                }
        };

    }  // namespace policy::divide


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(divide, detail::Divide)

}  // namespace lue
