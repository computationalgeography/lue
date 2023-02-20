#pragma once
#include "lue/framework/algorithm/definition/zonal_operation.hpp"
#include "lue/framework/algorithm/zonal_minimum.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include <unordered_map>


namespace lue {
    namespace detail {

        template<typename InputElement, typename Zone>
        class ZonalMinimum
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
                            auto [it, inserted] = _minimum_by_zone.try_emplace(zone, value);

                            (*it).second = std::min((*it).second, value);
                        }


                        void merge(Aggregator const& other)
                        {
                            for (auto const& [zone, value] : other._minimum_by_zone)
                            {
                                add(zone, value);
                            }
                        }


                        bool contains(Zone const zone) const
                        {
                            return _minimum_by_zone.find(zone) != _minimum_by_zone.end();
                        }


                        OutputElement operator[](Zone const zone) const
                        {
                            auto it{_minimum_by_zone.find(zone)};

                            lue_hpx_assert(it != _minimum_by_zone.end());

                            return (*it).second;
                        }


                    private:

                        friend class hpx::serialization::access;


                        template<typename Archive>
                        void serialize(Archive& archive, unsigned int const /* version */)
                        {
                            archive& _minimum_by_zone;
                        }


                        Map _minimum_by_zone;
                };
        };

    }  // namespace detail


    template<typename Policies, typename Element, typename Zone, Rank rank>
    PartitionedArray<Element, rank> zonal_minimum(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array,
        PartitionedArray<Zone, rank> const& zones)
    {
        using Functor = detail::ZonalMinimum<Element, Zone>;

        return zonal_operation(policies, array, zones, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ZONAL_MINIMUM(Policies, Element, Zone)                                               \
                                                                                                             \
    template LUE_ZONAL_OPERATION_EXPORT PartitionedArray<Element, 2>                                         \
    zonal_minimum<ArgumentType<void(Policies)>, Element, Zone, 2>(                                           \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<Element, 2> const&,                                                                 \
        PartitionedArray<Zone, 2> const&);
