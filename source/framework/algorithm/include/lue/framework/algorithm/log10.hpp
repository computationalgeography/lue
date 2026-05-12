#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<std::floating_point InputElement>
        class Log10
        {

            public:

                static constexpr char const* name{"log10"};

                using OutputElement = InputElement;


                auto operator()(InputElement const& input_element) const noexcept -> OutputElement
                {
                    return std::log10(input_element);
                }
        };

    }  // namespace detail


    namespace policy::log10 {

        template<std::floating_point Element>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(Element const element) noexcept -> bool
                {
                    return element >= 0;
                }
        };

    }  // namespace policy::log10


    LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(
        log10, detail::Log10, std::floating_point)

}  // namespace lue
