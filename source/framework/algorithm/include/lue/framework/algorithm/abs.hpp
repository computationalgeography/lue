#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/concept.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<SignedIntegralOrFloatingPoint InputElement>
        class Abs
        {

            public:

                static constexpr char const* name{"abs"};

                using OutputElement = InputElement;


                auto operator()(InputElement const input_element) const noexcept -> OutputElement
                {
                    return std::abs(input_element);
                }
        };

    }  // namespace detail


    LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(
        abs, detail::Abs, SignedIntegralOrFloatingPoint)

}  // namespace lue
