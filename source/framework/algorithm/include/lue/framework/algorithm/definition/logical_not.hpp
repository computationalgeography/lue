#pragma once
#include "lue/framework/algorithm/logical_not.hpp"
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<
            typename InputElement>
        class LogicalNot
        {

            public:

                static_assert(std::is_integral_v<InputElement>);

                using OutputElement = std::uint8_t;


                constexpr OutputElement operator()(
                    InputElement const& input_element) const noexcept
                {
                    return !input_element;
                }

        };

    }  // namespace detail


    template<
        typename Policies,
        typename InputElement,
        Rank rank>
    PartitionedArray<std::uint8_t, rank> logical_not(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& array)
    {
        return unary_local_operation(policies, array, detail::LogicalNot<InputElement>{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_LOGICAL_NOT(Policies, Element, rank)  \
                                                              \
    template LUE_LOCAL_OPERATION_EXPORT                       \
    PartitionedArray<std::uint8_t, rank> logical_not<         \
            ArgumentType<void(Policies)>, Element, rank>(     \
        ArgumentType<void(Policies)> const&,                  \
        PartitionedArray<Element, rank> const&);
