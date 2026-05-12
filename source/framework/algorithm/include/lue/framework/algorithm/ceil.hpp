#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<std::floating_point InputElement>
        class Ceil
        {

            public:

                static constexpr char const* name{"ceil"};

                using OutputElement = InputElement;


                auto operator()(InputElement const& input_element) const noexcept -> OutputElement
                {
                    return std::ceil(input_element);
                }
        };

    }  // namespace detail


    LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(
        ceil, detail::Ceil, std::floating_point)

}  // namespace lue
