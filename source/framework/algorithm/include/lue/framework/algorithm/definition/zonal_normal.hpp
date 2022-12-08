#pragma once
#include "lue/framework/algorithm/zonal_normal.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include "lue/framework/algorithm/definition/reclassify.hpp"
#include "lue/framework/algorithm/unique.hpp"
#include <algorithm>
#include <iterator>
#include <random>


namespace lue {

    template<
        typename Element,
        typename Policies,
        typename Zone,
        Rank rank>
    PartitionedArray<Element, rank> zonal_normal(
        [[maybe_unused]] Policies const& policies,
        PartitionedArray<Zone, rank> const& zones)
    {
        static_assert(std::is_floating_point_v<Element>);

        // This function is different from the other zonal operations in that there is no
        // array argument. It is not needed. We only need a list of unique zones, associate a
        // random value with each zone, and reclass a partition given the zone - value map.
        // zonal_normal can do the same. Only difference is the way values are determined per zone.

        // Determine unique values in zones array passed in

        // TODO Use Policies type to create ReclassifyPolicies
        using UniquePolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<Zone>,
            policy::OutputElements<Zone>,
            policy::InputElements<Zone>>;

        auto unique_zones_f = unique(UniquePolicies{}, zones);

        // Associate a new value, from the normal distribution, per zone
        auto zone_value_map_f = unique_zones_f.then(

                [](auto&& unique_zones_f)
                {
                    std::set<Zone> const& unique_zones{unique_zones_f.get()};
                    LookupTable<Zone, Element> zone_value_map;

                    // Will be used to obtain a seed for the random number engine
                    std::random_device random_device;

                    // Standard mersenne_twister_engine seeded with the random_device
                    std::mt19937 random_number_engine(random_device());

                    // Return values from normal distribution with mean 0 and standard deviation 1
                    auto distribution = std::normal_distribution<Element>{0, 1};

                    std::transform(
                            unique_zones.begin(), unique_zones.end(),
                            std::inserter(zone_value_map, zone_value_map.end()),

                            [random_number_engine, distribution](Zone const zone) mutable
                            {
                                return std::pair<Zone const, Element>{
                                    zone, distribution(random_number_engine)};
                            }

                        );

                    lue_hpx_assert(zone_value_map.size() == unique_zones.size());

                    return zone_value_map;
                }

            );

        // TODO Use Policies type to create ReclassifyPolicies
        using ReclassifyPolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<Zone>,
            policy::OutputElements<Element>,
            policy::InputElements<Zone>>;

        // Reclassify zones by values
        return reclassify(ReclassifyPolicies{}, zones, zone_value_map_f.share());
    }

}  // namespace lue


#define LUE_INSTANTIATE_ZONAL_NORMAL(                         \
    Policies, Element, Zone)                                  \
                                                              \
    template LUE_ZONAL_OPERATION_EXPORT                       \
    PartitionedArray<Element, 2> zonal_normal<                \
            Element, ArgumentType<void(Policies)>, Zone, 2>(  \
        ArgumentType<void(Policies)> const&,                  \
        PartitionedArray<Zone, 2> const&);
