#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<typename InputElement>
        class Abs
        {

            public:

                static_assert(std::is_signed_v<InputElement> || std::is_floating_point_v<InputElement>);

                using OutputElement = InputElement;

                auto operator()(InputElement const& input_element) const noexcept -> OutputElement
                {
                    return std::abs(input_element);
                }
        };

    }  // namespace detail


    LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(abs, detail::Abs)

}  // namespace lue
