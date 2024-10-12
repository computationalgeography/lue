#pragma once
#include "lue/framework/algorithm/definition/zonal_operation.hpp"
#include "lue/framework/algorithm/zonal_mean.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include <unordered_map>


namespace lue {
    namespace detail {

        template<typename InputElement, typename Zone>
        class ZonalMean
        {

            public:

                static_assert(std::is_arithmetic_v<InputElement>);

                static_assert(std::is_integral_v<Zone>);

                static constexpr char const* name{"zonal_mean"};

                using OutputElement = InputElement;

                using Sum = InputElement;
                using Count = std::uint64_t;


                class Aggregator
                {

                    public:

                        using Statistic = std::tuple<Sum, Count>;

                        using Map = std::unordered_map<Zone, Statistic>;


                        void add(Zone const zone, Statistic const& statistic)
                        {
                            auto it{_statistic_by_zone.find(zone)};

                            if (it == _statistic_by_zone.end())
                            {
                                _statistic_by_zone[zone] = statistic;
                            }
                            else
                            {
                                auto& [this_sum, this_count] = (*it).second;
                                auto const [other_sum, other_count] = statistic;

                                this_sum += other_sum;
                                this_count += other_count;
                            }
                        }


                        void add(Zone const zone, InputElement const value)
                        {
                            add(zone, Statistic{value, 1});
                        }


                        void merge(Aggregator const& other)
                        {
                            for (auto const& [zone, value] : other._statistic_by_zone)
                            {
                                add(zone, value);
                            }
                        }


                        bool contains(Zone const zone) const
                        {
                            return _statistic_by_zone.find(zone) != _statistic_by_zone.end();
                        }


                        OutputElement operator[](Zone const zone) const
                        {
                            auto it{_statistic_by_zone.find(zone)};

                            lue_hpx_assert(it != _statistic_by_zone.end());

                            auto [sum, count] = (*it).second;

                            return sum / count;
                        }


                    private:

                        friend class hpx::serialization::access;


                        template<typename Archive>
                        void serialize(Archive& archive, unsigned int const /* version */)
                        {
                            archive& _statistic_by_zone;
                        }


                        Map _statistic_by_zone;
                };
        };

    }  // namespace detail


    template<typename Policies, typename Element, typename Zone, Rank rank>
    PartitionedArray<Element, rank> zonal_mean(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array,
        PartitionedArray<Zone, rank> const& zones)
    {
        using Functor = detail::ZonalMean<Element, Zone>;

        return zonal_operation(policies, array, zones, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ZONAL_MEAN(Policies, Element, Zone)                                                  \
                                                                                                             \
    template LUE_ZONAL_OPERATION_EXPORT PartitionedArray<Element, 2>                                         \
    zonal_mean<ArgumentType<void(Policies)>, Element, Zone, 2>(                                              \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<Element, 2> const&,                                                                 \
        PartitionedArray<Zone, 2> const&);
