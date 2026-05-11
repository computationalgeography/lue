#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<std::floating_point InputElement, std::floating_point OutputElement_ = InputElement>
        class Pow
        {

            public:

                static constexpr char const* name{"pow"};

                using OutputElement = InputElement;


                auto operator()(InputElement const input_element, InputElement const exponent) const noexcept
                    -> OutputElement
                {
                    return std::pow(input_element, exponent);
                }
        };

    }  // namespace detail


    namespace policy::pow {

        template<std::floating_point InputElement>
        using DomainPolicy = AllValuesWithinDomain<InputElement, InputElement>;

    }


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(
        pow, detail::Pow, std::floating_point)

}  // namespace lue
