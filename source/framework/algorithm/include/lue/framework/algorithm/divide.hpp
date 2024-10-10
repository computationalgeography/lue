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

                static constexpr char const* name{"divide"};

                using OutputElement = OutputElement_;


                constexpr auto operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                    -> OutputElement
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

                static constexpr auto within_domain(
                    [[maybe_unused]] Element const numerator, Element const denominator) noexcept -> bool
                {
                    return denominator != 0;
                }
        };

    }  // namespace policy::divide


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(divide, detail::Divide)

}  // namespace lue
