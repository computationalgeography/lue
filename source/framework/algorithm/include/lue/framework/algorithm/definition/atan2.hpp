#pragma once
#include "lue/framework/algorithm/atan2.hpp"
#include "lue/framework/algorithm/definition/binary_local_operation.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<typename InputElement>
        class ATan2
        {

            public:

                static_assert(std::is_floating_point_v<InputElement>);

                using OutputElement = InputElement;

                OutputElement operator()(
                    InputElement const input_element1, InputElement const input_element2) const noexcept
                {
                    return std::atan2(input_element1, input_element2);
                }
        };

    }  // namespace detail


    template<typename Policies, typename Element, Rank rank>
    PartitionedArray<Element, rank> atan2(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array1,
        PartitionedArray<Element, rank> const& array2)
    {
        return binary_local_operation(policies, array1, array2, detail::ATan2<Element>{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ATAN2(Policies, Element, rank)                                                       \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    atan2<ArgumentType<void(Policies)>, Element, rank>(                                                      \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<Element, rank> const&,                                                              \
        PartitionedArray<Element, rank> const&);
