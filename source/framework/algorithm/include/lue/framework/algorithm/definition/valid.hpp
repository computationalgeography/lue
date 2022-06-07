#pragma once
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/framework/algorithm/valid.hpp"
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<
            typename InputElement,
            typename OutputElement_=std::uint8_t>
        class Valid
        {

            public:

                using OutputElement = OutputElement_;


                constexpr OutputElement operator()(
                    [[maybe_unused]] InputElement const& input_element) const noexcept
                {
                    return 1;
                }

        };

    }  // namespace detail


    template<
        typename BooleanElement,
        typename Policies,
        typename ExpressionElement,
        Rank rank>
    PartitionedArray<BooleanElement, rank> valid(
        Policies const& policies,
        PartitionedArray<ExpressionElement, rank> const& array)
    {
        return unary_local_operation(policies, array, detail::Valid<ExpressionElement>{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_VALID(Policies, BooleanElement, ExpressionElement, rank)     \
                                                                                     \
    template LUE_LOCAL_OPERATION_EXPORT                                              \
    PartitionedArray<BooleanElement, rank> valid<                                    \
            BooleanElement, ArgumentType<void(Policies)>, ExpressionElement, rank>(  \
        ArgumentType<void(Policies)> const&,                                         \
        PartitionedArray<ExpressionElement, rank> const&);
