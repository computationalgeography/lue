#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Pow
        {

            public:

                static_assert(std::is_floating_point_v<InputElement>);

                using OutputElement = OutputElement_;


                auto operator()(InputElement const input_element, InputElement const exponent) const noexcept
                    -> OutputElement
                {
                    return std::pow(input_element, exponent);
                }
        };

    }  // namespace detail


    namespace policy::pow {

        template<typename InputElement>
        using DomainPolicy = AllValuesWithinDomain<InputElement, InputElement>;

    }


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(pow, detail::Pow)

}  // namespace lue
