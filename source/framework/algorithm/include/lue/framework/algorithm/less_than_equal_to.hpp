#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"


namespace lue {
    namespace detail {

        template<
            typename InputElement,
            typename OutputElement_=std::uint8_t>
        class LessThanEqualTo
        {

        public:

            using OutputElement = OutputElement_;

            constexpr OutputElement operator()(
                InputElement const& input_element1,
                InputElement const& input_element2) const noexcept
            {
                return input_element1 <= input_element2;
            }

        };

    }  // namespace detail


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(less_than_equal_to, detail::LessThanEqualTo)

}  // namespace lue
