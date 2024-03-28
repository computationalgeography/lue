#pragma once
#include "lue/framework/algorithm/clump.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail::clump {


    }  // namespace detail::clump


    template<typename Policies, Rank rank>
    auto clump(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& zone)
            -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        static_assert(rank == 2);

        using ZoneElement = policy::InputElementT<Policies, 0>;

        static_assert(std::is_same_v<ZoneElement, policy::OutputElementT<Policies, 0>>);


        // TODO




    }

}  // namespace lue


#define LUE_INSTANTIATE_CLUMP(Policies, rank)                                                           \
                                                                                                             \
    template LUE_ZONAL_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies, 0>, rank>          \
    clump<ArgumentType<void(Policies)>, rank>(                                                          \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&);
