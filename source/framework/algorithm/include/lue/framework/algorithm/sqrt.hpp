#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<typename InputElement>
        class Sqrt
        {

            public:

                static_assert(std::is_floating_point_v<InputElement>);

                static constexpr char const* name{"sqrt"};

                using OutputElement = InputElement;

                auto operator()(InputElement const& input_element) const noexcept -> OutputElement
                {
                    return std::sqrt(input_element);
                }
        };

    }  // namespace detail


    namespace policy::sqrt {

        template<typename Element>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(Element const element) noexcept -> bool
                {
                    return element >= 0;
                }
        };

    }  // namespace policy::sqrt


    LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(sqrt, detail::Sqrt)

}  // namespace lue
