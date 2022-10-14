#pragma once
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/framework/algorithm/abs.hpp"
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/macro.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<
            typename InputElement>
        class Abs
        {

            public:

                static_assert(std::is_signed_v<InputElement> || std::is_floating_point_v<InputElement>);

                using OutputElement = InputElement;

                OutputElement operator()(
                    InputElement const& input_element) const noexcept
                {
                    return std::abs(input_element);
                }

        };

    }  // namespace detail


    template<
        typename Policies,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> abs(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array)
    {
        return unary_local_operation(policies, array, detail::Abs<Element>{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ABS(Policies, Element, rank)                                   \
                                                                                       \
    template LUE_LOCAL_OPERATION_EXPORT                                                \
    PartitionedArray<Element, rank> abs<ArgumentType<void(Policies)>, Element, rank>(  \
        ArgumentType<void(Policies)> const&,                                           \
        PartitionedArray<Element, rank> const&);
