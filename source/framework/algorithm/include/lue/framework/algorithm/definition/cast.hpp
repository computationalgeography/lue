#pragma once
#include "lue/framework/algorithm/cast.hpp"
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/framework/algorithm/export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<
            typename InputElement,
            typename OutputElement_>
        class Cast
        {

            public:

                using OutputElement = OutputElement_;


                constexpr OutputElement operator()(
                    InputElement const& input_element) const noexcept
                {
                    return static_cast<OutputElement>(input_element);
                }

            };

    }  // namespace detail


    template<
        typename OutputElement,
        typename InputArray>
    PartitionedArrayT<InputArray, OutputElement> cast(
        InputArray const& array)
    {
        using InputElement = ElementT<InputArray>;
        using Functor = detail::Cast<InputElement, OutputElement>;
        using Policies = policy::cast::DefaultPolicies<OutputElement, InputElement>;

        return unary_local_operation(Policies{}, array, Functor{});
    }

}  // namespace lue
