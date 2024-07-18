#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"
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
                    static_assert(
                        std::is_integral_v<InputElement>,
                        "Input arguments for modulus must be an integral type");

                    return input_element1 % input_element2;
                }
        };

    }  // namespace detail

    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(modulus, detail::Modulus)

}  // namespace lue
