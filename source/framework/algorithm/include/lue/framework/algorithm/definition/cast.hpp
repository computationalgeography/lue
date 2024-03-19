#pragma once
#include "lue/framework/algorithm/cast.hpp"
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/macro.hpp"
#include <type_traits>


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_>
        class Cast
        {

            public:

                using OutputElement = OutputElement_;

                static_assert(std::is_arithmetic_v<InputElement>);
                static_assert(std::is_arithmetic_v<OutputElement>);


                constexpr auto operator()(InputElement const& input_element) const noexcept -> OutputElement
                {
                    return static_cast<OutputElement>(input_element);
                }
        };

    }  // namespace detail


    template<typename OutputElement, typename Policies, typename InputElement, Rank rank>
    auto cast(Policies const& policies, PartitionedArray<InputElement, rank> const& array)
        -> PartitionedArray<OutputElement, rank>
    {
        return unary_local_operation(policies, array, detail::Cast<InputElement, OutputElement>{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_CAST(Policies, OutputElement, InputElement, rank)                                    \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<OutputElement, rank>                                \
    cast<OutputElement, ArgumentType<void(Policies)>, InputElement, rank>(                                   \
        ArgumentType<void(Policies)> const&, PartitionedArray<InputElement, rank> const&);
