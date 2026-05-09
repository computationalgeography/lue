#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/concept.hpp"


namespace lue {
    namespace detail {

        template<SignedIntegralOrFloatingPoint InputElement>
        class Negate
        {

            public:

                static constexpr char const* name{"negate"};

                using OutputElement = InputElement;


                auto operator()(InputElement const& input_element) const noexcept -> OutputElement
                {
                    return -input_element;
                }
        };

    }  // namespace detail


    LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(
        negate, detail::Negate, SignedIntegralOrFloatingPoint)

}  // namespace lue
