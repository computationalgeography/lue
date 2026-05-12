#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy.hpp"


namespace lue {
    namespace detail {

        template<std::integral InputElement, std::integral OutputElement_>
        class LogicalExclusiveOr
        {

            public:

                static constexpr char const* name{"logical_exclusive_or"};

                using OutputElement = OutputElement_;


                constexpr auto operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                    -> OutputElement
                {
                    return bool(input_element1) != bool(input_element2);
                }
        };

    }  // namespace detail


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
        logical_exclusive_or, detail::LogicalExclusiveOr, std::integral, std::integral)

}  // namespace lue
