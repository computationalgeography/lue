#pragma once
#include "lue/framework/algorithm/definition/zonal_operation.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include "lue/framework/algorithm/zonal_sum.hpp"
#include <unordered_map>


namespace lue {
    namespace detail {

        template<typename InputElement, typename Zone>
        class ZonalSum
        {

            public:

                static_assert(std::is_arithmetic_v<InputElement>);

                static_assert(std::is_integral_v<Zone>);

                using OutputElement = InputElement;


                class Aggregator
                {

                    public:

                        using Map = std::unordered_map<Zone, OutputElement>;


                        void add(Zone const zone, InputElement const value)
                        {
                            if (_sum_by_zone.find(zone) == _sum_by_zone.end())
                            {
                                _sum_by_zone[zone] = value;
                            }
                            else
                            {
                                _sum_by_zone[zone] += value;
                            }
                        }


                        void merge(Aggregator const& other)
                        {
                            for (auto const& [zone, value] : other._sum_by_zone)
                            {
                                add(zone, value);
                            }
                        }


                        bool contains(Zone const zone) const
                        {
                            return _sum_by_zone.find(zone) != _sum_by_zone.end();
                        }


                        OutputElement operator[](Zone const zone) const
                        {
                            auto it{_sum_by_zone.find(zone)};

                            lue_hpx_assert(it != _sum_by_zone.end());

                            return (*it).second;
                        }


                    private:

                        friend class hpx::serialization::access;


                        template<typename Archive>
                        void serialize(Archive& archive, unsigned int const /* version */)
                        {
                            archive& _sum_by_zone;
                        }


                        Map _sum_by_zone;
                };
        };

    }  // namespace detail


    template<typename Policies, typename Element, typename Zone, Rank rank>
    PartitionedArray<Element, rank> zonal_sum(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array,
        PartitionedArray<Zone, rank> const& zones)
    {
        using Functor = detail::ZonalSum<Element, Zone>;

        return zonal_operation(policies, array, zones, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ZONAL_SUM(Policies, Element, Zone)                                                   \
                                                                                                             \
    template LUE_ZONAL_OPERATION_EXPORT PartitionedArray<Element, 2>                                         \
    zonal_sum<ArgumentType<void(Policies)>, Element, Zone, 2>(                                               \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<Element, 2> const&,                                                                 \
        PartitionedArray<Zone, 2> const&);
