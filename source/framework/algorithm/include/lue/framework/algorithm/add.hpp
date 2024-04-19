#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Add
        {

            public:

                using OutputElement = OutputElement_;

                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                {
                    return input_element1 + input_element2;
                }
        };

    }  // namespace detail


    namespace policy::add {

        template<typename InputElement>
        using DomainPolicy = AllValuesWithinDomain<InputElement, InputElement>;

    }  // namespace policy::add


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS4(add, detail::Add)

}  // namespace lue
