#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"


namespace lue {
    namespace detail {

        template<Arithmetic InputElement, Arithmetic OutputElement_>
        class Cast
        {

            public:

                static constexpr char const* name{"cast"};

                using OutputElement = OutputElement_;


                constexpr auto operator()(InputElement const& input_element) const noexcept -> OutputElement
                {
                    return static_cast<OutputElement>(input_element);
                }
        };

    }  // namespace detail


    LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
        cast, detail::Cast, Arithmetic, Arithmetic)

}  // namespace lue
