#pragma once
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/framework/algorithm/log10.hpp"
#include "lue/macro.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<typename InputElement>
        class Log10
        {

            public:

                static_assert(std::is_floating_point_v<InputElement>);

                using OutputElement = InputElement;


                OutputElement operator()(InputElement const& input_element) const noexcept
                {
                    return std::log10(input_element);
                }
        };

    }  // namespace detail


    template<typename Policies, typename Element, Rank rank>
    PartitionedArray<Element, rank> log10(
        Policies const& policies, PartitionedArray<Element, rank> const& array)
    {
        return unary_local_operation(policies, array, detail::Log10<Element>{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_LOG10(Policies, Element, rank)                                                       \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    log10<ArgumentType<void(Policies)>, Element, rank>(                                                      \
        ArgumentType<void(Policies)> const&, PartitionedArray<Element, rank> const&);
