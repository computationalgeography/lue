#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<std::floating_point InputElement>
        class Round
        {

            public:

                static constexpr char const* name{"round"};

                using OutputElement = InputElement;


                auto operator()(InputElement const& input_element) const noexcept -> OutputElement
                {
                    return std::round(input_element);
                }
        };

    }  // namespace detail


    LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(
        round, detail::Round, std::floating_point)

}  // namespace lue
