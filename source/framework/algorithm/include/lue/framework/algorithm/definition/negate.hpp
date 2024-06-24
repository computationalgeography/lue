#pragma once
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/framework/algorithm/negate.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement>
        class Negate
        {

            public:

                static_assert(std::is_signed_v<InputElement> || std::is_floating_point_v<InputElement>);

                using OutputElement = InputElement;

                auto operator()(InputElement const& input_element) const noexcept -> OutputElement
                {
                    return -input_element;
                }
        };

    }  // namespace detail


    template<typename Policies, Rank rank>
    auto negate(
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        return unary_local_operation(policies, array, detail::Negate<policy::InputElementT<Policies, 0>>{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_NEGATE(Policies, rank)                                                               \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies, 0>, rank>          \
    negate<ArgumentType<void(Policies)>, rank>(                                                              \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&);
