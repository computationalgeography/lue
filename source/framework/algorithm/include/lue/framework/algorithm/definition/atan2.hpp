#pragma once
#include "lue/framework/algorithm/atan2.hpp"
#include "lue/framework/algorithm/definition/binary_local_operation.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<std::floating_point InputElement>
        class ATan2
        {

            public:

                static constexpr char const* name{"atan2"};

                using OutputElement = InputElement;


                auto operator()(InputElement const input_element1, InputElement const input_element2)
                    const noexcept -> OutputElement
                {
                    return std::atan2(input_element1, input_element2);
                }
        };

    }  // namespace detail


    template<typename Policies, typename Element, Rank rank>
        requires(
            std::floating_point<policy::InputElementT<Policies, 0>> &&
            std::floating_point<policy::InputElementT<Policies, 1>> &&
            std::same_as<policy::InputElementT<Policies, 0>, policy::InputElementT<Policies, 1>> &&
            std::floating_point<policy::OutputElementT<Policies, 0>>)
    auto atan2(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array1,
        PartitionedArray<Element, rank> const& array2) -> PartitionedArray<Element, rank>
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
