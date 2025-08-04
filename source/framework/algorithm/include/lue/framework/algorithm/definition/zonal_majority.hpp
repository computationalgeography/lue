#pragma once
#include "lue/framework/algorithm/definition/zonal_operation.hpp"
#include "lue/framework/algorithm/zonal_majority.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include <unordered_map>


namespace lue {
    namespace detail {

        template<typename InputElement, typename Zone>
        class ZonalMajority
        {

            public:

                static_assert(std::is_integral_v<InputElement>);
                static_assert(std::is_integral_v<Zone>);

                static constexpr char const* name{"zonal_majority"};

                using OutputElement = InputElement;


                class Aggregator
                {

                    public:

                        // Per zone we need to keep track of the frequency of the values
                        using Statistic = std::unordered_map<InputElement, Count>;

                        using Map = std::unordered_map<Zone, Statistic>;


                        void add(Zone const zone, InputElement const value)
                        {
                            // Obtain a pointer to the map for keeping track of frequencies
                            // per value for the zone passed in. If necessary, add an empty map first.
                            auto [it1, inserted1] = _statistic_by_zone.try_emplace(zone, Statistic{});

                            // Obtain a pointer to the (value, frequency) pair for the value passed in.
                            // If necessary, add an (value, 0) pair first.
                            auto [it2, inserted2] = (*it1).second.try_emplace(value, 0);

                            // Increase the frequency of the current value within the current zone
                            ++(*it2).second;
                        }


                        void merge(Aggregator const& other)
                        {
                            for (auto const& [zone, other_frequencies] : other._statistic_by_zone)
                            {
                                if (!contains(zone))
                                {
                                    // We don't have any frequencies for this zone. Add 'm all.
                                    _statistic_by_zone[zone] = other_frequencies;
                                }
                                else
                                {
                                    auto& this_frequencies{_statistic_by_zone[zone]};

                                    for (auto const& [value, frequency] : other_frequencies)
                                    {
                                        if (this_frequencies.find(value) == this_frequencies.end())
                                        {
                                            // We don't have a frequency for the current
                                            // value. Add the value and frequency.
                                            this_frequencies[value] = frequency;
                                        }
                                        else
                                        {
                                            // We already have a frequency for the current
                                            // value. Add the frequency.
                                            this_frequencies[value] += frequency;
                                        }
                                    }
                                }
                            }
                        }


                        bool contains(Zone const zone) const
                        {
                            return _statistic_by_zone.find(zone) != _statistic_by_zone.end();
                        }


                        OutputElement operator[](Zone const zone) const
                        {
                            auto const& frequencies{(*_statistic_by_zone.find(zone)).second};

                            // TODO This could happen! Valid zone with only no-data values in it.
                            //      We need access to the ondp.
                            lue_hpx_assert(!frequencies.empty());

                            OutputElement majority_value{};

                            Count majority_value_frequency{0};

                            // Find value with largest frequency. If there are multiple, output
                            // ... what? Largest, first one encountered (in map), randomly
                            // pick one, no-data?
                            // TODO Whatever we pick, use the same rule in focal_majority

                            for (auto const& [value, frequency] : frequencies)
                            {
                                if (frequency > majority_value_frequency)
                                {
                                    // Potential result
                                    majority_value_frequency = frequency;
                                    majority_value = value;
                                }
                                else if (frequency == majority_value_frequency)
                                {
                                    if (value > majority_value)
                                    {
                                        // PCRaster convention...
                                        majority_value = value;
                                    }
                                }
                            }

                            return majority_value;
                        }


                    private:

                        friend class hpx::serialization::access;


                        template<typename Archive>
                        void serialize(Archive& archive, unsigned int const /* version */)
                        {
                            archive & _statistic_by_zone;
                        }


                        Map _statistic_by_zone;
                };
        };

    }  // namespace detail


    template<typename Policies, typename Element, typename Zone, Rank rank>
    PartitionedArray<Element, rank> zonal_majority(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array,
        PartitionedArray<Zone, rank> const& zones)
    {
        using Functor = detail::ZonalMajority<Element, Zone>;

        return zonal_operation(policies, array, zones, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ZONAL_MAJORITY(Policies, Element, Zone)                                              \
                                                                                                             \
    template LUE_ZONAL_OPERATION_EXPORT PartitionedArray<Element, 2>                                         \
    zonal_majority<ArgumentType<void(Policies)>, Element, Zone, 2>(                                          \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<Element, 2> const&,                                                                 \
        PartitionedArray<Zone, 2> const&);
