#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy.hpp"


namespace lue {
    namespace detail {

        template<
            typename InputElement,
            typename OutputElement_=std::uint8_t>
        class LogicalExclusiveOr
        {

            public:

                static_assert(std::is_integral_v<InputElement>);

                using OutputElement = OutputElement_;


                constexpr OutputElement operator()(
                    InputElement const& input_element1,
                    InputElement const& input_element2) const noexcept
                {
                    return bool(input_element1) != bool(input_element2);
                }

        };

    }  // namespace detail


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(logical_exclusive_or, detail::LogicalExclusiveOr)

}  // namespace lue
