#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::log {

        template<typename Element>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(Element const element) noexcept
                {
                    return element >= 0;
                }
        };

    }  // namespace policy::log


    template<typename Policies, typename Element, Rank rank>
    PartitionedArray<Element, rank> log(
        Policies const& policies, PartitionedArray<Element, rank> const& array);

}  // namespace lue
