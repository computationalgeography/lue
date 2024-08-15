#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = std::uint8_t>
        class LogicalAnd
        {

            public:

                static_assert(std::is_integral_v<InputElement>);

                using OutputElement = OutputElement_;


                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                {
                    return input_element1 && input_element2;
                }
        };

    }  // namespace detail


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES(logical_and, detail::LogicalAnd)

}  // namespace lue
