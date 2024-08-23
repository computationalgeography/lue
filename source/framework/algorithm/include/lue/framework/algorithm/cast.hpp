#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_>
        class Cast
        {

            public:

                using OutputElement = OutputElement_;

                static_assert(std::is_arithmetic_v<InputElement>);
                static_assert(std::is_arithmetic_v<OutputElement>);


                constexpr auto operator()(InputElement const& input_element) const noexcept -> OutputElement
                {
                    return static_cast<OutputElement>(input_element);
                }
        };

    }  // namespace detail


    LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_DIFFERENT_OUTPUT_ELEMENT(cast, detail::Cast)

}  // namespace lue
