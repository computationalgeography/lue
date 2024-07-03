#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include <cmath>
#include <type_traits>


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Modulus
        {

            public:

                using OutputElement = OutputElement_;


                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                {

                    if constexpr (std::is_integral_v<InputElement>)
                    {
                        return input_element1 % input_element2;
                    }
                    else if constexpr (std::is_floating_point_v<InputElement>)
                    {
                        return std::fmod(input_element1, input_element2);
                    }
                    else
                    {
                        static_assert(
                            std::is_arithmetic_v<InputElement>,
                            "Modulus operation requires arithmetic types");
                    }
                }
        };

    }  // namespace detail


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(modulus, detail::Modulus)

}  // namespace lue
