#pragma once
#include "lue/framework/algorithm/sqrt.hpp"
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/framework/algorithm/export.hpp"
#include "lue/macro.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<
            typename InputElement>
        class Sqrt
        {

            public:

                static_assert(std::is_floating_point_v<InputElement>);

                using OutputElement = InputElement;

                OutputElement operator()(
                    InputElement const& input_element) const noexcept
                {
                    return std::sqrt(input_element);
                }

        };

    }  // namespace detail


    template<
        typename Policies,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> sqrt(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array)
    {
        return unary_local_operation(policies, array, detail::Sqrt<Element>{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_SQRT(Policies, Element, rank)                                   \
                                                                                        \
    template LUE_FA_EXPORT                                                              \
    PartitionedArray<Element, rank> sqrt<ArgumentType<void(Policies)>, Element, rank>(  \
        ArgumentType<void(Policies)> const&,                                            \
        PartitionedArray<Element, rank> const&);
