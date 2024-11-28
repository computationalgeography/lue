#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_>
        class LogicalNot
        {

            public:

                static_assert(std::is_integral_v<InputElement>);
                static_assert(std::is_integral_v<OutputElement_>);

                static constexpr char const* name{"logical_not"};

                using OutputElement = OutputElement_;


                constexpr auto operator()([[maybe_unused]] InputElement const& input_element) const noexcept
                    -> OutputElement
                {
                    return !input_element;
                }
        };

    }  // namespace detail


    LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
        logical_not, detail::LogicalNot)

}  // namespace lue
