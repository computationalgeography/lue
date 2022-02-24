#pragma once
#include "lue/framework/algorithm/cast.hpp"
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
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
        typename Policies,
        typename InputElement,
        Rank rank>
    PartitionedArray<OutputElement, rank> cast(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& array)
    {
        return unary_local_operation(policies, array, detail::Cast<InputElement, OutputElement>{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_CAST(Policies, OutputElement, InputElement, rank)       \
                                                                                \
    template LUE_LOCAL_OPERATION_EXPORT                                         \
    PartitionedArray<OutputElement, rank>                                       \
        cast<OutputElement, ArgumentType<void(Policies)>, InputElement, rank>(  \
            ArgumentType<void(Policies)> const&,                                \
            PartitionedArray<InputElement, rank> const&);
